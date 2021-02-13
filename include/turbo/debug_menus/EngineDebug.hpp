
/* this file in only read in debug mode */

#ifndef __TURBO_ENGINE_ENGINEDEBUG_HPP__
#define __TURBO_ENGINE_ENGINEDEBUG_HPP__

#include "DebugWindow.hpp"
#include "../RotativeBuffer.hpp"
#include "../Vector2.hpp"

namespace turbo {
    class Engine;
}

namespace debug {

    class EngineDebug: public DebugWindow {
    public:
        EngineDebug(turbo::Engine* engine);

        void render() override;
        void register_fps_time(float fps);
        void update_fps(int fps);
        void update_win_size(const turbo::Vector2<unsigned short> &size);


    protected:
        int fps = 0;
        turbo::Vector2<unsigned short> win_size;
        turbo::Engine* engine = nullptr;

        turbo::RotativeBuffer<float> fps_buffer = turbo::RotativeBuffer<float>(0, 60);
    };
}

#endif
