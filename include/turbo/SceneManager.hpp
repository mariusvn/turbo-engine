
#ifndef __TURBO_SCENE_MANAGER_HPP__
#define __TURBO_SCENE_MANAGER_HPP__

#include <map>
#include <unordered_map>
#include <string>
#include "Scene.hpp"
#include "DebugImgui.hpp"

namespace turbo {
    /**
     * Manage the scene instances
     */
    class SceneManager {
    public:
        SceneManager();

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

        /**
         * @brief Name under which the active scene was registered (empty if none)
         */
        const std::string& get_active_scene_name() const;

        ONLYIMGUI(debug::SceneManagerDebug debug);

    private:
        Scene* active_scene = nullptr;
        std::string active_scene_name = {};
        std::map<std::string, Scene*> scenes = {};
    };
}

#endif