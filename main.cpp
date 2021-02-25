#include <iostream>
#include <turbo/Engine.hpp>
#include <turbo/sound/MusicStream.hpp>


class TestScene: public turbo::Scene {
public:
    TestScene() = default;

    void load() override {
        turbo::Path* path = turbo::Path::get_resources_path();
        path->drop_tail();
        path->drop_tail();
        path->drop_tail();
        path->append_component("assets/demo");
        path->set_filename("png-test.png");

        this->texture = new turbo::Texture(path);
        auto* sprite = new turbo::Sprite(texture);
        sprite->rect.x = 150;
        sprite->rect.width = 100;

        auto* root = new turbo::GameObject(nullptr, "root");
        root->set_position(100,100);
        root->set_drawable(sprite);

        this->root_gameobject = root;

        delete path;

    }

    void unload() override {
        delete this->root_gameobject;
        delete this->texture;
        this->root_gameobject = nullptr;
        this->texture = nullptr;
    }

private:
    turbo::Texture* texture;

};

int main() {
    turbo::Engine engine = turbo::Engine();
    TestScene* test_scene = new TestScene();
    engine.scene_manager.register_scene(test_scene, "test scene");
    engine.scene_manager.set_active_scene("test scene");
    try {
        engine.start_window("Turbo Engine", 1600, 900);
        engine.loop();
        engine.stop_window();
        test_scene->unload();
        delete test_scene;
    } catch (std::exception& e) {
        turbo::Logger::log(e.what());
        return 1;
    }
    return 0;
}