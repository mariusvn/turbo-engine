
#ifndef __TURBO_GAMEOBJECT_HPP__
#define __TURBO_GAMEOBJECT_HPP__

#include <list>
#include <string>
#include <type_traits>
#include "Component.hpp"
#include "graphics/Drawable.hpp"
#include "Transform.hpp"

namespace turbo {
    /**
     * @brief Basic scene object
     * 
     */
    class GameObject final: public Transform {
    public:
        /**
         * @brief Construct a new GameObject object
         * 
         * @param parent parent GameObject or null if root node
         * @param name GameObject name
         */
        explicit GameObject(GameObject* parent, std::string name);
        ~GameObject();

        /**
         * @brief Add a component to the GameObject
         * 
         * @param comp component instance
         */
        void add_component(Component* comp);

        /**
         * @brief Remove a component to the GameObject
         * 
         * @param comp component instance
         */
        void remove_component(Component* comp);

        void render(void* transform);
        void update(int delta_time);

        void set_drawable(Drawable* drawable);
        Drawable* get_drawable() const;

        /**
         * @brief Get the component object
         * 
         * @tparam T Type of component
         * @return T* instance of the component
         */
        template<
        typename T,
        typename = std::enable_if_t<std::is_base_of_v<Component, T>>
        >
        T* get_component() {
            for (Component* comp : this->components) {
                if (T* ptr = dynamic_cast<T*>(comp))
                    return ptr;
            }
            return nullptr;
        }

        bool show = true;

    private:
        std::list<Component*> components{};
        std::list<GameObject*> childs{};
        GameObject* parent = nullptr;
        std::string name;
        Drawable* drawable = nullptr;
    };
}

#endif