#ifdef __TURBO_USE_IMGUI__

#include <turbo/editor/Gizmo.hpp>
#include <turbo/GameObject.hpp>
#include <turbo/Utils.hpp>

#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>

namespace turbo::editor {

    namespace {
        // Handle ids.
        constexpr int HANDLE_X      = 0;   // X axis (translate / scale)
        constexpr int HANDLE_Y      = 1;   // Y axis (translate / scale)
        constexpr int HANDLE_CENTER = 2;   // free move / uniform scale
        constexpr int HANDLE_RING   = 3;   // rotation ring

        constexpr float AXIS_LEN = 82.0f;
        constexpr float TIP      = 9.0f;
        constexpr float PICK     = 7.0f;
        constexpr float RING_R   = 70.0f;

        const ImU32 COL_X      = IM_COL32(230, 80,  80,  255);
        const ImU32 COL_Y      = IM_COL32(90,  200, 90,  255);
        const ImU32 COL_CENTER = IM_COL32(240, 240, 90,  255);
        const ImU32 COL_RING   = IM_COL32(90,  165, 240, 255);
        const ImU32 COL_HOT    = IM_COL32(255, 235, 120, 255);

        // --- tiny ImVec2 helpers (ImGui math operators aren't enabled here) ---
        ImVec2 add(ImVec2 a, ImVec2 b)  { return ImVec2(a.x + b.x, a.y + b.y); }
        ImVec2 sub(ImVec2 a, ImVec2 b)  { return ImVec2(a.x - b.x, a.y - b.y); }
        ImVec2 mul(ImVec2 a, float s)   { return ImVec2(a.x * s, a.y * s); }
        float  dot(ImVec2 a, ImVec2 b)  { return a.x * b.x + a.y * b.y; }
        float  length(ImVec2 a)         { return std::sqrt(a.x * a.x + a.y * a.y); }
        float  dist(ImVec2 a, ImVec2 b) { return length(sub(a, b)); }
        ImVec2 normalize(ImVec2 a) {
            float l = length(a);
            return (l > 1e-6f) ? ImVec2(a.x / l, a.y / l) : ImVec2(0, 0);
        }
        float seg_dist(ImVec2 p, ImVec2 a, ImVec2 b) {
            ImVec2 ab = sub(b, a);
            float len2 = dot(ab, ab);
            if (len2 < 1e-6f) return dist(p, a);
            float t = dot(sub(p, a), ab) / len2;
            t = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t);
            return dist(p, add(a, mul(ab, t)));
        }

        // Local transform of a single node (mirrors Transform::get_transformer).
        ALLEGRO_TRANSFORM local_tf(turbo::GameObject* go) {
            ALLEGRO_TRANSFORM t;
            al_identity_transform(&t);
            const turbo::Vector2<float> s = go->get_scale();
            const turbo::Vector2<int>   c = go->get_center();
            const turbo::Vector2<float> p = go->get_position();
            al_scale_transform(&t, s.x, s.y);
            al_translate_transform(&t, -static_cast<float>(c.x), -static_cast<float>(c.y));
            al_rotate_transform(&t, static_cast<float>(DEGTORAD(go->get_angle())));
            al_translate_transform(&t, static_cast<float>(c.x) + p.x, static_cast<float>(c.y) + p.y);
            return t;
        }

        // Transform mapping `go`'s parent space to screen space (identity at root).
        ALLEGRO_TRANSFORM parent_world(turbo::GameObject* go) {
            ALLEGRO_TRANSFORM acc;
            al_identity_transform(&acc);
            turbo::GameObject* parent = go->get_parent();
            if (!parent) return acc;          // root: parent space already == screen
            acc = local_tf(parent);
            for (turbo::GameObject* n = parent->get_parent(); n; n = n->get_parent()) {
                ALLEGRO_TRANSFORM nl = local_tf(n);
                al_compose_transform(&acc, &nl);  // apply acc, then n's local
            }
            return acc;
        }

