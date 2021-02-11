#include <turbo/Input.hpp>
#include <stdlib.h>
#include <iostream>

namespace turbo {
    Input::Input() {
        memset(this->key, 0, sizeof(this->key));
    }

    void Input::process_timer_event() {
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
            this->key[i] &= __TURBO_KEY_SEEN;
        }
    }

    void Input::process_key_down_event(ALLEGRO_EVENT* event) {
        this->key[event->keyboard.keycode] = __TURBO_KEY_SEEN | __TURBO_KEY_RELEASED;
    }

    void Input::process_key_up_event(ALLEGRO_EVENT* event) {
        this->key[event->keyboard.keycode] &= __TURBO_KEY_RELEASED;
    }

    bool Input::is_key_pressed(int key) const {
        if (this->key[key])
            return true;
        else
            return false;
    }
}