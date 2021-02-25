
#ifndef __TURBO_ENGINE_MUSIC_HPP__
#define __TURBO_ENGINE_MUSIC_HPP__

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include "../Path.hpp"
#include "Audio.hpp"

namespace turbo {

    class MusicStream : public Audio {
    public:
        explicit MusicStream(Path* path);
        ~MusicStream();

        void play() override;
        void pause() override;
        void reset() override;
        [[nodiscard]] bool is_playing() const override;
    protected:
        ALLEGRO_AUDIO_STREAM* stream = nullptr;
    };
}

#endif
