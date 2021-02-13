
#ifdef __TURBO_USE_IMGUI__
#include <turbo/debug_menus/SceneManagerDebug.hpp>
    #include <turbo/DebugImgui.hpp>
    #include <turbo/Engine.hpp>
    #include <turbo/debug_menus/GameObjectDebug.hpp>
namespace debug {
    SceneManagerDebug::SceneManagerDebug() {
        this->event_handler = new turbo::EventHandler<>([this]{
            this->render();
        });
    }

    void SceneManagerDebug::render() {
        if (!this->initialized) {
            this->is_window_open = false;
            turbo::Engine::engine->render_tick += *this->event_handler;
            this->initialized = true;
        } else {
            if (!is_window_open)
                return;
            ImGui::Begin("Scene Manager", &this->is_window_open, ImGuiWindowFlags_AlwaysAutoResize);
            if (!this->scene_name) {
                ImGui::Text("Scene name: No scene loaded");
                ImGui::End();
                return;
            }
            ImGui::Text("Scene name: \"%s\"", this->scene_name);
            ImGui::Separator();
            if (this->root_gameobject) {
                ImGui::Text("Game objects:");
                this->draw_child(this->root_gameobject);
            }
            ImGui::End();
        }
    }

    void SceneManagerDebug::draw_child(turbo::GameObject* gm) {
        if (ImGui::TreeNode(gm->get_name().c_str())) {
            if (ImGui::Button("Open Properties")) {
                GameObjectDebug* debug = new GameObjectDebug(gm);
            }
            ImGui::Text("Childs: %s", (gm->childs.empty()) ? "None" : "");
            for (auto &child : gm->childs) {
                this->draw_child(child);
            }
            ImGui::TreePop();
        }
    }

    void SceneManagerDebug::set_root_gameobject(turbo::GameObject *game_object) {
        this->root_gameobject = game_object;
    }
}
#endif