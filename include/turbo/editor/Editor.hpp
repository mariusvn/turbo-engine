
#ifndef __TURBO_EDITOR_EDITOR_HPP__
#define __TURBO_EDITOR_EDITOR_HPP__

/*
 * The editor is the demo's main interface. It is built entirely on top of the
 * Dear ImGui overlay, so the whole class only exists when the engine is
 * compiled with __TURBO_USE_IMGUI__. Without it, this header is empty and the
 * Engine simply has no `editor` member.
 */
#ifdef __TURBO_USE_IMGUI__

#include <string>
#include <vector>
#include <streambuf>
#include "../RotativeBuffer.hpp"
#include "../debug_menus/Inspector.hpp"
#include "Gizmo.hpp"

namespace turbo {
    class Engine;
    class GameObject;
    class Component;
    class LuaComponent;
}

namespace turbo::editor {

    /**
     * @brief A small ImGui scene editor: hierarchy, inspector, console and a
     * Play/Pause toolbar laid out around the live viewport.
     *
     * The editor never owns engine data; it observes the active scene through
     * the Engine and mutates GameObjects in place. Structural changes (create /
     * delete) are deferred to the end of the frame so the scene tree is never
     * mutated while it is being walked.
     */
    class Editor {
    public:
        explicit Editor(turbo::Engine* engine);
        ~Editor();

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        /** @brief Build the whole editor UI for the current frame. */
        void render();

        /**
         * @brief Allegro-side overlay drawn after the scene and before ImGui:
         * the shader-based selection outline. Call from the render pass.
         */
        void draw_scene_overlay();

        /** @brief Feed a frames-per-second sample to the stats graph. */
        void push_fps(float fps);

        /** @brief Append a line to the editor console. */
        void log(const std::string& line);

    private:
        float draw_menu_bar();
        void draw_toolbar(float y, float height);
        void draw_hierarchy(float x, float y, float w, float h);
        void draw_inspector(float x, float y, float w, float h);
        void draw_console(float x, float y, float w, float h);
        void draw_stats_overlay(float x, float y);
        void draw_about();
        void draw_script_editor();
        void draw_code_highlight(ImDrawList* draw_list, const ImVec2& origin, float char_w, float line_h);

        void open_script_file(const std::string& path);
        void save_script_file();
        void reload_script_in_scene(const std::string& path);
        void validate_selection();

    public:
        /** @brief Monospace font used by the Lua code editor (set by the Engine). */
        ImFont* code_font = nullptr;

    private:

        void draw_hierarchy_node(turbo::GameObject* go, turbo::GameObject* root);
        void draw_transform_editor(turbo::GameObject* go);
        void draw_drawable_section(turbo::GameObject* go);
        void draw_material_section(turbo::GameObject* go);
        void draw_component(turbo::Component* comp, int index);
        void draw_script_component(turbo::LuaComponent* script);
        void draw_observer(debug::InspectorObserver& obs, int uid);
        void draw_grid();
        void refresh_scripts();

        turbo::GameObject* active_root() const;
        turbo::GameObject* create_object(turbo::GameObject* parent, int kind);
        turbo::GameObject* clone_object(turbo::GameObject* src, turbo::GameObject* parent);
        void destroy_object(turbo::GameObject* go);
        void apply_pending();

        turbo::Engine* engine = nullptr;
        turbo::GameObject* selected = nullptr;

        // Deferred structural edits, applied once the hierarchy is fully drawn.
        turbo::GameObject* pending_add_parent = nullptr;
        int pending_add_kind = -1;
        turbo::GameObject* pending_delete = nullptr;
        turbo::GameObject* pending_duplicate = nullptr;

        bool show_hierarchy = true;
        bool show_inspector = true;
        bool show_console = true;
        bool show_stats = true;
        bool show_about = false;
        bool show_grid = true;
        bool show_script_editor = false;

        // Lua code editor state.
        std::string editor_script_path;
        std::string editor_buffer;
        std::string editor_status;
        char repl_buf[512] = {0};
        char new_script_name[64] = {0};

        Gizmo gizmo;

        // Rename scratch buffer, refilled when the selection changes.
        char name_buf[128] = {0};
        turbo::GameObject* name_buf_owner = nullptr;

        float fps = 0.0f;
        turbo::RotativeBuffer<float> fps_history = turbo::RotativeBuffer<float>(0.0f, 90);

        std::vector<std::string> logs;
        bool console_autoscroll = true;

        // Cached list of assets/scripts/*.lua paths for the "Add Script" picker.
        std::vector<std::string> available_scripts;

        // std::cout capture so engine logs surface in the console panel.
        class ConsoleStreambuf;
        ConsoleStreambuf* console_buf = nullptr;
        std::streambuf* previous_cout = nullptr;
    };
}

#endif // __TURBO_USE_IMGUI__
#endif // __TURBO_EDITOR_EDITOR_HPP__
