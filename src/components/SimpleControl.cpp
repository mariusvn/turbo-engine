#include <turbo/components/SimpleControl.hpp>
#include <turbo/Engine.hpp>

namespace turbo::component {
    void SimpleControl::load() {
        this->name = strdup("Simple Control");
        this->velocity.reset();

        this->debug_inspector_observers.push_back(
            new debug::InspectorObserver(
                &this->velocity.x,
                debug::FLOAT,
                "Velocity X"
                )
            );

        this->debug_inspector_observers.push_back(
            new debug::InspectorObserver(
                &this->velocity.y,
                debug::FLOAT,
                "Velocity Y"
            )
        );
    }

    void SimpleControl::unload() {

    }

    void SimpleControl::update(int delta_time) {
        bool changed = false;
        if (Engine::input.is_key_pressed(turbo::KEY_UP)) {
            this->velocity.y -= (float)delta_time * this->acceleration;
            changed = true;
        }
        if (Engine::input.is_key_pressed(turbo::KEY_DOWN)) {
            this->velocity.y += (float)delta_time * this->acceleration;
            changed = true;
        }
        if (Engine::input.is_key_pressed(turbo::KEY_LEFT)) {
            this->velocity.x -= (float)delta_time * this->acceleration;
            changed = true;
        }
        if (Engine::input.is_key_pressed(turbo::KEY_RIGHT)) {
            this->velocity.x += (float)delta_time * this->acceleration;
            changed = true;
        }
        if (changed) {
            if (this->velocity.x > this->max_speed) {
                this->velocity.x = this->max_speed;
            }
            if (this->velocity.x < -1 * this->max_speed) {
                this->velocity.x = -1 * this->max_speed;
            }
            if (this->velocity.y > this->max_speed) {
                this->velocity.y = this->max_speed;
            }
            if (this->velocity.y < -1 * this->max_speed) {
                this->velocity.y = -1 * this->max_speed;
            }
        }
        if (!(this->velocity.x < 0.001 && this->velocity.x > -0.001 && this->velocity.y < 0.001 && this->velocity.y > -0.001)) {
            this->velocity = this->velocity / (this->friction * (float) delta_time);
            this->gameObject->translate(this->velocity.x, this->velocity.y);
        }
    }

    void SimpleControl::on_disable() {

    }

    void SimpleControl::on_enable() {
        this->velocity.reset();
    }
}