        void draw_arrow(ImDrawList* dl, ImVec2 from, ImVec2 to, ImVec2 dir, ImU32 col) {
            const ImU32 shadow = IM_COL32(0, 0, 0, 200);
            ImVec2 perp(-dir.y, dir.x);
            ImVec2 base = sub(to, mul(dir, TIP * 1.6f));
            // dark backing so the handle stays readable over any object colour
            dl->AddLine(from, to, shadow, 5.0f);
            dl->AddTriangleFilled(add(to, mul(dir, 2.0f)),
                                  add(base, mul(perp, TIP * 0.85f)),
                                  sub(base, mul(perp, TIP * 0.85f)), shadow);
            dl->AddLine(from, to, col, 2.5f);
            dl->AddTriangleFilled(to, add(base, mul(perp, TIP * 0.6f)),
                                  sub(base, mul(perp, TIP * 0.6f)), col);
        }
    }

    const char* Gizmo::mode_name() const {
        switch (this->m_mode) {
            case Mode::Translate: return "Move";
            case Mode::Rotate:    return "Rotate";
            case Mode::Scale:     return "Scale";
        }
        return "?";
    }

    bool Gizmo::manipulate(turbo::GameObject* go) {
        if (!go) { this->dragging = false; this->handle = -1; return false; }

        ImGuiIO& io = ImGui::GetIO();
        const ImVec2 mouse = io.MousePos;

        // --- world <-> screen mapping for this frame ---
        ALLEGRO_TRANSFORM pw = parent_world(go);
        ALLEGRO_TRANSFORM ipw = pw;
        al_invert_transform(&ipw);

        const turbo::Vector2<float> pos = go->get_position();
        float sx = pos.x, sy = pos.y;
        al_transform_coordinates(&pw, &sx, &sy);
        const ImVec2 pivot(sx, sy);

        float axp = pos.x + 1.0f, ayp = pos.y;
        al_transform_coordinates(&pw, &axp, &ayp);
        const ImVec2 dir_x = normalize(sub(ImVec2(axp, ayp), pivot));

        float bxp = pos.x, byp = pos.y + 1.0f;
        al_transform_coordinates(&pw, &bxp, &byp);
        const ImVec2 dir_y = normalize(sub(ImVec2(bxp, byp), pivot));

        const ImVec2 x_tip = add(pivot, mul(dir_x, AXIS_LEN));
        const ImVec2 y_tip = add(pivot, mul(dir_y, AXIS_LEN));

        // --- hit testing (skip starting a drag while the cursor is over a panel) ---
        int hovered = -1;
        if (this->dragging) {
            hovered = this->handle;
        } else if (!io.WantCaptureMouse) {
            if (this->m_mode == Mode::Rotate) {
                if (std::fabs(dist(mouse, pivot) - RING_R) < 8.0f) hovered = HANDLE_RING;
            } else {
                if (dist(mouse, pivot) < 11.0f) hovered = HANDLE_CENTER;
                else if (dist(mouse, x_tip) < TIP + 2.0f || seg_dist(mouse, pivot, x_tip) < PICK) hovered = HANDLE_X;
                else if (dist(mouse, y_tip) < TIP + 2.0f || seg_dist(mouse, pivot, y_tip) < PICK) hovered = HANDLE_Y;
            }
        }

        // --- drawing (background list: above the scene, below the panels) ---
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        if (this->m_mode == Mode::Rotate) {
            dl->AddCircle(pivot, RING_R, hovered == HANDLE_RING ? COL_HOT : COL_RING, 64, 2.5f);
            if (this->dragging) dl->AddLine(pivot, mouse, COL_HOT, 1.5f);
            dl->AddCircleFilled(pivot, 3.5f, COL_RING);
        } else {
            const bool scale = (this->m_mode == Mode::Scale);
            draw_arrow(dl, pivot, x_tip, dir_x, hovered == HANDLE_X ? COL_HOT : COL_X);
            draw_arrow(dl, pivot, y_tip, dir_y, hovered == HANDLE_Y ? COL_HOT : COL_Y);
            if (scale) {
                dl->AddRectFilled(sub(x_tip, ImVec2(5, 5)), add(x_tip, ImVec2(5, 5)),
                                  hovered == HANDLE_X ? COL_HOT : COL_X);
                dl->AddRectFilled(sub(y_tip, ImVec2(5, 5)), add(y_tip, ImVec2(5, 5)),
                                  hovered == HANDLE_Y ? COL_HOT : COL_Y);
            }
            const ImU32 cc = hovered == HANDLE_CENTER ? COL_HOT : COL_CENTER;
            dl->AddRectFilled(sub(pivot, ImVec2(6, 6)), add(pivot, ImVec2(6, 6)), IM_COL32(0, 0, 0, 200));
            dl->AddRectFilled(sub(pivot, ImVec2(5, 5)), add(pivot, ImVec2(5, 5)), cc);
        }

        // --- begin drag ---
        if (!this->dragging && hovered != -1 &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !io.WantCaptureMouse) {
            this->dragging    = true;
            this->handle      = hovered;
            this->start_mouse = mouse;
            this->start_pos   = pos;
            this->start_angle = go->get_angle();
            this->start_scale = go->get_scale();
        }

        // --- update drag ---
        if (this->dragging) {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                this->dragging = false;
                this->handle = -1;
                return false;
            }

            if (this->m_mode == Mode::Translate) {
                float lnx = mouse.x, lny = mouse.y;
                al_transform_coordinates(&ipw, &lnx, &lny);
                float lsx = this->start_mouse.x, lsy = this->start_mouse.y;
                al_transform_coordinates(&ipw, &lsx, &lsy);
                float dx = lnx - lsx, dy = lny - lsy;
                if (this->handle == HANDLE_X) dy = 0.0f;
                else if (this->handle == HANDLE_Y) dx = 0.0f;
                go->set_position(this->start_pos.x + dx, this->start_pos.y + dy);

            } else if (this->m_mode == Mode::Rotate) {
                float a0 = std::atan2(this->start_mouse.y - pivot.y, this->start_mouse.x - pivot.x);
                float a1 = std::atan2(mouse.y - pivot.y, mouse.x - pivot.x);
                float deg = this->start_angle + (a1 - a0) * 180.0f / static_cast<float>(ALLEGRO_PI);
                while (deg < 0.0f)    deg += 360.0f;
                while (deg >= 360.0f) deg -= 360.0f;
                go->set_rotation(deg);

            } else { // Scale
                if (this->handle == HANDLE_CENTER) {
                    float d0 = dist(this->start_mouse, pivot);
                    float factor = d0 > 1e-3f ? dist(mouse, pivot) / d0 : 1.0f;
                    go->set_scale(std::max(0.01f, this->start_scale.x * factor),
                                  std::max(0.01f, this->start_scale.y * factor));
                } else {
                    ImVec2 axis = (this->handle == HANDLE_X) ? dir_x : dir_y;
                    float p0 = dot(sub(this->start_mouse, pivot), axis);
                    float p1 = dot(sub(mouse, pivot), axis);
                    float factor = std::fabs(p0) > 1e-3f ? p1 / p0 : 1.0f;
                    float nx = this->start_scale.x, ny = this->start_scale.y;
                    if (this->handle == HANDLE_X) nx = std::max(0.01f, this->start_scale.x * factor);
                    else                          ny = std::max(0.01f, this->start_scale.y * factor);
                    go->set_scale(nx, ny);
                }
            }
        }

        return this->dragging;
    }
}

#endif // __TURBO_USE_IMGUI__
