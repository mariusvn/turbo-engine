#include <turbo/Component.hpp>

namespace turbo {

    Component::Component(GameObject* parent): gameObject(parent) {}

    void Component::disable() {
        if (this->enabled == true) {
            this->enabled = false;
            this->on_disable();
        }
    }

    void Component::enable() {
        if (this->enabled == false) {
            this->enabled = true;
            this->on_enable();
        }
    }

    bool Component::is_enabled() {
        return this->enabled;
    }

}