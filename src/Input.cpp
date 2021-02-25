#include <turbo/Input.hpp>
#include <cstdlib>
#include <iostream>
#include <allegro5/allegro.h>
#include <turbo/Engine.hpp>

// TODO add camera mouse input

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

    void Input::internal::set_display(Input* self, void* display) {
        self->display = (ALLEGRO_DISPLAY*) display;
    }

    void Input::internal::update_mouse_position(Input* self, int& x, int& y) {
        self->mouse_position.x = x;
        self->mouse_position.y = y;
    }

    bool Input::is_key_pressed(int key) const {
        if (this->key[key])
            return true;
        else
            return false;
    }

    void Input::set_mouse_position(const Vector2<int>& pos) {
        this->set_mouse_position(pos.x, pos.y);
    }

    void Input::set_mouse_position(int x, int y) {
        if (!this->display)
            return;
        al_set_mouse_xy(this->display, x, y);
        this->mouse_position.x = x;
        this->mouse_position.y = y;
    }

    const Vector2<int>& Input::get_mouse_position() const {
        return this->mouse_position;
    }

    bool Input::is_mouse_button_pressed(enum mouse_buttons button) const {
        ALLEGRO_MOUSE_STATE state;
        al_get_mouse_state(&state);
        return al_mouse_button_down(&state, (int)button);
    }
}