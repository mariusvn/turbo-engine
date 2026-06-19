
#ifndef __TURBO_ENGINE_HPP__
#define __TURBO_ENGINE_HPP__

#include <string>
#include <memory>
#include <allegro5/allegro.h>
#include "Path.hpp"
#include "Logger.hpp"
#include "GameObject.hpp"
#include "Component.hpp"
#include "graphics/Graphics.hpp"
#include "graphics/ShaderLibrary.hpp"
#include "Rectangle.hpp"
#include "Scene.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"
#include "Transform.hpp"
#include "SceneManager.hpp"
#include "Font.hpp"
#include "Input.hpp"
#include "DebugImgui.hpp"
#include "RotativeBuffer.hpp"
#include "editor/Editor.hpp"

namespace turbo {
    namespace script { class ScriptEngine; }

    /**
     * @brief Main engine class
     */
    class Engine final {
    public:
        /**
         * @brief Initialize a new engine
         * @note Only one instance should be used at a time
         * 
         * @param win_name Name of the window
         * @param width width of the window
         * @param height height of the window
         */
        Engine();
        ~Engine();

        /**
         * @brief Create the display, initialise the renderer (and the editor
         * overlay when built with ImGui) and start the engine subsystems.
         * @param win_name window title
         * @param width window width in pixels
         * @param height window height in pixels
         */
        void start_window(const char* win_name, unsigned short width, unsigned short height);

        /** @brief Destroy the display and release the renderer. */
        void stop_window();

        /** @brief Change the window title. */
        void set_window_title(const char* title);

        /** @brief Resize the window. */
        void set_window_size(unsigned short width, unsigned short height);

        /**
         * @brief Starts main loop
         */
        void loop();

        /**
         * @brief Force the engine to render as soon as possible
         */
        void force_render();

        /**
         * @brief Force the engine to tick as soon as possible
         */
        void force_logic();

        /**
         * @brief Stops the main loop and close the window
         */
        void close();

        /**
         * @brief Whether scene logic (component updates) is currently running.
         * When false the engine still renders and processes input, but the
         * active scene is not ticked — this is the editor "edit mode".
         */
        bool is_simulating() const { return this->simulating; }

        /**
         * @brief Enable/disable scene logic ticking (editor Play/Pause).
         */
        void set_simulating(bool value) { this->simulating = value; }

        /**
         * @brief Engine version
         * @note This string is hard writter read only
         */
        inline static const std::string version = "0.3";

        /** @brief The active engine instance (set in the constructor). */
        static inline Engine* engine = nullptr;

        /** @brief Owns the registered scenes and the active scene. */
        SceneManager scene_manager;

        /** @brief Global keyboard/mouse input state. */
        static inline Input input = Input();

        /**
         * @brief Engine-wide registry of shader programs and materials.
         * Populated once the GL context exists (see start_window).
         */
        ShaderLibrary shaders;

        /**
         * @brief The Lua scripting VM (owns the engine-API bindings).
         */
        script::ScriptEngine* scripts() const { return this->script_engine.get(); }

        /**
         * @brief Run a line of Lua against the shared state (editor REPL). Thin
         * forwarder so callers don't need the sol2 headers.
         */
        void run_script_repl(const std::string& code);

        /** @brief Tick the active scene's logic (one fixed update step). */
        void on_update_tick();
        /** @brief Render the active scene (one frame). */
        void on_render_tick();

        /**
         * @brief Register the built-in shader programs/materials. Requires a GL
         * context, so it runs from start_window after the display is created.
         */
        void init_shader_library();

        /**
         * @brief Create the ImGui context, load the UI + monospace fonts and
         * apply the theme. No-op when built without ImGui.
         */
        void init_imgui();

        /** @brief Fired on every logic tick; subscribe to run game logic. */
        Event<> update_tick = Event<>();
        /** @brief Fired on every rendered frame; subscribe to draw. */
        Event<> render_tick = Event<>();
        /** @brief Engine-wide logger. */
        Logger logger = Logger("Turbo Engine");

        ONLYIMGUI(editor::Editor editor = editor::Editor(this));

    private:
        /** @brief Fixed logic timestep in milliseconds (60 Hz). */
        static constexpr int UPDATE_DELTA_MS = 1000 / 60;

        /**
         * @brief Timer driving the fixed-rate logic ticks (60 Hz).
         */
        ALLEGRO_TIMER* update_timer = nullptr;

        /**
         * @brief Update on the next main loop turn
         */
        bool update = false;

        /**
         * @brief Main loop event queue
         */
        ALLEGRO_EVENT_QUEUE* event_queue = nullptr;

        /**
         * @brief Main display
         */
        ALLEGRO_DISPLAY* display = nullptr;

        /**
         * @brief Main loop condition, closes the window if false
         */
        bool main_loop = true;

        /**
         * @brief When false the active scene is not ticked (editor edit mode)
         */
        bool simulating = false;

        /**
         * @brief Lua VM, created at construction (forward-declared to keep sol2
         * out of this header).
         */
        std::unique_ptr<script::ScriptEngine> script_engine;
    };
}

#endif