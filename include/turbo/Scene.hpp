
#ifndef __TURBO_SCENE_HPP__
#define __TURBO_SCENE_HPP__

#include "GameObject.hpp"

namespace turbo {
    /**
     * @brief Basic scene class
     */
    class Scene {
    public:
        /**
         * @brief Called when the scene is loading
         */
        virtual void load() {};

        /**
         * @brief Called when the scene is unloading
         */
        virtual void unload() {};

        /**
         * @brief Called on every logic frame
         * 
         * @param delta_time milliseconds since the last call
         */
        virtual void update(int delta_time) final;

        /**
         * @brief Called on every render frame
         */
        virtual void render() final;

        GameObject* get_root_gameobject() const {
            return this->root_gameobject;
        }

    protected:
        /**
         * @brief Root GameObject of the scene objects hierarchy
         */
        GameObject* root_gameobject;
    };
}

#endif