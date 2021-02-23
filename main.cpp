#include <iostream>
#include <turbo/Engine.hpp>
#include <turbo/components/SimpleControl.hpp>

class TestComponent: public turbo::Component {
public:
    using turbo::Component::Component;

    bool toRight = true;

    void load() override {
        this->name = "Test Component";
    }

    void unload() override {
    }

    void update(int delta_time) override {
        turbo::GameObject* game_obj = this->gameObject;
        delta_time /= 8;
        game_obj->rotate(delta_time / 2);
        if (game_obj->get_position().x > 400)
            this->toRight = false;
        if (game_obj->get_position().x < 30)
            this->toRight = true;
        if (!this->toRight)
            game_obj->translate(-1 * (delta_time / 2), 0);
        else
            game_obj->translate(delta_time / 2, 0);
    }
};

class TestScene: public turbo::Scene {
public:
    TestScene() = default;

    void load() override {
        turbo::Path* path = turbo::Path::get_resources_path();
        path->drop_tail();
        path->drop_tail();
        path->drop_tail();
        path->append_component("assets/demo");
        path->set_filename("OpenSans-Regular.ttf");

        this->font = new turbo::Font(*path, 30);

        path->set_filename("png-test.png");

        this->texture = new turbo::Texture(path);
        turbo::Sprite* sprite = new turbo::Sprite(texture);
        turbo::Text* text = new turbo::Text(this->font, "Hello World");
        turbo::Text* title = new turbo::Text(this->font, "This is fixed");

        turbo::GameObject* root = new turbo::GameObject(nullptr, "root");
        turbo::GameObject* parent = new turbo::GameObject(root, "Parent");
        turbo::GameObject* child = new turbo::GameObject(parent, "child");
        turbo::GameObject* other = new turbo::GameObject(root, "other");

        parent->set_drawable(sprite);
        child->set_drawable(text);
        other->set_drawable(title);

        TestComponent* component = new TestComponent(parent);
        turbo::component::SimpleControl* move_comp = new turbo::component::SimpleControl(other);

        parent->add_component(component);
        other->add_component(move_comp);

        this->root_gameobject = root;
        parent->set_position(350, 350);
        parent->set_center(150, 150);
        child->set_position(0, -30);
        other->set_position(50, 50);

        delete path;

    }

    void unload() override {
        for (turbo::GameObject* go : this->garbage)
            delete go;
        this->garbage.clear();
        delete this->font;
        delete this->texture;
        this->font = nullptr;
        this->texture = nullptr;
    }

private:
    turbo::Font* font;
    turbo::Texture* texture;
    std::list<turbo::GameObject*> garbage{};
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