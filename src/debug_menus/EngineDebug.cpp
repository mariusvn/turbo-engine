
#ifdef __TURBO_USE_IMGUI__

    #include <turbo/debug_menus/EngineDebug.hpp>
    #include <turbo/DebugImgui.hpp>
    #include <string>
    #include <turbo/Engine.hpp>

namespace debug
{
    EngineDebug::EngineDebug(turbo::Engine *engine) {
        this->engine = engine;
    }

    void EngineDebug::render()
    {
        ImGui::Begin("Engine##MainWin", 0, ImGuiWindowFlags_AlwaysAutoResize);
        std::string fps_txt = std::to_string(this->fps);
        float* fps_history = this->fps_buffer.get_array();
        ImGui::PlotLines(("FPS: " + fps_txt).c_str(), fps_history, this->fps_buffer.get_size());
        ImGui::Text("%s: {x: %d, y: %d}", "Window size", this->win_size.x, this->win_size.y);
        if (ImGui::Button("Scene Manager")) {
            this->engine->scene_manager.debug.open();
        }
        delete fps_history;
        ImGui::End();
    }

    void EngineDebug::register_fps_time(float fps) {
        this->fps_buffer.turn();
        this->fps_buffer.set_last(fps);
    }

    void EngineDebug::update_fps(int fps) {
        this->fps = fps;
    }

    void EngineDebug::update_win_size(const turbo::Vector2<unsigned short> &size) {
        this->win_size = size;
    }

}

#endif