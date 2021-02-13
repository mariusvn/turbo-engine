
#ifndef __TURBO_ENGINE_HPP__
#define __TURBO_ENGINE_HPP__

#include <string>
#include <allegro5/allegro.h>
#include "Path.hpp"
#include "Logger.hpp"
#include "GameObject.hpp"
#include "Component.hpp"
#include "graphics/Graphics.hpp"
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

namespace turbo {
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

        void start_window(const char* win_name, unsigned short width, unsigned short height);
        void stop_window();
        void set_window_title(const char* title);
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
         * @brief Engine version
         * @note This string is hard writter read only
         */
        inline static const std::string version = "0.2";

        static inline Engine* engine = nullptr;

        SceneManager scene_manager;
        static inline Input input = Input();

        void on_update_tick();
        void on_render_tick();
        Event<> update_tick = Event<>();
        Event<> render_tick = Event<>();

        ONLYIMGUI(debug::EngineDebug debug = debug::EngineDebug(this));

    private:

        /**
         * @brief Timer for the render frames
         */
        ALLEGRO_TIMER* render_timer = nullptr;

        /**
         * @brief Render on next main loop turn
         */
        bool render = false;

        /**
         * @brief Timer for the logic frames
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

        unsigned long _loop_time = clock();
        float loop_time = 0;
        unsigned long fps_actualizer = clock();
    };
}

#endif


/**
 * @mainpage Turbo Engine Documentation
 * <a href="hierarchy.html">Class hierarchy</a>
 * 
 */