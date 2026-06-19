#include <iostream>
#include <turbo/Engine.hpp>
#include <turbo/graphics/Sprite.hpp>
#include <turbo/graphics/Texture.hpp>
#include <turbo/graphics/RectangleShape.hpp>
#include <turbo/graphics/CircleShape.hpp>
#include <turbo/graphics/Color.hpp>
#include <turbo/components/SimpleControl.hpp>

// A small demo scene built to be explored with the editor: a root holding a
// controllable sprite, a couple of primitive shapes and a nested child.
class TestScene: public turbo::Scene {
public:
    TestScene() = default;

    void load() override {
        turbo::Path* path = turbo::Path::get_resources_path();
        path->append_component("assets/demo");
        path->set_filename("png-test.png");
        this->texture = new turbo::Texture(path);
        delete path;

        auto* root = new turbo::GameObject(nullptr, "Scene Root");

        // Controllable sprite (arrow keys, once you press Play).
        auto* player = new turbo::GameObject(root, "Player");
        player->set_position(760, 360);
        player->set_drawable(new turbo::Sprite(this->texture));
        player->add_component(new turbo::component::SimpleControl(player));

        // A rectangle with a nested child rectangle.
        auto* box = new turbo::GameObject(root, "Red Box");
        box->set_position(450, 260);
        box->set_drawable(new turbo::RectangleShape(140, 90, turbo::Color(220, 70, 70)));

        auto* small = new turbo::GameObject(box, "Small Box");
        small->set_position(40, 110);
        small->set_drawable(new turbo::RectangleShape(40, 40, turbo::Color(240, 240, 240)));

        // A standalone circle.
        auto* ball = new turbo::GameObject(root, "Yellow Ball");
        ball->set_position(1120, 560);
        ball->set_drawable(new turbo::CircleShape(45, turbo::Color(240, 205, 70)));

        this->root_gameobject = root;
    }

    void unload() override {
        delete this->root_gameobject;   // recursively frees the whole subtree
        this->root_gameobject = nullptr;
        delete this->texture;
        this->texture = nullptr;
    }

private:
    turbo::Texture* texture = nullptr;
};

// Note: the (argc, argv) signature is required for Allegro's "magic main" on
// macOS, which renames main to _al_mangled_main(int, char**). Standard and
// harmless on Windows/Linux.
int main(int argc, char** argv) {
    (void)argc; (void)argv;
    try {
        turbo::Engine engine;
        TestScene* test_scene = new TestScene();
        engine.scene_manager.register_scene(test_scene, "Demo Scene");
        engine.scene_manager.set_active_scene("Demo Scene");
        engine.start_window("Turbo Editor", 1600, 900);
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
