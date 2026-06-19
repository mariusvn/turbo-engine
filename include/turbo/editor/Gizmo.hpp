
#ifndef __TURBO_EDITOR_GIZMO_HPP__
#define __TURBO_EDITOR_GIZMO_HPP__

#ifdef __TURBO_USE_IMGUI__

#include <imgui/imgui.h>
#include "../Vector2.hpp"

namespace turbo { class GameObject; }

namespace turbo::editor {

    /**
     * @brief In-viewport manipulator for the selected GameObject.
     *
     * Draws translate / rotate / scale handles over the live scene (using
     * ImGui's background draw list, so the gizmo sits above the rendered scene
     * but below the editor panels) and edits the object's transform from mouse
     * drags. World <-> screen mapping is rebuilt every frame from the chain of
     * ancestor transforms, so the gizmo stays correct under nested, rotated or
     * scaled parents.
     */
    class Gizmo {
    public:
        enum class Mode { Translate, Rotate, Scale };

        void set_mode(Mode mode) { this->m_mode = mode; }
        Mode mode() const { return this->m_mode; }
        const char* mode_name() const;

        /**
         * @brief Draw and update the gizmo for `go`.
         * @return true while the gizmo is currently grabbing the mouse.
         */
        bool manipulate(turbo::GameObject* go);

        bool is_active() const { return this->dragging; }

    private:
        Mode m_mode = Mode::Translate;

        // Currently grabbed handle (see HANDLE_* in the .cpp); -1 when idle.
        int handle = -1;
        bool dragging = false;

        // Transform snapshot captured when a drag starts.
        ImVec2 start_mouse{0, 0};
        turbo::Vector2<float> start_pos{0, 0};
        float start_angle = 0.0f;
        turbo::Vector2<float> start_scale{1, 1};
    };
}

#endif // __TURBO_USE_IMGUI__
#endif // __TURBO_EDITOR_GIZMO_HPP__
