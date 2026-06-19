
#ifndef __TURBO_GAMEOBJECT_HPP__
#define __TURBO_GAMEOBJECT_HPP__

#include <vector>
#include <string>
#include <type_traits>
#include "Component.hpp"
#include "graphics/Drawable.hpp"
#include "Transform.hpp"

namespace turbo {
    class Material;

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

        // A GameObject owns its drawable, components and children through raw
        // pointers; copying it would double-free. Deleted until ownership is
        // expressed with smart pointers.
        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;

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

        unsigned short get_component_amount() const;

        void render(void* transform);
        void update(int delta_time);

        void set_drawable(Drawable* drawable);
        Drawable* get_drawable() const;

        /**
         * @brief Material applied to the drawable when rendering (null = default).
         * Not owned: materials are shared and owned by the ShaderLibrary.
         */
        void set_material(Material* material);
        /** @brief Material currently applied to the drawable (null = default). */
        Material* get_material() const;

        std::string get_name() const;

        /**
         * @brief Rename the object. Fails (returns false) if a sibling already
         * uses that name or the name is empty.
         */
        bool set_name(const std::string& name);

        /**
         * @brief Get the parent GameObject (null for the scene root)
         */
        GameObject* get_parent() const;

        /**
         * @brief Re-parent this object (detaching from its current parent first).
         * Passing nullptr detaches it from the hierarchy without destroying it.
         */
        void attach_to(GameObject* parent);

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
        std::vector<GameObject*> childs{};
        std::vector<Component*> components{};

    private:
        GameObject* parent = nullptr;
        std::string name;
        Drawable* drawable = nullptr;
        Material* material = nullptr;
    };
}

#endif