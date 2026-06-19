#ifdef __TURBO_USE_IMGUI__

#include <turbo/editor/Editor.hpp>
#include <turbo/Engine.hpp>
#include <turbo/DebugImgui.hpp>
#include <turbo/Utils.hpp>
#include <turbo/graphics/RectangleShape.hpp>
#include <turbo/graphics/CircleShape.hpp>
#include <turbo/graphics/Sprite.hpp>
#include <turbo/graphics/Color.hpp>
#include <turbo/graphics/Shader.hpp>
#include <turbo/graphics/Material.hpp>
#include <turbo/components/SimpleControl.hpp>
#include <turbo/script/LuaComponent.hpp>

#include <allegro5/allegro.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <set>
#include <filesystem>

namespace turbo::editor {

    namespace {
        // Shared flags for the fixed, tiled editor panels.
        const ImGuiWindowFlags PANEL_FLAGS =
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

        int count_tree(turbo::GameObject* go) {
            if (!go) return 0;
            int total = 1;
            for (turbo::GameObject* child : go->childs)
                total += count_tree(child);
            return total;
        }

        // Local transform of one node (mirrors Transform::get_transformer).
        ALLEGRO_TRANSFORM local_tf(turbo::GameObject* go) {
            ALLEGRO_TRANSFORM t;
            al_identity_transform(&t);
            const turbo::Vector2<float> s = go->get_scale();
            const turbo::Vector2<int>   c = go->get_center();
            const turbo::Vector2<float> p = go->get_position();
            al_scale_transform(&t, s.x, s.y);
            al_translate_transform(&t, -static_cast<float>(c.x), -static_cast<float>(c.y));
            al_rotate_transform(&t, static_cast<float>(DEGTORAD(go->get_angle())));
            al_translate_transform(&t, static_cast<float>(c.x) + p.x, static_cast<float>(c.y) + p.y);
            return t;
        }

        // Full world transform of a node: drawable space -> screen.
        ALLEGRO_TRANSFORM world_tf(turbo::GameObject* go) {
            ALLEGRO_TRANSFORM acc = local_tf(go);
            for (turbo::GameObject* n = go->get_parent(); n; n = n->get_parent()) {
                ALLEGRO_TRANSFORM nl = local_tf(n);
                al_compose_transform(&acc, &nl);  // apply acc, then n's local
            }
            return acc;
        }

        // Deep-copy a drawable by concrete type (sprites share their texture).
        turbo::Drawable* clone_drawable(turbo::Drawable* d) {
            if (auto* r = dynamic_cast<turbo::RectangleShape*>(d)) {
                auto* n = new turbo::RectangleShape(r->size.x, r->size.y, r->color);
                n->is_filled = r->is_filled;
                n->thickness = r->thickness;
                n->border_radius = r->border_radius;
                return n;
            }
            if (auto* c = dynamic_cast<turbo::CircleShape*>(d)) {
                auto* n = new turbo::CircleShape(c->radius, c->color);
                n->is_filled = c->is_filled;
                n->thickness = c->thickness;
                return n;
            }
            if (auto* s = dynamic_cast<turbo::Sprite*>(d)) {
                auto* n = new turbo::Sprite(s->get_texture());
                n->rect = s->rect;
                return n;
            }
            return nullptr;
        }

        // Edit a turbo::Color through an ImGui RGBA picker. Returns true if changed.
        bool edit_color(const char* label, turbo::Color& col) {
            float c[4] = {col.r / 255.0f, col.g / 255.0f, col.b / 255.0f, col.a / 255.0f};
            if (ImGui::ColorEdit4(label, c)) {
                col.r = static_cast<unsigned char>(c[0] * 255.0f);
                col.g = static_cast<unsigned char>(c[1] * 255.0f);
                col.b = static_cast<unsigned char>(c[2] * 255.0f);
                col.a = static_cast<unsigned char>(c[3] * 255.0f);
                return true;
            }
            return false;
        }
    }

    void Editor::draw_scene_overlay() {
        if (!this->selected || !this->selected->get_drawable())
            return;
        Shader* outline = this->engine->shaders.get("__outline__");
        if (!outline || !outline->valid())
            return;

        Drawable* drawable = this->selected->get_drawable();
        const ALLEGRO_TRANSFORM world = world_tf(this->selected);

        // Pulsing outline colour so the selection reads at a glance.
        const float t = static_cast<float>(al_get_time());
        const float k = 0.55f + 0.45f * (0.5f + 0.5f * std::sin(t * 6.0f));

        // Pass 1: draw the silhouette offset in 8 directions to grow a ring.
        outline->use();
        outline->set_vec4("outline_color", 1.0f, 0.75f * k, 0.15f, 1.0f);
        const float O = 3.0f;
        const float offsets[8][2] = {
            {O, 0}, {-O, 0}, {0, O}, {0, -O}, {O, O}, {-O, -O}, {O, -O}, {-O, O}
        };
        for (const auto& off : offsets) {
            ALLEGRO_TRANSFORM offset_world = world;
            al_translate_transform(&offset_world, off[0], off[1]);  // screen-space offset
            al_use_transform(&offset_world);
            drawable->draw();
        }

        // Pass 2: redraw the object on top so only the outline ring shows,
        // matching its in-scene appearance (material included).
        if (Shader* def = Shader::get_active_shader()) def->use();
        al_use_transform(&world);
        if (Material* mat = this->selected->get_material())
            mat->bind_at(static_cast<float>(al_get_time()));
        drawable->draw();
        if (Shader* def = Shader::get_active_shader()) def->use();
    }

    // ----------------------------------------------------------------------
    //  std::cout capture: every line written to std::cout (the Logger writes
    //  there) is mirrored into the console panel while still echoing to the
    //  real terminal.
    // ----------------------------------------------------------------------
    class Editor::ConsoleStreambuf : public std::streambuf {
    public:
        ConsoleStreambuf(std::vector<std::string>* sink, std::streambuf* echo)
            : sink(sink), echo(echo) {}

    protected:
        int overflow(int c) override {
            if (c == traits_type::eof()) return c;
            char ch = traits_type::to_char_type(c);
            if (echo) echo->sputc(ch);
            push(ch);
            return c;
        }

