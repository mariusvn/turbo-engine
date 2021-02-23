
/* Only read in debug mode */

#ifndef __TURBO_ENGINE_DEBUGWINDOW_HPP__
#define __TURBO_ENGINE_DEBUGWINDOW_HPP__

#ifdef __TURBO_USE_IMGUI__
    #include <imgui/imgui.h>
#endif
namespace debug {
    class DebugWindow {
    public:
        virtual void render() = 0;
        virtual void open() {this->is_window_open = true;}
        virtual void close() {this->is_window_open = false;}
        static void set_color_theme();

    protected:
        bool is_window_open = true;
    };
}

#endif
