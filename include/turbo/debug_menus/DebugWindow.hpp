
/* Only read in debug mode */

#ifndef __TURBO_ENGINE_DEBUGWINDOW_HPP__
#define __TURBO_ENGINE_DEBUGWINDOW_HPP__

namespace debug {
    class DebugWindow {
    public:
        virtual void render() = 0;
        virtual void open() {this->is_window_open = true;}
        virtual void close() {this->is_window_open = false;}

    protected:
        bool is_window_open = true;
    };
}

#endif
