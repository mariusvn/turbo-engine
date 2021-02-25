
#ifndef __TURBO_ENGINE_AUDIO_HPP__
#define __TURBO_ENGINE_AUDIO_HPP__

namespace turbo {
    class Audio {
    public:
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void reset() = 0;
        [[nodiscard]] virtual bool is_playing() const = 0;
    };
}

#endif
