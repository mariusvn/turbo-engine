#include <turbo/SceneManager.hpp>
#include <stdexcept>

namespace turbo {
    SceneManager::SceneManager() {
    }

    void SceneManager::register_scene(Scene* scene, const char* name) {
        if (this->scenes.find(name) != this->scenes.end()) {
            throw std::runtime_error(std::string("The scene \"") + name + "\" already exists");
        }
        this->scenes[name] = scene;
    }

    void SceneManager::set_active_scene(const char* name) {
        if (this->scenes.find(name) == this->scenes.end()) {
            throw std::runtime_error(std::string("The scene \"") + name + "\" doesn't exists");
        }
        if (this->active_scene) {
            this->active_scene->unload();
        }
        Scene* tmp = this->scenes[name];
        tmp->load();
        this->active_scene = tmp;
        this->active_scene_name = name;
        ONLYIMGUI(
            this->debug.scene_name = name;
            this->debug.set_root_gameobject(this->active_scene->get_root_gameobject());
        );
    }

    Scene* SceneManager::get_active_scene() const {
        return this->active_scene;
    }

    const std::string& SceneManager::get_active_scene_name() const {
        return this->active_scene_name;
    }
}