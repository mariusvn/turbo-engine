#include <turbo/sound/MusicStream.hpp>
#include <stdexcept>

namespace turbo {

    MusicStream::MusicStream(Path* path) {
        this->stream = al_load_audio_stream(path->c_str(), 4, 1024);
        if (!this->stream) {
            throw std::runtime_error("Cannot load " + std::string(path->c_str()));
        }
        al_set_audio_stream_playmode(this->stream, ALLEGRO_PLAYMODE_ONCE);
        al_set_audio_stream_playing(this->stream, false);
        al_attach_audio_stream_to_mixer(this->stream, al_get_default_mixer());
    }

    MusicStream::~MusicStream() {
        bool playing = al_get_audio_stream_playing(this->stream);
        if (playing && this->stream)
            al_drain_audio_stream(this->stream);
        al_destroy_audio_stream(this->stream);
    }

    void MusicStream::play() {
        al_set_audio_stream_playing(this->stream, true);
    }

    void MusicStream::pause() {
        al_set_audio_stream_playing(this->stream, false);
    }

    void MusicStream::reset() {
        al_seek_audio_stream_secs(this->stream, 0);
    }

    bool MusicStream::is_playing() const {
        return al_get_audio_stream_playing(this->stream);
    }
}