#include <turbo/Engine.hpp>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <turbo/graphics/Shader.hpp>
#include <turbo/DebugImgui.hpp>

namespace turbo {
    Engine::Engine() {
        this->engine = this;
        al_init();
        al_install_keyboard();
        al_init_font_addon();
        al_init_ttf_addon();
        al_install_mouse();
        al_init_image_addon();
        al_init_primitives_addon();

        this->render_timer = al_create_timer(1.0 / 60.0);
        this->update_timer = al_create_timer(1.0 / 60.0);

        this->event_queue = al_create_event_queue();

        al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
        al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
        al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);

        al_register_event_source(this->event_queue, al_get_keyboard_event_source());
        al_register_event_source(this->event_queue, al_get_mouse_event_source());
        al_register_event_source(this->event_queue, al_get_timer_event_source(this->render_timer));
        al_register_event_source(this->event_queue, al_get_timer_event_source(this->update_timer));

        this->render_tick += [this] { on_render_tick(); };
        this->update_tick += [this] { on_update_tick(); };

        ONLYIMGUI(this->scene_manager.debug.render());
    }

    void Engine::loop() {
        ALLEGRO_EVENT event;

        al_start_timer(this->render_timer);
        al_start_timer(this->update_timer);

        while (this->main_loop) {

            al_wait_for_event(this->event_queue, &event);

            ONLYIMGUI(ImGui_ImplAllegro5_ProcessEvent(&event));

            //TODO refactor with event system
            if (event.type == ALLEGRO_EVENT_TIMER) {

                if (event.timer.source == this->render_timer) {
                    this->force_render();
                } else if (event.timer.source == this->update_timer) {
                    this->force_logic();
                }

            } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                this->close();
                continue;
            } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                Engine::input.process_key_down_event(&event);
            } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
                Engine::input.process_key_up_event(&event);
            } else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
                ONLYIMGUI(ImGui_ImplAllegro5_InvalidateDeviceObjects());
                al_acknowledge_resize(display);
                ONLYIMGUI(ImGui_ImplAllegro5_CreateDeviceObjects());
            }

            if (this->main_loop && al_is_event_queue_empty(this->event_queue)) {
                
                if (this->update) {
                    this->update_tick();
                    this->update = false;
                }

                if (this->render) {
                    this->loop_time = (float) (((double)(clock() - this->_loop_time)) / CLOCKS_PER_SEC);
                    this->_loop_time = clock();

                    ONLYIMGUI(
                        this->debug.register_fps_time(1 / this->loop_time);
                        ImGui_ImplAllegro5_NewFrame();
                        ImGui::NewFrame();
                        );
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    this->render_tick();
                    ONLYIMGUI(
                        this->debug.render();
                        ImGui::Render();
                        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData())
                        );
                    al_flip_display();
                    this->render = false;

                }

            }
        }

        al_stop_timer(this->render_timer);
        al_stop_timer(this->update_timer);
    }

    Engine::~Engine() {
        al_destroy_timer(this->render_timer);
        al_destroy_timer(this->update_timer);
        al_destroy_event_queue(this->event_queue);

        al_uninstall_keyboard();
        al_uninstall_system();
    }

    void Engine::start_window(const char* win_name, unsigned short width, unsigned short height) {
        this->display = al_create_display(width, height);
        if (!this->display)
            throw std::runtime_error("Cannot open window");
        al_set_window_title(this->display, win_name);
        al_register_event_source(this->event_queue, al_get_display_event_source(this->display));
        Shader::init_shaders();

        ONLYIMGUI(
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();
            ImGui_ImplAllegro5_Init(this->display);
            this->debug.update_win_size(turbo::Vector2(width, height));
        );
    }

    void Engine::stop_window() {
        ONLYIMGUI(ImGui_ImplAllegro5_Shutdown());
        al_use_shader(nullptr);
        al_unregister_event_source(this->event_queue, al_get_display_event_source(this->display));
        al_destroy_display(this->display);
        this->display = nullptr;
    }

    void Engine::set_window_title(const char* title) {
        al_set_window_title(this->display, title);
    }

    void Engine::set_window_size(unsigned short width, unsigned short height) {
        al_resize_display(this->display, width, height);
        ONLYIMGUI(this->debug.update_win_size(turbo::Vector2(width, height)));
    }

    void Engine::force_render() {
        this->render = true;
    }

    void Engine::force_logic() {
        this->update = true;
    }

    void Engine::close() {
        this->main_loop = false;
    }

    void Engine::on_render_tick() {
        turbo::Scene* scene = this->scene_manager.get_active_scene();
        if (scene)
            scene->render();
    }

    void Engine::on_update_tick() {
        if (((float)(clock() - fps_actualizer)) / CLOCKS_PER_SEC >= 0.2) {
            ONLYIMGUI(this->debug.update_fps((int)(1.0f / (float)this->loop_time)));
            this->fps_actualizer = clock();
        }
        Engine::input.process_timer_event();
        turbo::Scene* active_scene = this->scene_manager.get_active_scene();
        if (active_scene)
            active_scene->update(1000*loop_time);
    }
}