        std::streamsize xsputn(const char* s, std::streamsize n) override {
            if (echo) echo->sputn(s, n);
            for (std::streamsize i = 0; i < n; i++) push(s[i]);
            return n;
        }

    private:
        void push(char ch) {
            if (ch == '\n') {
                sink->push_back(cur);
                cur.clear();
                if (sink->size() > 500) sink->erase(sink->begin());
            } else if (ch != '\r') {
                cur.push_back(ch);
            }
        }

        std::vector<std::string>* sink;
        std::streambuf* echo;
        std::string cur;
    };

    // ----------------------------------------------------------------------
    //  Lifecycle
    // ----------------------------------------------------------------------
    Editor::Editor(turbo::Engine* engine): engine(engine) {
        previous_cout = std::cout.rdbuf();
        console_buf = new ConsoleStreambuf(&this->logs, previous_cout);
        std::cout.rdbuf(console_buf);
        this->log("Turbo Editor ready - EDIT mode. Press Play to simulate.");
    }

    Editor::~Editor() {
        if (previous_cout) std::cout.rdbuf(previous_cout);
        delete console_buf;
    }

    void Editor::push_fps(float value) {
        this->fps = value;
        this->fps_history.turn();
        this->fps_history.set_last(value);
    }

    void Editor::log(const std::string& line) {
        this->logs.push_back(line);
        if (this->logs.size() > 500) this->logs.erase(this->logs.begin());
    }

    turbo::GameObject* Editor::active_root() const {
        turbo::Scene* scene = this->engine->scene_manager.get_active_scene();
        return scene ? scene->get_root_gameobject() : nullptr;
    }

    // ----------------------------------------------------------------------
    //  Frame entry point
    // ----------------------------------------------------------------------
    void Editor::render() {
        ImGuiIO& io = ImGui::GetIO();
        const float W = io.DisplaySize.x;
        const float H = io.DisplaySize.y;

        // Scripts may have destroyed the selected object; drop a dangling selection.
        this->validate_selection();

        const float menu_h = this->draw_menu_bar();
        const float toolbar_h = 46.0f;
        this->draw_toolbar(menu_h, toolbar_h);

        if (this->show_grid)
            this->draw_grid();

        const float top = menu_h + toolbar_h;
        const float left_w = 290.0f;
        const float right_w = 340.0f;
        const float console_h = 180.0f;
        const float panels_bottom = this->show_console ? (H - console_h) : H;

        if (this->show_hierarchy)
            this->draw_hierarchy(0, top, left_w, panels_bottom - top);
        if (this->show_inspector)
            this->draw_inspector(W - right_w, top, right_w, panels_bottom - top);
        if (this->show_console)
            this->draw_console(0, H - console_h, W, console_h);
        if (this->show_stats)
            this->draw_stats_overlay((this->show_hierarchy ? left_w : 0.0f) + 12.0f, top + 12.0f);
        if (this->show_script_editor)
            this->draw_script_editor();
        if (this->show_about)
            this->draw_about();

        // Gizmo shortcuts (W/E/R) and in-viewport manipulation of the selection.
        if (!io.WantTextInput) {
            if (ImGui::IsKeyPressed(ImGuiKey_W)) this->gizmo.set_mode(Gizmo::Mode::Translate);
            if (ImGui::IsKeyPressed(ImGuiKey_E)) this->gizmo.set_mode(Gizmo::Mode::Rotate);
            if (ImGui::IsKeyPressed(ImGuiKey_R)) this->gizmo.set_mode(Gizmo::Mode::Scale);
            if (this->selected && ImGui::IsKeyPressed(ImGuiKey_Delete))
                this->pending_delete = this->selected;
            if (this->selected && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D))
                this->pending_duplicate = this->selected;
        }
        if (this->selected)
            this->gizmo.manipulate(this->selected);

