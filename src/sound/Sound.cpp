#include <turbo/sound/Sound.hpp>

namespace  turbo {
    Sound::Sound(Path* path) {
        this->sample = al_load_sample(path->c_str());
        if (!this->sample) {
            throw std::runtime_error("Cannot load " + std::string(path->c_str()));
        }

    }

    Sound::~Sound() {
        if (this->sample_instance && al_get_sample_instance_playing(this->sample_instance))
            al_stop_sample_instance(this->sample_instance);
        al_destroy_sample_instance(this->sample_instance);
        this->sample_instance = nullptr;
        al_destroy_sample(this->sample);
        this->sample = nullptr;
    }

    void Sound::play() {
        if (this->sample_instance) {
            if (al_get_sample_instance_playing(this->sample_instance)) {
                al_stop_sample_instance(this->sample_instance);
            }
            al_detach_sample_instance(this->sample_instance);
            al_destroy_sample_instance(this->sample_instance);
            this->sample_instance = nullptr;
        }
        this->sample_instance = al_create_sample_instance(this->sample);
        al_set_sample_instance_playing(this->sample_instance, true);
    }

    void Sound::pause() {
        if (!this->sample_instance)
            return;
        al_set_sample_instance_playing(this->sample_instance, false);
    }

    void Sound::reset() {
        if (!this->sample_instance)
            return;
        al_set_sample_instance_position(this->sample_instance, 0);
    }

    bool Sound::is_playing() const {
        return al_get_sample_instance_playing(this->sample_instance);
    }

}