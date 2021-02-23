
#ifdef __TURBO_USE_IMGUI__

    #include <turbo/debug_menus/GameObjectDebug.hpp>

namespace debug {
    GameObjectDebug::GameObjectDebug(turbo::GameObject *gameobject): gameobject(gameobject) {
        std::string title = "[GameObject] " + gameobject->get_name();
        this->win_name = title;
        std::vector<std::string>* vec = &(GameObjectDebug::current_instances);
        auto iter = std::find(vec->begin(), vec->end(), title);
        if (iter != vec->end()) {
            delete this;
            return;
        }
        turbo::Engine::engine->render_tick += *this->eventHandler;
        vec->push_back(this->win_name);
    }

    void GameObjectDebug::render() {
        if (!this->is_window_open) {
            /* delete window */
            turbo::Engine::engine->render_tick -= *this->eventHandler;
            std::vector<std::string>* vec = &(GameObjectDebug::current_instances);
            vec->erase(std::remove(vec->begin(), vec->end(), std::string(this->win_name)), vec->end());
            return delete this;
        }
        if (!ImGui::Begin(this->win_name.c_str(), &this->is_window_open, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::End();
            return;
        }

        ImGui::Text("Name: %s", this->gameobject->get_name().c_str());
        ImGui::Text("Has drawable: %s", (this->gameobject->get_drawable() == nullptr) ? "false" : "true");
        ImGui::Text("Components: %d", this->gameobject->get_component_amount());

        if (ImGui::TreeNode("Component list")) {
            for (turbo::Component* comp : this->gameobject->components) {
                ImGui::Text("%s", comp->get_name());
            }
            ImGui::TreePop();
        }
        ImGui::Separator();

        turbo::Vector2<float> o_pos = this->gameobject->get_position();
        float pos[2] = {o_pos.x, o_pos.y};
        ImGui::InputFloat2("Position", pos);
        if (pos[0] != o_pos.x || pos[1] != o_pos.y) {
            this->gameobject->set_position(pos[0], pos[1]);
        }

        float angle = this->gameobject->get_angle();
        float new_angle = angle;
        ImGui::SliderFloat("Rotation", &new_angle, 0.0, 360.0);
        if (new_angle != angle) {
            this->gameobject->set_rotation(new_angle);
        }

        turbo::Vector2<float> o_scale = this->gameobject->get_scale();
        float scale[2] = {o_scale.x, o_scale.y};
        ImGui::InputFloat2("Scale", scale);
        if (scale[0] != o_scale.x || scale[1] != o_scale.y) {
            if (scale[0] >= 0.001 && scale[1] >= 0.001)
                this->gameobject->set_scale(scale[0], scale[1]);
        }

        turbo::Vector2<int> o_center = this->gameobject->get_center();
        int center[2] = {o_center.x, o_center.y};
        ImGui::InputInt2("Center", center);
        if (center[0] != o_center.x || center[1] != o_center.y) {
            this->gameobject->set_center(center[0], center[1]);
        }

        ImGui::End();
    }
}

#endif