#include <turbo/GameObject.hpp>
#include <stdexcept>
#include <iostream>
#include <algorithm>

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
        this->components = std::vector<Component*>();
    }

    void GameObject::add_component(Component* comp) {
        this->components.push_back(comp);
        comp->load();
    }

    void GameObject::remove_component(Component* comp) {
        std::vector<Component *>& vec = this->components;
        vec.erase(std::remove(vec.begin(), vec.end(), comp), vec.end());
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

    std::string GameObject::get_name() const {
        return this->name;
    }

    unsigned short GameObject::get_component_amount() const {
        return this->components.size();
    }
}