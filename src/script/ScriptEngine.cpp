#include <turbo/script/ScriptEngine.hpp>
#include <turbo/script/LuaComponent.hpp>
#include <turbo/GameObject.hpp>
#include <turbo/Scene.hpp>
#include <turbo/Vector2.hpp>
#include <turbo/Input.hpp>
#include <turbo/Engine.hpp>
#include <turbo/graphics/RectangleShape.hpp>
#include <turbo/graphics/CircleShape.hpp>
#include <turbo/graphics/Color.hpp>

#include <allegro5/allegro.h>
#include <iostream>

namespace turbo::script {

    namespace {
        turbo::GameObject* scene_root() {
            if (!turbo::Engine::engine) return nullptr;
            turbo::Scene* s = turbo::Engine::engine->scene_manager.get_active_scene();
            return s ? s->get_root_gameobject() : nullptr;
        }

        turbo::GameObject* find_recursive(turbo::GameObject* go, const std::string& name) {
            if (!go) return nullptr;
            if (go->get_name() == name) return go;
            for (turbo::GameObject* c : go->childs) {
                if (turbo::GameObject* hit = find_recursive(c, name)) return hit;
            }
            return nullptr;
        }

        unsigned char to_byte(double v) {
            if (v <= 1.0) v *= 255.0;          // accept 0..1 or 0..255
            if (v < 0) v = 0; if (v > 255) v = 255;
            return static_cast<unsigned char>(v);
        }
    }

    ScriptEngine::ScriptEngine() {
        this->lua.open_libraries(
            sol::lib::base, sol::lib::math, sol::lib::string,
            sol::lib::table, sol::lib::os);
        this->bind_api();
    }

    void ScriptEngine::run_deferred() {
        if (this->deferred.empty()) return;
        // Swap out first: a deferred command may queue more.
        std::vector<std::function<void()>> batch;
        batch.swap(this->deferred);
        for (auto& fn : batch) fn();
    }

    void ScriptEngine::run_string(const std::string& code) {
        // Try as an expression first so REPL results get printed, then as a statement.
        sol::load_result expr = this->lua.load("return " + code);
        sol::protected_function_result r =
            expr.valid() ? expr.call() : this->lua.safe_script(code, sol::script_pass_on_error);

        if (!r.valid()) {
            sol::error err = r;
            std::cout << "[lua][error] " << err.what() << std::endl;
            return;
        }
        sol::object o = r;
        if (o.valid() && o.get_type() != sol::type::nil) {
            sol::protected_function tostring = this->lua["tostring"];
            sol::protected_function_result s = tostring(o);
            if (s.valid()) std::cout << "[lua] " << s.get<std::string>() << std::endl;
        }
    }

