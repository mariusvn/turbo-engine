#include <turbo/GameObject.hpp>
#include <turbo/graphics/Material.hpp>
#include <turbo/graphics/Shader.hpp>
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
        // A GameObject owns its subtree: destroying it recursively destroys its
        // children (previously leaked), its drawable and its components.
        for (GameObject* child : this->childs)
            delete child;
        this->childs.clear();
        delete this->drawable;
        for (Component* comp: this->components)
            delete comp;
        this->components.clear();
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
            ALLEGRO_TRANSFORM t_holder;
            al_copy_transform(&t_holder, transform);
            if (p_transform) {
                al_compose_transform(&t_holder, static_cast<ALLEGRO_TRANSFORM*>(p_transform));
            }
            al_use_transform(&t_holder);
            if (this->drawable) {
                if (this->material)
                    this->material->bind_at(static_cast<float>(al_get_time()));
                this->drawable->draw();
                if (this->material) {
                    // Restore the scene-wide default shader for the next object.
                    if (Shader* def = Shader::get_active_shader())
                        def->use();
                }
            }
            for (GameObject *go : this->childs) {
                go->render(&t_holder);
            }
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
        if (this->drawable != drawable)
            delete this->drawable;
        this->drawable = drawable;
    }

    Drawable* GameObject::get_drawable() const {
        return this->drawable;
    }

    void GameObject::set_material(Material* material) {
        this->material = material;
    }

    Material* GameObject::get_material() const {
        return this->material;
    }

    GameObject* GameObject::get_parent() const {
        return this->parent;
    }

    void GameObject::attach_to(GameObject* new_parent) {
        if (this->parent == new_parent)
            return;
        if (this->parent) {
            std::vector<GameObject*>& siblings = this->parent->childs;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        }
        this->parent = new_parent;
        if (new_parent)
            new_parent->childs.push_back(this);
    }

    bool GameObject::set_name(const std::string& new_name) {
        if (new_name.empty())
            return false;
        if (this->parent) {
            for (GameObject* sibling : this->parent->childs) {
                if (sibling != this && sibling->name == new_name)
                    return false;
            }
        }
        this->name = new_name;
        return true;
    }

    std::string GameObject::get_name() const {
        return this->name;
    }

    unsigned short GameObject::get_component_amount() const {
        return this->components.size();
    }
}