
#ifndef __TURBO_ENGINE_SOUND_HPP__
#define __TURBO_ENGINE_SOUND_HPP__

#include "Audio.hpp"
#include "../Path.hpp"
#include <allegro5/allegro_audio.h>

namespace turbo {
    class Sound : public Audio {
    public:
        explicit Sound(Path* path);
        ~Sound();

        void play() override;
        void pause() override;
        void reset() override;
        [[nodiscard]] bool is_playing() const override;
    protected:
        ALLEGRO_SAMPLE* sample = nullptr;
        ALLEGRO_SAMPLE_INSTANCE* sample_instance = nullptr;
    };
}

#endif