    void ScriptEngine::bind_api() {
        sol::state& l = this->lua;

        // -------- Vector2 --------
        l.new_usertype<turbo::Vector2<float>>("Vector2",
            sol::constructors<turbo::Vector2<float>(), turbo::Vector2<float>(float, float)>(),
            "x", &turbo::Vector2<float>::x,
            "y", &turbo::Vector2<float>::y);

        // -------- GameObject --------
        l.new_usertype<turbo::GameObject>("GameObject",
            sol::no_constructor,
            "translate",    [](turbo::GameObject& g, float x, float y) { g.translate(x, y); },
            "set_position", [](turbo::GameObject& g, float x, float y) { g.set_position(x, y); },
            "get_position", [](turbo::GameObject& g) { return g.get_position(); },
            "rotate",       [](turbo::GameObject& g, float deg) { g.rotate(deg); },
            "set_rotation", [](turbo::GameObject& g, float deg) { g.set_rotation(deg); },
            "get_angle",    [](turbo::GameObject& g) { return g.get_angle(); },
            "set_scale",    [](turbo::GameObject& g, float x, float y) { g.set_scale(x, y); },
            "get_scale",    [](turbo::GameObject& g) { return g.get_scale(); },
            "set_center",   [](turbo::GameObject& g, int x, int y) { g.set_center(x, y); },
            "get_name",     [](turbo::GameObject& g) { return g.get_name(); },
            "set_name",     [](turbo::GameObject& g, const std::string& n) { return g.set_name(n); },
            "show",         sol::property(
                                [](turbo::GameObject& g) { return g.show; },
                                [](turbo::GameObject& g, bool v) { g.show = v; }),
            // hierarchy
            "get_parent",   [](turbo::GameObject& g) { return g.get_parent(); },
            "children",     [](turbo::GameObject& g) { return g.childs; },
            "find_child",   [](turbo::GameObject& g, const std::string& n) -> turbo::GameObject* {
                                for (turbo::GameObject* c : g.childs) if (c->get_name() == n) return c;
                                return nullptr;
                            },
            // drawables & material
            "set_rectangle", [](turbo::GameObject& g, float w, float h, sol::optional<double> r,
                                sol::optional<double> gr, sol::optional<double> b) {
                                turbo::Color col(to_byte(r.value_or(255)), to_byte(gr.value_or(255)), to_byte(b.value_or(255)));
                                g.set_drawable(new turbo::RectangleShape(static_cast<unsigned int>(w), static_cast<unsigned int>(h), col));
                            },
            "set_circle",    [](turbo::GameObject& g, float radius, sol::optional<double> r,
                                sol::optional<double> gr, sol::optional<double> b) {
                                turbo::Color col(to_byte(r.value_or(255)), to_byte(gr.value_or(255)), to_byte(b.value_or(255)));
                                g.set_drawable(new turbo::CircleShape(static_cast<unsigned short>(radius), col));
                            },
            "set_material",  [](turbo::GameObject& g, const std::string& name) {
                                if (turbo::Engine::engine)
                                    g.set_material(turbo::Engine::engine->shaders.material(name));
                            },
            "clear_material",[](turbo::GameObject& g) { g.set_material(nullptr); },
            "add_script",    [](turbo::GameObject& g, const std::string& path) {
                                g.add_component(new turbo::LuaComponent(&g, path));
                            });

        // -------- globals: scene --------
        l.set_function("root", []() { return scene_root(); });
        l.set_function("find", [](const std::string& name) { return find_recursive(scene_root(), name); });
        l.set_function("spawn", [this](const std::string& name) -> turbo::GameObject* {
            turbo::GameObject* go = new turbo::GameObject(nullptr, name); // unparented for now
            turbo::GameObject* root = scene_root();
            this->defer([go, root]() { go->attach_to(root); });           // linked after the update pass
            return go;
        });
        l.set_function("destroy", [this](turbo::GameObject* go) {
            if (!go) return;
            this->defer([go]() {
                turbo::GameObject* p = go->get_parent();
                if (!p) return;                  // never delete the scene root / unlinked nodes
                go->attach_to(nullptr);
                delete go;
            });
        });

        // -------- globals: logging --------
        l.set_function("log",   [](sol::variadic_args va) {
            std::cout << "[lua]";
            for (auto v : va) std::cout << " " << v.as<std::string>();
            std::cout << std::endl;
        });
        l.set_function("warn",  [](sol::variadic_args va) {
            std::cout << "[lua][warn]";
            for (auto v : va) std::cout << " " << v.as<std::string>();
            std::cout << std::endl;
        });
        l.set_function("error_log", [](sol::variadic_args va) {
            std::cout << "[lua][error]";
            for (auto v : va) std::cout << " " << v.as<std::string>();
            std::cout << std::endl;
        });

        // -------- globals: input & time --------
        l.set_function("is_key_pressed", [](int key) {
            return turbo::Engine::input.is_key_pressed(key);
        });
        l.set_function("is_mouse_pressed", [](int button) {
            return turbo::Engine::input.is_mouse_button_pressed(static_cast<turbo::mouse_buttons>(button));
        });
        l.set_function("mouse_position", []() {
            return turbo::Engine::input.get_mouse_position();
        });
        l.set_function("time", []() { return al_get_time(); });

        l.new_usertype<turbo::Vector2<int>>("Vector2i",
            "x", &turbo::Vector2<int>::x,
            "y", &turbo::Vector2<int>::y);

        // -------- key / mouse constants --------
        sol::table key = l.create_named_table("Key");
        key["A"] = turbo::KEY_A; key["B"] = turbo::KEY_B; key["C"] = turbo::KEY_C;
        key["D"] = turbo::KEY_D; key["E"] = turbo::KEY_E; key["F"] = turbo::KEY_F;
        key["G"] = turbo::KEY_G; key["H"] = turbo::KEY_H; key["I"] = turbo::KEY_I;
        key["J"] = turbo::KEY_J; key["K"] = turbo::KEY_K; key["L"] = turbo::KEY_L;
        key["M"] = turbo::KEY_M; key["N"] = turbo::KEY_N; key["O"] = turbo::KEY_O;
        key["P"] = turbo::KEY_P; key["Q"] = turbo::KEY_Q; key["R"] = turbo::KEY_R;
        key["S"] = turbo::KEY_S; key["T"] = turbo::KEY_T; key["U"] = turbo::KEY_U;
        key["V"] = turbo::KEY_V; key["W"] = turbo::KEY_W; key["X"] = turbo::KEY_X;
        key["Y"] = turbo::KEY_Y; key["Z"] = turbo::KEY_Z;
        key["SPACE"] = turbo::KEY_SPACE; key["ENTER"] = turbo::KEY_ENTER; key["ESCAPE"] = turbo::KEY_ESCAPE;
        key["UP"] = turbo::KEY_UP; key["DOWN"] = turbo::KEY_DOWN;
        key["LEFT"] = turbo::KEY_LEFT; key["RIGHT"] = turbo::KEY_RIGHT;

        sol::table mouse = l.create_named_table("Mouse");
        mouse["LEFT"] = turbo::LEFT_CLICK;
        mouse["RIGHT"] = turbo::RIGHT_CLICK;
    }
}
