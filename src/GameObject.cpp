#include <turbo/GameObject.hpp>
#include <stdexcept>
#include <iostream>

namespace turbo {

    GameObject::GameObject(GameObject* parent, std::string name): parent(parent), name(name) {
        if (parent) {
            for (GameObject* child : parent->childs) {
                if (child->name == name) {
                    throw std::runtime_error(std::string("The name \"" + name + "\" is not available as child of \"" + parent->name + "\"").c_str());
                }
            }
            parent->childs.push_back(this);
        }
    }
    GameObject::~GameObject() {
        delete this->drawable;
        for (Component* comp: this->components)
            delete comp;
        this->components = std::list<Component*>();
    }

    void GameObject::add_component(Component* comp) {
        this->components.push_back(comp);
        comp->load();
    }

    void GameObject::remove_component(Component* comp) {
        this->components.remove(comp);
        comp->unload();
    }

    void GameObject::render(void* p_transform) {
        if (this->show) {
            ALLEGRO_TRANSFORM* transform = this->get_transformer();
            ALLEGRO_TRANSFORM* t_holder = new ALLEGRO_TRANSFORM();
            al_copy_transform(t_holder, transform);
            al_use_transform(t_holder);
            if (p_transform) {
                al_compose_transform(t_holder, static_cast<ALLEGRO_TRANSFORM*>(p_transform));
            }
            al_use_transform(t_holder);
            if (this->drawable) {
                this->drawable->draw();
            }
            for (GameObject *go : this->childs) {
                go->render(t_holder);
            }
            delete t_holder;
        }
    }

    void GameObject::update(int delta_time) {
        for (Component* comp : this->components) {
            comp->update(delta_time);
        }
        for (GameObject* go : this->childs) {
            go->update(delta_time);
        }
    }

    void GameObject::set_drawable(Drawable* drawable) {
        this->drawable = drawable;
    }

    Drawable* GameObject::get_drawable() const {
        return this->drawable;
    }
}