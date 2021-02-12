
#ifndef __TURBO_SCENE_MANAGER_HPP__
#define __TURBO_SCENE_MANAGER_HPP__

#include <map>
#include <unordered_map>
#include <string>
#include "Scene.hpp"

namespace turbo {
    /**
     * Manage the scene instances
     */
    class SceneManager {
    public:
        SceneManager() = default;

        /**
         * @brief Register a scene in the manager
         * @param scene The target scene
         * @param name The name of the scene (used to identify the scene in the registry)
         */
        void register_scene(Scene* scene, const char* name);

        /**
         * @brief Set the scene currently displayed
         * @param name Name of the scene
         */
        void set_active_scene(const char* name);

        /**
         * @brief Retrieves the currently displayed scene
         * @return Current scene
         */
        Scene* get_active_scene() const;

    private:
        Scene* active_scene = nullptr;
        std::map<const char*, Scene*> scenes = {};
    };
}

#endif