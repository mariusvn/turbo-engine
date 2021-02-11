#include <turbo/Scene.hpp>

namespace turbo {

    void Scene::update(int delta_time) {
        if (this->root_gameobject) {
            this->root_gameobject->update(delta_time);
        }
    }

    void Scene::render() {
        if (this->root_gameobject) {
            this->root_gameobject->render(nullptr);
        }
    }

}