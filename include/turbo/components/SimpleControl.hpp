

#ifndef __TURBO_COMP_SIMPLE_CONTROL_HPP__
#define __TURBO_COMP_SIMPLE_CONTROL_HPP__

#include "../Component.hpp"
#include "../Vector2.hpp"

namespace turbo::component {
    /**
     * @brief Simple pre-made component used handle an 8-direction movement with the arrow keys
     */
    class SimpleControl : public ::turbo::Component {
    public:
        using turbo::Component::Component;

        void load() override;
        void on_disable() override;
        void on_enable() override;
        void unload() override;
        void update(int delta_time) override;

        float acceleration = 1.0;
        float max_speed = 15;
        float friction = 0.1;

    protected:
        ::turbo::Vector2<float> velocity = ::turbo::Vector2<float>();
    };
}

#endif