        // Structural edits are applied here, after the scene tree has been
        // fully walked, so a delete/create never invalidates an active iterator.
        this->apply_pending();
    }

    float Editor::draw_menu_bar() {
        float height = 0.0f;
        if (ImGui::BeginMainMenuBar()) {
            height = ImGui::GetWindowSize().y;
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Play", "F5", false, !this->engine->is_simulating()))
                    this->engine->set_simulating(true);
                if (ImGui::MenuItem("Pause", "F6", false, this->engine->is_simulating()))
                    this->engine->set_simulating(false);
                ImGui::Separator();
                if (ImGui::MenuItem("Quit"))
                    this->engine->close();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("GameObject")) {
                turbo::GameObject* parent = this->selected ? this->selected : this->active_root();
                if (ImGui::MenuItem("Create Empty"))     { this->pending_add_parent = parent; this->pending_add_kind = 0; }
                if (ImGui::MenuItem("Create Rectangle")) { this->pending_add_parent = parent; this->pending_add_kind = 1; }
                if (ImGui::MenuItem("Create Circle"))    { this->pending_add_parent = parent; this->pending_add_kind = 2; }
                ImGui::Separator();
                if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, this->selected != nullptr))
                    this->pending_duplicate = this->selected;
                if (ImGui::MenuItem("Delete Selected", "Del", false, this->selected != nullptr))
                    this->pending_delete = this->selected;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Hierarchy", nullptr, &this->show_hierarchy);
                ImGui::MenuItem("Inspector", nullptr, &this->show_inspector);
                ImGui::MenuItem("Console",   nullptr, &this->show_console);
                ImGui::MenuItem("Stats",     nullptr, &this->show_stats);
                ImGui::MenuItem("Grid",      nullptr, &this->show_grid);
                ImGui::Separator();
                ImGui::MenuItem("Script Editor", "Lua", &this->show_script_editor);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About")) this->show_about = true;
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        return height > 0.0f ? height : ImGui::GetFrameHeight();
    }

    void Editor::draw_toolbar(float y, float height) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0, y));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, height));
        const ImGuiWindowFlags flags = PANEL_FLAGS | ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        if (ImGui::Begin("##toolbar", nullptr, flags)) {
            const bool sim = this->engine->is_simulating();
            if (sim) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.78f, 0.55f, 0.10f, 1.0f));
                if (ImGui::Button("|| Pause")) this->engine->set_simulating(false);
                ImGui::PopStyleColor();
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.60f, 0.24f, 1.0f));
                if (ImGui::Button("|> Play")) this->engine->set_simulating(true);
                ImGui::PopStyleColor();
            }

            turbo::GameObject* parent = this->selected ? this->selected : this->active_root();
            ImGui::SameLine(); ImGui::TextDisabled("|"); ImGui::SameLine();
            if (ImGui::Button("+ Empty"))  { this->pending_add_parent = parent; this->pending_add_kind = 0; }
            ImGui::SameLine();
            if (ImGui::Button("+ Rect"))   { this->pending_add_parent = parent; this->pending_add_kind = 1; }
            ImGui::SameLine();
            if (ImGui::Button("+ Circle")) { this->pending_add_parent = parent; this->pending_add_kind = 2; }
            ImGui::SameLine();
            if (ImGui::Button("Delete") && this->selected) this->pending_delete = this->selected;

            // Gizmo mode selector (W/E/R), the active mode is highlighted.
            ImGui::SameLine(); ImGui::TextDisabled("|"); ImGui::SameLine();
            const Gizmo::Mode modes[3] = {Gizmo::Mode::Translate, Gizmo::Mode::Rotate, Gizmo::Mode::Scale};
            const char* mode_labels[3] = {"Move", "Rotate", "Scale"};
            for (int i = 0; i < 3; i++) {
                const bool active = this->gizmo.mode() == modes[i];
                if (active) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.00f, 0.78f, 0.05f, 0.90f));
                    ImGui::PushStyleColor(ImGuiCol_Text,   ImVec4(0.10f, 0.10f, 0.08f, 1.00f));
                }
                if (ImGui::Button(mode_labels[i])) this->gizmo.set_mode(modes[i]);
                if (active) ImGui::PopStyleColor(2);
                ImGui::SameLine();
            }

            ImGui::TextDisabled("|"); ImGui::SameLine();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Mode: %s", sim ? "PLAYING" : "EDIT");

            char buf[48];
            std::snprintf(buf, sizeof(buf), "%.0f FPS", this->fps);
            const float tw = ImGui::CalcTextSize(buf).x;
            ImGui::SameLine(io.DisplaySize.x - tw - 18.0f);
            ImGui::Text("%s", buf);
        }
        ImGui::End();
    }

    // ----------------------------------------------------------------------
    //  Hierarchy
    // ----------------------------------------------------------------------
    void Editor::draw_hierarchy(float x, float y, float w, float h) {
        ImGui::SetNextWindowPos(ImVec2(x, y));
        ImGui::SetNextWindowSize(ImVec2(w, h));
        if (ImGui::Begin("Hierarchy", nullptr, PANEL_FLAGS)) {
            ImGui::TextDisabled("Scene: %s", this->engine->scene_manager.get_active_scene_name().c_str());
            ImGui::Separator();
            turbo::GameObject* root = this->active_root();
            if (root) {
                this->draw_hierarchy_node(root, root);
            } else {
                ImGui::TextWrapped("No active scene.");
            }
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
                this->selected = nullptr;
        }
        ImGui::End();
    }

    void Editor::draw_hierarchy_node(turbo::GameObject* go, turbo::GameObject* root) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                   ImGuiTreeNodeFlags_SpanAvailWidth |
                                   ImGuiTreeNodeFlags_DefaultOpen;
        if (go == this->selected) flags |= ImGuiTreeNodeFlags_Selected;
        const bool leaf = go->childs.empty();
        if (leaf) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::PushID(static_cast<void*>(go));
        const bool open = ImGui::TreeNodeEx("##node", flags, "%s%s",
                                            go->show ? "" : "[hidden] ",
                                            go->get_name().c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            this->selected = go;

        if (ImGui::BeginPopupContextItem("ctx")) {
            this->selected = go;
            if (ImGui::MenuItem("Add Empty Child"))     { this->pending_add_parent = go; this->pending_add_kind = 0; }
            if (ImGui::MenuItem("Add Rectangle Child")) { this->pending_add_parent = go; this->pending_add_kind = 1; }
            if (ImGui::MenuItem("Add Circle Child"))    { this->pending_add_parent = go; this->pending_add_kind = 2; }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", nullptr, false, go != root))
                this->pending_delete = go;
            ImGui::EndPopup();
        }

        if (open && !leaf) {
            for (turbo::GameObject* child : go->childs)
                this->draw_hierarchy_node(child, root);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    // ----------------------------------------------------------------------
    //  Inspector
    // ----------------------------------------------------------------------
    void Editor::draw_inspector(float x, float y, float w, float h) {
        ImGui::SetNextWindowPos(ImVec2(x, y));
        ImGui::SetNextWindowSize(ImVec2(w, h));
        if (ImGui::Begin("Inspector", nullptr, PANEL_FLAGS)) {
            turbo::GameObject* go = this->selected;
            if (!go) {
                ImGui::TextDisabled("Select an object in the Hierarchy.");
            } else {
                ImGui::PushID(static_cast<void*>(go));

                // Editable name (buffer refilled whenever the selection changes).
                if (this->name_buf_owner != go) {
                    std::snprintf(this->name_buf, sizeof(this->name_buf), "%s", go->get_name().c_str());
                    this->name_buf_owner = go;
                }
                if (ImGui::InputText("Name", this->name_buf, sizeof(this->name_buf),
                                     ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if (!go->set_name(this->name_buf))
                        std::snprintf(this->name_buf, sizeof(this->name_buf), "%s", go->get_name().c_str());
                }

                bool visible = go->show;
                if (ImGui::Checkbox("Visible", &visible)) go->show = visible;
                ImGui::SameLine();
                if (ImGui::SmallButton("Duplicate")) this->pending_duplicate = go;

                this->draw_transform_editor(go);
                this->draw_drawable_section(go);
                this->draw_material_section(go);

                ImGui::SeparatorText("Components");
                if (go->components.empty())
                    ImGui::TextDisabled("No components.");
                for (size_t i = 0; i < go->components.size(); i++)
                    this->draw_component(go->components[i], static_cast<int>(i));

                ImGui::Spacing();
                if (go->get_component<turbo::component::SimpleControl>() == nullptr) {
                    if (ImGui::Button("+ Add SimpleControl"))
                        go->add_component(new turbo::component::SimpleControl(go));
                } else {
                    ImGui::TextDisabled("SimpleControl attached");
                }

                ImGui::SameLine();
                if (ImGui::Button("+ Add Script")) {
                    this->refresh_scripts();
                    ImGui::OpenPopup("add_script_popup");
                }
                if (ImGui::BeginPopup("add_script_popup")) {
                    ImGui::TextDisabled("assets/scripts");
                    ImGui::Separator();
                    if (this->available_scripts.empty())
                        ImGui::TextDisabled("no .lua files found");
                    for (const std::string& path : this->available_scripts) {
                        const std::string label = std::filesystem::path(path).stem().string();
                        if (ImGui::Selectable(label.c_str())) {
                            go->add_component(new turbo::LuaComponent(go, path));
                            this->log(std::string("Attached script '") + label + "'");
                        }
                    }
                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }
        }
        ImGui::End();
    }

    void Editor::draw_transform_editor(turbo::GameObject* go) {
        ImGui::SeparatorText("Transform");

        turbo::Vector2<float> p = go->get_position();
        float pos[2] = {p.x, p.y};
        if (ImGui::DragFloat2("Position", pos, 1.0f))
            go->set_position(pos[0], pos[1]);

        float angle = go->get_angle();
        if (ImGui::SliderFloat("Rotation", &angle, 0.0f, 360.0f))
            go->set_rotation(angle);

        turbo::Vector2<float> s = go->get_scale();
        float scale[2] = {s.x, s.y};
        if (ImGui::DragFloat2("Scale", scale, 0.01f, 0.01f, 100.0f)) {
            if (scale[0] >= 0.001f && scale[1] >= 0.001f)
                go->set_scale(scale[0], scale[1]);
        }

        turbo::Vector2<int> c = go->get_center();
        int center[2] = {c.x, c.y};
        if (ImGui::DragInt2("Center", center))
            go->set_center(center[0], center[1]);
    }

    void Editor::draw_drawable_section(turbo::GameObject* go) {
        ImGui::SeparatorText("Drawable");
        turbo::Drawable* d = go->get_drawable();
        ImGui::Text("Current: %s", d ? "set" : "none");
        if (ImGui::Button("Set Rectangle"))
            go->set_drawable(new turbo::RectangleShape(80, 50, turbo::Color(80, 160, 240)));
        ImGui::SameLine();
        if (ImGui::Button("Set Circle"))
            go->set_drawable(new turbo::CircleShape(35, turbo::Color(240, 170, 70)));
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
            go->set_drawable(nullptr);

        d = go->get_drawable();
        if (!d) return;

        if (auto* r = dynamic_cast<turbo::RectangleShape*>(d)) {
            int size[2] = {static_cast<int>(r->size.x), static_cast<int>(r->size.y)};
            if (ImGui::DragInt2("Size", size, 1, 1, 8000)) {
                r->size.x = static_cast<unsigned int>(size[0] < 1 ? 1 : size[0]);
                r->size.y = static_cast<unsigned int>(size[1] < 1 ? 1 : size[1]);
            }
            edit_color("Color", r->color);
            ImGui::Checkbox("Filled", &r->is_filled);
            if (!r->is_filled)
                ImGui::DragFloat("Thickness", &r->thickness, 0.1f, 0.1f, 50.0f);
            ImGui::DragFloat("Border radius", &r->border_radius, 0.5f, 0.0f, 500.0f);
        } else if (auto* c = dynamic_cast<turbo::CircleShape*>(d)) {
            int radius = c->radius;
            if (ImGui::DragInt("Radius", &radius, 1, 1, 2000))
                c->radius = static_cast<unsigned short>(radius < 1 ? 1 : radius);
            edit_color("Color", c->color);
            ImGui::Checkbox("Filled", &c->is_filled);
            if (!c->is_filled)
                ImGui::DragFloat("Thickness", &c->thickness, 0.1f, 0.1f, 50.0f);
        } else if (dynamic_cast<turbo::Sprite*>(d)) {
            ImGui::TextDisabled("Sprite (texture)");
        }
    }

    void Editor::draw_material_section(turbo::GameObject* go) {
        ImGui::SeparatorText("Material");
        turbo::Material* current = go->get_material();
        const char* preview = current ? current->name().c_str() : "None";

        if (ImGui::BeginCombo("Shader", preview)) {
            if (ImGui::Selectable("None", current == nullptr))
                go->set_material(nullptr);
            for (const std::string& name : this->engine->shaders.material_names()) {
                // Hide editor-internal materials/shaders (e.g. the outline).
                if (name.size() >= 2 && name[0] == '_' && name[1] == '_') continue;
                turbo::Material* m = this->engine->shaders.material(name);
                if (ImGui::Selectable(name.c_str(), current == m))
                    go->set_material(m);
            }
            ImGui::EndCombo();
        }

        if (!current) return;

        if (current->shader() && !current->shader()->valid())
            ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "shader failed to build");

        // Data-driven uniform editors.
        for (auto& entry : current->uniforms()) {
            const std::string& uname = entry.first;
            UniformValue& v = entry.second;
            if (uname == "time") continue;   // fed automatically each frame
            ImGui::PushID(uname.c_str());
            const bool is_color = uname.find("tint") != std::string::npos ||
                                  uname.find("color") != std::string::npos;
            switch (v.type) {
                case UniformValue::Type::Float: ImGui::DragFloat(uname.c_str(), &v.f[0], 0.01f); break;
                case UniformValue::Type::Int:   ImGui::DragInt(uname.c_str(), &v.i); break;
                case UniformValue::Type::Bool:  ImGui::Checkbox(uname.c_str(), &v.b); break;
                case UniformValue::Type::Vec2:  ImGui::DragFloat2(uname.c_str(), v.f, 0.01f); break;
                case UniformValue::Type::Vec3:
                    if (is_color) ImGui::ColorEdit3(uname.c_str(), v.f);
                    else          ImGui::DragFloat3(uname.c_str(), v.f, 0.01f);
                    break;
                case UniformValue::Type::Vec4:
                    if (is_color) ImGui::ColorEdit4(uname.c_str(), v.f);
                    else          ImGui::DragFloat4(uname.c_str(), v.f, 0.01f);
                    break;
                default: ImGui::TextDisabled("%s", uname.c_str()); break;
            }
            ImGui::PopID();
        }
        ImGui::TextDisabled("Materials are shared between objects.");
    }

    void Editor::draw_grid() {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        const ImGuiIO& io = ImGui::GetIO();
        const float step = 50.0f;
        const ImU32 line = IM_COL32(255, 255, 255, 16);
        for (float x = 0; x < io.DisplaySize.x; x += step)
            dl->AddLine(ImVec2(x, 0), ImVec2(x, io.DisplaySize.y), line);
        for (float y = 0; y < io.DisplaySize.y; y += step)
            dl->AddLine(ImVec2(0, y), ImVec2(io.DisplaySize.x, y), line);
    }

    turbo::GameObject* Editor::clone_object(turbo::GameObject* src, turbo::GameObject* parent) {
        // Unique sibling name "<src> (n)".
        std::string name;
        int n = 1;
        bool taken;
        do {
            name = src->get_name() + " (" + std::to_string(n++) + ")";
            taken = false;
            for (turbo::GameObject* c : parent->childs)
                if (c->get_name() == name) { taken = true; break; }
        } while (taken);

        turbo::GameObject* copy = nullptr;
        try {
            copy = new turbo::GameObject(parent, name);
        } catch (const std::exception& e) {
            this->log(std::string("Duplicate failed: ") + e.what());
            return nullptr;
        }
        copy->set_position(src->get_position());
        copy->set_scale(src->get_scale());
        copy->set_rotation(src->get_angle());
        copy->set_center(src->get_center());
        copy->show = src->show;
        copy->set_material(src->get_material());
        if (turbo::Drawable* d = src->get_drawable())
            copy->set_drawable(clone_drawable(d));
        // Clone components we know how to reconstruct (scripts re-attach by path).
        for (turbo::Component* c : src->components) {
            if (auto* lua = dynamic_cast<turbo::LuaComponent*>(c))
                copy->add_component(new turbo::LuaComponent(copy, lua->script_path()));
        }
        if (src->get_component<turbo::component::SimpleControl>())
            copy->add_component(new turbo::component::SimpleControl(copy));
        for (turbo::GameObject* child : src->childs)
            this->clone_object(child, copy);
        return copy;
    }

    void Editor::draw_component(turbo::Component* comp, int index) {
        ImGui::PushID(index);
        const char* cname = comp->get_name() ? comp->get_name() : "Component";

        bool enabled = comp->is_enabled();
        if (ImGui::Checkbox("##enabled", &enabled)) {
            if (enabled) comp->enable();
            else comp->disable();
        }
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(cname, ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            if (auto* script = dynamic_cast<turbo::LuaComponent*>(comp)) {
                this->draw_script_component(script);
            } else {
                int uid = 0;
                for (debug::InspectorObserver* obs : comp->debug_inspector_observers) {
                    if (obs) this->draw_observer(*obs, uid++);
                }
                if (comp->debug_inspector_observers.empty())
                    ImGui::TextDisabled("No inspectable fields.");
            }
            ImGui::Unindent();
        }
        ImGui::PopID();
    }

    void Editor::draw_script_component(turbo::LuaComponent* script) {
        ImGui::TextDisabled("Lua script: %s.lua", script->script_name().c_str());
        if (!script->valid())
            ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "failed to load (see console)");
        if (ImGui::SmallButton("Reload")) {
            script->reload();
            this->log(std::string("Reloaded script '") + script->script_name() + "'");
        }

        for (const ScriptField& f : script->fields()) {
            ImGui::PushID(f.name.c_str());
            switch (f.type) {
                case ScriptField::Type::Number: {
                    float v = static_cast<float>(f.number);
                    if (ImGui::DragFloat(f.name.c_str(), &v, 0.1f))
                        script->set_number(f.name, v);
                    break;
                }
                case ScriptField::Type::Bool: {
                    bool v = f.boolean;
                    if (ImGui::Checkbox(f.name.c_str(), &v))
                        script->set_bool(f.name, v);
                    break;
                }
                case ScriptField::Type::String: {
                    char buf[256];
                    std::snprintf(buf, sizeof(buf), "%s", f.text.c_str());
                    if (ImGui::InputText(f.name.c_str(), buf, sizeof(buf)))
                        script->set_string(f.name, buf);
                    break;
                }
            }
            ImGui::PopID();
        }
    }

    void Editor::refresh_scripts() {
        this->available_scripts.clear();
        turbo::Path* p = turbo::Path::get_resources_path();
        p->append_component("assets");
        p->append_component("scripts");
        const std::string dir = p->c_str();
        delete p;

        std::error_code ec;
        if (!std::filesystem::is_directory(dir, ec)) return;
        for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
            if (entry.path().extension() == ".lua")
                this->available_scripts.push_back(entry.path().string());
        }
        std::sort(this->available_scripts.begin(), this->available_scripts.end());
    }

    namespace {
        // std::string-backed InputText (avoids depending on imgui_stdlib linkage).
        int grow_string(ImGuiInputTextCallbackData* data) {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                std::string* s = static_cast<std::string*>(data->UserData);
                s->resize(data->BufTextLen);
                data->Buf = s->data();
            }
            return 0;
        }

        void reload_walk(turbo::GameObject* go, const std::string& path, int& count) {
            if (!go) return;
            for (turbo::Component* c : go->components) {
                if (auto* lua = dynamic_cast<turbo::LuaComponent*>(c)) {
                    if (lua->script_path() == path) { lua->reload(); count++; }
                }
            }
            for (turbo::GameObject* child : go->childs)
                reload_walk(child, path, count);
        }

        bool reachable(turbo::GameObject* go, turbo::GameObject* target) {
            if (!go) return false;
            if (go == target) return true;
            for (turbo::GameObject* c : go->childs)
                if (reachable(c, target)) return true;
            return false;
        }
    }

    void Editor::validate_selection() {
        if (!this->selected) return;
        if (!reachable(this->active_root(), this->selected)) {
            this->selected = nullptr;
            this->name_buf_owner = nullptr;
        }
    }

    void Editor::open_script_file(const std::string& path) {
        std::ifstream fs(path, std::ios::binary);
        if (!fs) { this->editor_status = "could not open file"; return; }
        std::ostringstream ss;
        ss << fs.rdbuf();
        this->editor_buffer = ss.str();
        this->editor_buffer.reserve(this->editor_buffer.size() + 4096);
        this->editor_script_path = path;
        this->editor_status = "opened";
    }

    void Editor::save_script_file() {
        if (this->editor_script_path.empty()) return;
        std::ofstream fs(this->editor_script_path, std::ios::binary | std::ios::trunc);
        if (!fs) { this->editor_status = "save failed"; return; }
        fs << this->editor_buffer;
        this->editor_status = "saved";
        this->log(std::string("Saved ") + std::filesystem::path(this->editor_script_path).filename().string());
    }

    void Editor::reload_script_in_scene(const std::string& path) {
        int count = 0;
        reload_walk(this->active_root(), path, count);
        this->editor_status = "reloaded (" + std::to_string(count) + " live)";
        this->log("Hot-reloaded script in " + std::to_string(count) + " component(s)");
    }

    void Editor::draw_script_editor() {
        ImGui::SetNextWindowSize(ImVec2(760, 560), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(120, 380), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Lua Script Editor", &this->show_script_editor)) {
            ImGui::End();
            return;
        }

        if (ImGui::Button("New")) ImGui::OpenPopup("new_script");
        ImGui::SameLine();
        if (ImGui::Button("Save")) this->save_script_file();
        ImGui::SameLine();
        if (ImGui::Button("Save & Reload")) {
            this->save_script_file();
            this->reload_script_in_scene(this->editor_script_path);
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) this->refresh_scripts();
        ImGui::SameLine();
        ImGui::TextDisabled("%s", this->editor_status.c_str());

        if (ImGui::BeginPopup("new_script")) {
            ImGui::InputText("name", this->new_script_name, sizeof(this->new_script_name));
            ImGui::SameLine();
            if (ImGui::Button("Create") && this->new_script_name[0]) {
                turbo::Path* p = turbo::Path::get_resources_path();
                p->append_component("assets");
                p->append_component("scripts");
                std::string dir = p->c_str();
                delete p;
                std::string path = dir + "/" + this->new_script_name + ".lua";
                std::ofstream fs(path, std::ios::binary);
                fs << "local " << this->new_script_name << " = {}\n\n"
                   << "function " << this->new_script_name << ":load()\nend\n\n"
                   << "function " << this->new_script_name << ":update(dt)\nend\n\n"
                   << "return " << this->new_script_name << "\n";
                fs.close();
                this->refresh_scripts();
                this->open_script_file(path);
                this->new_script_name[0] = 0;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        if (this->available_scripts.empty())
            this->refresh_scripts();

        // Left: file list.
        ImGui::BeginChild("script_files", ImVec2(150, 0), ImGuiChildFlags_Borders);
        for (const std::string& path : this->available_scripts) {
            const std::string label = std::filesystem::path(path).stem().string();
            if (ImGui::Selectable(label.c_str(), path == this->editor_script_path))
                this->open_script_file(path);
        }
        ImGui::EndChild();
        ImGui::SameLine();

        // Right: code + REPL.
        ImGui::BeginChild("script_code", ImVec2(0, 0));
        if (this->editor_script_path.empty()) {
            ImGui::TextDisabled("Select a script on the left, or create a new one.");
        } else {
            ImGui::TextDisabled("%s", std::filesystem::path(this->editor_script_path).filename().string().c_str());
            const float repl_h = 64.0f;
            const ImVec2 avail = ImGui::GetContentRegionAvail();

            if (this->code_font) ImGui::PushFont(this->code_font);
            const float line_h = ImGui::GetTextLineHeight();
            const float char_w = ImGui::CalcTextSize(" ").x;   // monospace advance

            int line_count = 1, max_len = 0, cur_len = 0;
            for (char ch : this->editor_buffer) {
                if (ch == '\n') { line_count++; if (cur_len > max_len) max_len = cur_len; cur_len = 0; }
                else cur_len++;
            }
            if (cur_len > max_len) max_len = cur_len;

            const ImVec2 pad = ImGui::GetStyle().FramePadding;
            const float content_w = max_len * char_w + pad.x * 2.0f + 8.0f;

            ImGui::BeginChild("code_scroll", ImVec2(-1, avail.y - repl_h), ImGuiChildFlags_Borders,
                              ImGuiWindowFlags_HorizontalScrollbar);
            const float view_w = ImGui::GetContentRegionAvail().x;
            ImVec2 origin = ImGui::GetCursorScreenPos();
            origin.x += pad.x;
            origin.y += pad.y;
            ImGui::InputTextMultiline("##code", this->editor_buffer.data(), this->editor_buffer.capacity() + 1,
                ImVec2(content_w > view_w ? content_w : view_w, line_count * line_h + pad.y * 2.0f + 4.0f),
                ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_NoHorizontalScroll,
                grow_string, &this->editor_buffer);
            const bool code_focused = ImGui::IsItemActive();

            // Live syntax colouring is drawn over the plain monospace text. We
            // use the foreground draw list (clipped to this child) so it stays
            // on top even while the input is focused — otherwise the active
            // input's inner child window paints over a parent-list overlay.
            ImDrawList* overlay = ImGui::GetForegroundDrawList();
            const ImVec2 clip_min = ImGui::GetWindowPos();
            const ImVec2 clip_max(clip_min.x + ImGui::GetWindowSize().x,
                                  clip_min.y + ImGui::GetWindowSize().y);
            overlay->PushClipRect(clip_min, clip_max, true);
            this->draw_code_highlight(overlay, origin, char_w, line_h);
            overlay->PopClipRect();
            ImGui::EndChild();
            if (this->code_font) ImGui::PopFont();

            if (code_focused && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
                this->save_script_file();

            ImGui::Separator();
            ImGui::TextDisabled("REPL >");
            ImGui::SameLine();
            ImGui::PushItemWidth(-54.0f);
            const bool enter = ImGui::InputText("##repl", this->repl_buf, sizeof(this->repl_buf),
                                                ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if ((ImGui::Button("Run") || enter) && this->repl_buf[0]) {
                this->log(std::string("> ") + this->repl_buf);
                this->engine->run_script_repl(this->repl_buf);
                this->repl_buf[0] = 0;
            }
        }
        ImGui::EndChild();
        ImGui::End();
    }

    namespace {
        const std::set<std::string>& lua_keywords() {
            static const std::set<std::string> kw = {
                "and", "break", "do", "else", "elseif", "end", "false", "for",
                "function", "goto", "if", "in", "local", "nil", "not", "or",
                "repeat", "return", "then", "true", "until", "while", "self"
            };
            return kw;
        }
        bool is_ident_start(char c) { return std::isalpha((unsigned char)c) || c == '_'; }
        bool is_ident(char c)       { return std::isalnum((unsigned char)c) || c == '_'; }
    }

    void Editor::draw_code_highlight(ImDrawList* dl, const ImVec2& origin, float /*char_w*/, float line_h) {
        const std::string& s = this->editor_buffer;
        ImFont* font = ImGui::GetFont();
        const float fs = ImGui::GetFontSize();
        const float scale = fs / font->FontSize;
        const float space_w = font->GetCharAdvance((ImWchar)' ') * scale;

        const ImU32 col_def = ImGui::GetColorU32(ImGuiCol_Text);   // identifiers / punctuation
        const ImU32 col_kw  = IM_COL32(255, 205, 70, 255);         // keywords (yellow)
        const ImU32 col_str = IM_COL32(150, 205, 120, 255);        // strings
        const ImU32 col_num = IM_COL32(130, 180, 255, 255);        // numbers
        const ImU32 col_com = IM_COL32(112, 112, 100, 255);        // comments

        int line = 0;
        float x = origin.x;
        size_t i = 0;
        // Advance x exactly like ImGui lays the text out, so the coloured overlay
        // lines up perfectly on top of the input (no horizontal drift).
        auto advance_of = [&](char c) -> float {
            if (c == '\t') {
                const float stop = space_w * 4.0f;
                const float used = x - origin.x;
                return stop - std::fmod(used, stop);
            }
            return font->GetCharAdvance((ImWchar)(unsigned char)c) * scale;
        };
        auto draw_run = [&](size_t a, size_t b, ImU32 color) {
            if (b > a)
                dl->AddText(font, fs, ImVec2(x, origin.y + line * line_h), color, s.c_str() + a, s.c_str() + b);
            for (size_t k = a; k < b; k++) x += advance_of(s[k]);
        };

        while (i < s.size()) {
            const char ch = s[i];
            if (ch == '\n') { line++; x = origin.x; i++; continue; }
            if (ch == ' ' || ch == '\t') { x += advance_of(ch); i++; continue; }

            if (ch == '-' && i + 1 < s.size() && s[i + 1] == '-') {       // line comment
                size_t j = i; while (j < s.size() && s[j] != '\n') j++;
                draw_run(i, j, col_com); i = j; continue;
            }
            if (ch == '"' || ch == '\'') {                                // string literal
                const char q = ch; size_t j = i + 1;
                while (j < s.size() && s[j] != q && s[j] != '\n') {
                    if (s[j] == '\\' && j + 1 < s.size()) j++;
                    j++;
                }
                if (j < s.size() && s[j] == q) j++;
                draw_run(i, j, col_str); i = j; continue;
            }
            if (std::isdigit((unsigned char)ch)) {                        // number
                size_t j = i;
                while (j < s.size() && (std::isalnum((unsigned char)s[j]) || s[j] == '.')) j++;
                draw_run(i, j, col_num); i = j; continue;
            }
            if (is_ident_start(ch)) {                                     // identifier / keyword
                size_t j = i; while (j < s.size() && is_ident(s[j])) j++;
                const std::string word = s.substr(i, j - i);
                draw_run(i, j, lua_keywords().count(word) ? col_kw : col_def);
                i = j; continue;
            }
            draw_run(i, i + 1, col_def); i++;                            // punctuation
        }
    }

    void Editor::draw_observer(debug::InspectorObserver& obs, int uid) {
        ImGui::PushID(uid);
        const char* label = obs.name ? obs.name : "value";
        switch (obs.type) {
            case debug::FLOAT: {
                auto* v = static_cast<float*>(obs.data);
                if (v) ImGui::InputFloat(label, v);
                break;
            }
            case debug::INT: {
                auto* v = static_cast<int*>(obs.data);
                if (v) ImGui::InputInt(label, v);
                break;
            }
            case debug::FLOAT_RANGE: {
                auto* v = static_cast<float*>(obs.data);
                if (v) ImGui::SliderFloat(label, v, static_cast<float>(obs.min), static_cast<float>(obs.max));
                break;
            }
            case debug::INT_RANGE: {
                auto* v = static_cast<int*>(obs.data);
                if (v) ImGui::SliderInt(label, v, static_cast<int>(obs.min), static_cast<int>(obs.max));
                break;
            }
            case debug::CSTRING: {
                auto* v = static_cast<char**>(obs.data);
                if (v && *v) {
                    char buf[256];
                    std::snprintf(buf, sizeof(buf), "%s", *v);
                    if (ImGui::InputText(label, buf, sizeof(buf)))
                        *v = strdup(buf);
                }
                break;
            }
            case debug::UI_TEXT: {
                auto* v = static_cast<const char*>(obs.data);
                if (v) ImGui::TextWrapped("%s", v);
                break;
            }
            case debug::UI_SEPARATOR:
                ImGui::Separator();
                break;
            default:
                ImGui::TextDisabled("%s (unsupported type)", label);
                break;
        }
        ImGui::PopID();
    }

    // ----------------------------------------------------------------------
    //  Console
    // ----------------------------------------------------------------------
    void Editor::draw_console(float x, float y, float w, float h) {
        ImGui::SetNextWindowPos(ImVec2(x, y));
        ImGui::SetNextWindowSize(ImVec2(w, h));
        if (ImGui::Begin("Console", nullptr, PANEL_FLAGS)) {
            if (ImGui::Button("Clear")) this->logs.clear();
            ImGui::SameLine();
            ImGui::Checkbox("Auto-scroll", &this->console_autoscroll);
            ImGui::SameLine();
            ImGui::TextDisabled("(%d lines)", static_cast<int>(this->logs.size()));
            ImGui::Separator();

            ImGui::BeginChild("##log_scroll", ImVec2(0, 0), ImGuiChildFlags_None,
                              ImGuiWindowFlags_HorizontalScrollbar);
            for (const std::string& line : this->logs) {
                ImVec4 color(0.85f, 0.85f, 0.85f, 1.0f);
                if (line.find("[error]") != std::string::npos)
                    color = ImVec4(1.0f, 0.42f, 0.42f, 1.0f);
                else if (line.find("[warn]") != std::string::npos)
                    color = ImVec4(1.0f, 0.80f, 0.32f, 1.0f);
                ImGui::TextColored(color, "%s", line.c_str());
            }
            if (this->console_autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
            ImGui::EndChild();
        }
        ImGui::End();
    }

    // ----------------------------------------------------------------------
    //  Stats overlay (floats over the viewport)
    // ----------------------------------------------------------------------
    void Editor::draw_stats_overlay(float x, float y) {
        ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.55f);
        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing |
                                       ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (ImGui::Begin("##stats", nullptr, flags)) {
            ImGui::Text("Turbo Editor  v%s", turbo::Engine::version.c_str());
            ImGui::Separator();
            ImGui::Text("FPS: %.0f", this->fps);

            float* history = this->fps_history.get_array();
            ImGui::PlotLines("##fps_plot", history, this->fps_history.get_size(),
                             0, nullptr, 0.0f, 144.0f, ImVec2(220, 45));
            delete[] history;

            ImGui::Text("Objects: %d", count_tree(this->active_root()));
            const turbo::Vector2<int>& mouse = turbo::Engine::input.get_mouse_position();
            ImGui::Text("Mouse: %d, %d", mouse.x, mouse.y);
            ImGui::Text("Mode: %s", this->engine->is_simulating() ? "PLAYING" : "EDIT");
        }
        ImGui::End();
    }

    void Editor::draw_about() {
        ImGui::OpenPopup("About Turbo Editor");
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("About Turbo Editor", &this->show_about, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Turbo Engine editor  -  v%s", turbo::Engine::version.c_str());
            ImGui::Separator();
            ImGui::TextWrapped(
                "A small ImGui scene editor for the Turbo 2D engine.\n\n"
                "- Hierarchy: select, right-click to add/delete objects.\n"
                "- Inspector: edit the transform, drawable and components.\n"
                "- Toolbar: Play/Pause toggles scene simulation.\n"
                "- The live scene renders behind the panels (the viewport).");
            ImGui::Spacing();
            if (ImGui::Button("Close", ImVec2(120, 0))) {
                this->show_about = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    // ----------------------------------------------------------------------
    //  Structural edits (deferred)
    // ----------------------------------------------------------------------
    turbo::GameObject* Editor::create_object(turbo::GameObject* parent, int kind) {
        if (!parent) parent = this->active_root();
        if (!parent) return nullptr;

        const char* base = (kind == 1) ? "Rectangle" : (kind == 2) ? "Circle" : "Empty";
        std::string name;
        int n = 1;
        bool taken;
        do {
            name = std::string(base) + " " + std::to_string(n++);
            taken = false;
            for (turbo::GameObject* child : parent->childs) {
                if (child->get_name() == name) { taken = true; break; }
            }
        } while (taken);

        turbo::GameObject* go = nullptr;
        try {
            go = new turbo::GameObject(parent, name);
        } catch (const std::exception& e) {
            this->log(std::string("Create failed: ") + e.what());
            return nullptr;
        }

        if (kind == 1)
            go->set_drawable(new turbo::RectangleShape(80, 50, turbo::Color(80, 160, 240)));
        else if (kind == 2)
            go->set_drawable(new turbo::CircleShape(35, turbo::Color(240, 170, 70)));

        const turbo::Vector2<float>& pp = parent->get_position();
        go->set_position(pp.x + 40.0f, pp.y + 40.0f);
        this->selected = go;
        this->log(std::string("Created '") + name + "'");
        return go;
    }

    void Editor::destroy_object(turbo::GameObject* go) {
        if (!go) return;
        turbo::GameObject* parent = go->get_parent();
        if (!parent) {
            this->log("Cannot delete the scene root.");
            return;
        }
        const std::string name = go->get_name();
        std::vector<turbo::GameObject*>& siblings = parent->childs;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), go), siblings.end());
        // The selection may have been the deleted node or one of its children.
        this->selected = nullptr;
        delete go;   // recursively destroys the whole subtree
        this->log(std::string("Deleted '") + name + "'");
    }

    void Editor::apply_pending() {
        if (this->pending_add_kind >= 0) {
            this->create_object(this->pending_add_parent, this->pending_add_kind);
            this->pending_add_parent = nullptr;
            this->pending_add_kind = -1;
        }
        if (this->pending_duplicate) {
            turbo::GameObject* src = this->pending_duplicate;
            this->pending_duplicate = nullptr;
            turbo::GameObject* parent = src->get_parent();
            if (!parent) {
                this->log("Cannot duplicate the scene root.");
            } else if (turbo::GameObject* copy = this->clone_object(src, parent)) {
                this->selected = copy;
                this->log(std::string("Duplicated '") + src->get_name() + "'");
            }
        }
        if (this->pending_delete) {
            this->destroy_object(this->pending_delete);
            this->pending_delete = nullptr;
        }
    }
}

#endif // __TURBO_USE_IMGUI__
