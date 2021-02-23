
#ifdef __TURBO_USE_IMGUI__

    #include <turbo/debug_menus/ComponentDebug.hpp>
    #include <turbo/DebugImgui.hpp>
    #include <turbo/Engine.hpp>

    #define __TURBO_IMGUI_LABEL(text, index, name) ((std::string(text) + "##" + std::string(name) + std::to_string(index)).c_str())

namespace debug {
    ComponentDebug::ComponentDebug(turbo::Component *component) {
        if (std::find(opened_wins.begin(), opened_wins.end(), component) != opened_wins.end()) {
            /* already opened */
            delete this;
        } else {
            ComponentDebug::opened_wins.push_back(component);
            this->component = component;
            this->observers = &component->debug_inspector_observers;
            turbo::Engine::engine->render_tick += *this->eventHandler;
        }
    }

    void ComponentDebug::render() {
        if (!this->is_window_open) {
            opened_wins.erase(std::remove(opened_wins.begin(), opened_wins.end(), this->component), opened_wins.end());
            turbo::Engine::engine->render_tick -= *this->eventHandler;
            return delete this;
        }
        if (!ImGui::Begin(
            ( std::string("Component: ") + ((this->component->get_name()) ? this->component->get_name() : "")).c_str(),
            &this->is_window_open,
            ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::End();
            return;
        }

        if (this->observers && !this->observers->empty()) {
            for (auto & i : *this->observers) {
                InspectorObserver observer = *i;
                ComponentDebug::render(observer);
            }
        }

        ImGui::End();

    }

    void ComponentDebug::render(debug::InspectorObserver& obs) {
        for (unsigned short i = 0; renderers[i].func; i++) {
            if (renderers[i].type == obs.type)
                ComponentDebug::renderers[i].func(obs);
        }
    }


    void ComponentDebug::render_float(debug::InspectorObserver& obs) {
        auto val = (float*) obs.data;
        if (val)
            ImGui::InputFloat(obs.name, val, 1.0f);
    }

    void ComponentDebug::render_int(debug::InspectorObserver& obs) {
        auto val = (int*) obs.data;
        if (val)
            ImGui::InputInt(obs.name, val);
    }

    void ComponentDebug::render_cstring(debug::InspectorObserver& obs) {
        auto val = (char**) obs.data;
        if (val) {
            unsigned short buffer_size = strlen(*val) + 10;

            char* buf = new char[buffer_size]();
            strcpy(buf, *val);
            ImGui::InputText(obs.name, buf, buffer_size);
            if (strcmp(buf, *val) != 0) {
                *val = strdup(buf);
            }
            delete[] buf;
        }
    }

    void ComponentDebug::render_int_vector(InspectorObserver &obs) {
        auto val = (std::vector<int>*) obs.data;
        if (val) {
            if (ImGui::TreeNode(obs.name)) {
                for (unsigned long long i = 0; i < val->size(); i++) {
                    int value = (*val)[i];
                    ImGui::InputInt(std::to_string(i).c_str(), &value);
                    if (value != (*val)[i])
                        (*val)[i] = value;
                    ImGui::SameLine();
                    if (ImGui::Button(__TURBO_IMGUI_LABEL("Delete", i, obs.name))) {
                        val->erase(val->begin() + (int)i);
                    }
                }
                if (ImGui::Button(__TURBO_IMGUI_LABEL("+ Add", 0, obs.name)))
                    val->push_back(0);
                ImGui::TreePop();
            }
        }
    }

    void ComponentDebug::render_float_vector(InspectorObserver &obs) {
        auto val = (std::vector<float>*) obs.data;
        if (val) {
            if (ImGui::TreeNode(obs.name)) {
                for (unsigned long long i = 0; i < val->size(); i++) {
                    float value = (*val)[i];
                    ImGui::InputFloat(std::to_string(i).c_str(), &value);
                    if (value != (*val)[i])
                        (*val)[i] = value;
                    ImGui::SameLine();
                    if (ImGui::Button(__TURBO_IMGUI_LABEL("Delete", i, obs.name))) {
                        val->erase(val->begin() + (int)i);
                    }
                }
                if (ImGui::Button(__TURBO_IMGUI_LABEL("+ Add", 0, obs.name)))
                    val->push_back(0.0f);
                ImGui::TreePop();
            }
        }
    }

    void ComponentDebug::render_multiple_float(InspectorObserver &obs) {
        auto val = (float**) obs.data;
        if (val) {
            char i = 0;
            ImGui::PushItemWidth(150);
            switch(obs.type) {
                case FLOAT4:
                    ImGui::InputFloat(__TURBO_IMGUI_LABEL(std::to_string(i), i, std::string("_") + obs.name), val[i], 1.0f);
                    i++;
                    ImGui::SameLine();
                case FLOAT3:
                    ImGui::InputFloat(__TURBO_IMGUI_LABEL(std::to_string(i), i, std::string("_") + obs.name), val[i], 1.0f);
                    i++;
                    ImGui::SameLine();
                case FLOAT2:
                    ImGui::InputFloat(__TURBO_IMGUI_LABEL(std::to_string(i), i, std::string("_") + obs.name), val[i], 1.0f);
                    i++;
                    ImGui::SameLine();
                    ImGui::InputFloat(__TURBO_IMGUI_LABEL(std::to_string(i), i, std::string("_") + obs.name), val[i], 1.0f);
                    break;
                default: return;
            }
            ImGui::SameLine();
            ImGui::PopItemWidth();
            ImGui::Text("%s", obs.name);
        }
    }

    void ComponentDebug::render_multiple_int(InspectorObserver &obs) {
        auto val = (int **) obs.data;
        if (val) {
            char i = 0;
            ImGui::PushItemWidth(150);
            switch(obs.type) {
                case INT4:
                    ImGui::InputInt(__TURBO_IMGUI_LABEL(std::to_string(i), i, std::string("_") + obs.name), val[i]);
                    i++;
                    ImGui::SameLine();
                case INT3:
                    ImGui::InputInt(__TURBO_IMGUI_LABEL(std::to_string(i), i, std::string("_") + obs.name), val[i]);
                    i++;
                    ImGui::SameLine();
                case INT2:
                    ImGui::InputInt(__TURBO_IMGUI_LABEL(std::to_string(i), i, std::string("_") + obs.name), val[i]);
                    i++;
                    ImGui::SameLine();
                    ImGui::InputInt(__TURBO_IMGUI_LABEL(std::to_string(i), i, std::string("_") + obs.name), val[i]);
                    break;
                default: return;
            }
            ImGui::SameLine();
            ImGui::PopItemWidth();
            ImGui::Text("%s", obs.name);
        }
    }

    void ComponentDebug::render_float_range(InspectorObserver &obs) {
        auto val = (float*) obs.data;
        if (val) {
            ImGui::DragFloat(__TURBO_IMGUI_LABEL(obs.name, 0, obs.name), val, 1.0f, obs.min, obs.max);
        }
    }

    void ComponentDebug::render_int_range(InspectorObserver &obs) {
        auto val = (int*) obs.data;
        if (val) {
            ImGui::DragInt(__TURBO_IMGUI_LABEL(obs.name, 0, obs.name), val, 1.0f, obs.min, obs.max);
        }
    }

    void ComponentDebug::render_ui_text(InspectorObserver &obs) {
        auto val = (const char*) obs.data;
        if (val) {
            ImGui::Text("%s", val);
        }
    }

    void ComponentDebug::render_ui_separator(InspectorObserver &obs) {
        ImGui::Separator();
    }

}

#endif