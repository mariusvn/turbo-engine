
#ifndef __TURBO_ENGINE_AUDIO_HPP__
#define __TURBO_ENGINE_AUDIO_HPP__

namespace turbo {
    /**
     * @brief Interface for playable audio (implemented by Sound and MusicStream).
     */
    class Audio {
    public:
        /** @brief Start, or resume, playback. */
        virtual void play() = 0;
        /** @brief Pause playback, keeping the current position. */
        virtual void pause() = 0;
        /** @brief Stop playback and rewind to the beginning. */
        virtual void reset() = 0;
        /** @brief Whether the audio is currently playing. */
        [[nodiscard]] virtual bool is_playing() const = 0;
    };
}

#endif
