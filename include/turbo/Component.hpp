
#ifndef __TURBO_COMPONENT_HPP__
#define __TURBO_COMPONENT_HPP__

#include "debug_menus/Inspector.hpp"
#include <vector>

namespace turbo {
    class GameObject;

    /**
     * @brief Components who defines the behaviour of a GameObject
     */
    class Component {
    public:
        /**
         * @brief Construct a new Component. The component will be
         * automatically added to the GameObject given as parameter
         * 
         * @param parent GameObject that holds the component
         */
        explicit Component(GameObject* parent);

        /**
         * @brief Called when the component is loaded into the scene.
         */
        virtual void load() {};

        /**
         * @brief Called when the component is disabled
         */
        virtual void on_disable() {};

        /**
         * @brief Called when the component is enabled (Not
         * called on the first initialization)
         */
        virtual void on_enable() {};

        /**
         * @brief Logic update. Souldn't be used for anything else
         * than logic.
         * 
         * @param delta_time milliseconds since the last call
         */
        virtual void update(int delta_time) {};

        /**
         * @brief Called when the component is unloaded
         */
        virtual void unload() {};

        /**
         * @brief Disable the component. Do nothing if already disabled
         */
        void disable();

        /**
         * @brief Enable the component. Do nothing if already enabled
         */
        void enable();

        /**
         * @brief Check if the component is enabled
         * 
         * @return true if enabled
         */
        bool is_enabled();

        const char* get_name() const;

        /**
         * @brief Debug use only. List of observed values in the inspector
         */
        ::std::vector<debug::InspectorObserver*> debug_inspector_observers{};

    protected:
        GameObject* gameObject = nullptr;
        bool enabled = true;
        const char *name = "Unknown";
    };
}

#endif