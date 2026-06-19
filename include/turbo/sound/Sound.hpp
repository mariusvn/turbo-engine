
#ifndef __TURBO_ENGINE_SOUND_HPP__
#define __TURBO_ENGINE_SOUND_HPP__

#include "Audio.hpp"
#include "../Path.hpp"
#include <allegro5/allegro_audio.h>

namespace turbo {
    /**
     * @brief A short sound effect, loaded fully into memory and playable
     * on demand. For longer tracks streamed from disk, use MusicStream.
     */
    class Sound : public Audio {
    public:
        /**
         * @brief Load a sound sample from an audio file.
         * @param path path to the audio file
         */
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
