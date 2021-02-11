
#ifndef __TURBO_SCENE_MANAGER_HPP__
#define __TURBO_SCENE_MANAGER_HPP__

#include <map>
#include <unordered_map>
#include <string>
#include "Scene.hpp"

namespace turbo {
    class SceneManager {
    public:
        SceneManager() = default;

        void register_scene(Scene* scene, const char* name);
        void set_active_scene(const char* name);
        Scene* get_active_scene() const;
    private:
        Scene* active_scene = nullptr;
        std::map<const char*, Scene*> scenes = {};
    };
}

#endif