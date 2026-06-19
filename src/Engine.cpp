#include <turbo/Engine.hpp>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <turbo/graphics/Shader.hpp>
#include <turbo/DebugImgui.hpp>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <chrono>
#include <stdexcept>
#include <filesystem>
#include <turbo/script/ScriptEngine.hpp>

namespace turbo {
    Engine::Engine() {
        turbo::Engine::engine = this;
        this->script_engine = std::make_unique<script::ScriptEngine>();

        this->logger.info("Initializing system");
        if (!al_init()) {
            this->logger.error("Cannot initialize system");
            throw std::runtime_error("Cannot initialize Allegro system");
        }

        std::function<bool()> initializers[] = {
            al_install_keyboard,
            al_init_acodec_addon,
            al_install_audio,
            al_init_font_addon,
            al_init_ttf_addon,
            al_install_mouse,
            al_init_image_addon,
            al_init_primitives_addon,
            nullptr
        };

        std::string initializer_names[] = {
            "keyboard",
            "audio codecs",
            "audio",
            "font",
            "fft",
            "mouse",
            "image",
            "primitives",
            ""
        };

        for (unsigned short i = 0; initializers[i]; i++) {
            auto& func = initializers[i];
            this->logger.info("Initializing " + initializer_names[i]);
            if (!func()) {
                this->logger.error("Cannot initialize " + initializer_names[i]);
                throw std::runtime_error("Cannot initialize " + initializer_names[i]);
            }
        }

        al_reserve_samples(16);

        // Logic ticks at a fixed 60 Hz; rendering is uncapped and paced by the
        // display's vsync instead (so the frame rate follows the monitor).
        this->update_timer = al_create_timer(1.0 / 60.0);

        this->event_queue = al_create_event_queue();

        al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
        al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
        al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
        al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);

        al_register_event_source(this->event_queue, al_get_keyboard_event_source());
        al_register_event_source(this->event_queue, al_get_mouse_event_source());
        al_register_event_source(this->event_queue, al_get_timer_event_source(this->update_timer));

        this->render_tick += [this] { on_render_tick(); };
        this->update_tick += [this] { on_update_tick(); };
    }

    void Engine::loop() {
        ALLEGRO_EVENT event;
        al_start_timer(this->update_timer);
        double last_frame = al_get_time();

        while (this->main_loop) {
            // Drain every pending event without blocking, so rendering can run
            // free at the vsync rate instead of being gated by a render timer.
            while (al_get_next_event(this->event_queue, &event)) {
                ONLYIMGUI(ImGui_ImplAllegro5_ProcessEvent(&event));

                if (event.type == ALLEGRO_EVENT_TIMER) {
                    if (event.timer.source == this->update_timer)
                        this->update = true;
                } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    this->close();
                } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                    Engine::input.process_key_down_event(&event);
                } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
                    Engine::input.process_key_up_event(&event);
                } else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
                    ONLYIMGUI(ImGui_ImplAllegro5_InvalidateDeviceObjects());
                    al_acknowledge_resize(this->display);
                    ONLYIMGUI(ImGui_ImplAllegro5_CreateDeviceObjects());
                } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
                    Input::internal().update_mouse_position(&Engine::input, event.mouse.x, event.mouse.y);
                }
            }

            if (!this->main_loop)
                break;

            if (this->update) {
                this->update_tick();
                this->update = false;
            }
            // Apply queued spawn/destroy from scripts (and the REPL) here, once
            // the scene tree is no longer being walked.
            this->script_engine->run_deferred();

            const double now = al_get_time();
            const double dt = now - last_frame;
            last_frame = now;
            if (dt > 0.0)
                ONLYIMGUI(this->editor.push_fps(static_cast<float>(1.0 / dt)));

            ONLYIMGUI(
                ImGui_ImplAllegro5_NewFrame();
                ImGui::NewFrame();
            );
            al_clear_to_color(al_map_rgb(18, 18, 16));   // washed-black viewport
            this->render_tick();
            ONLYIMGUI(this->editor.draw_scene_overlay());
            ONLYIMGUI(
                this->editor.render();
                ImGui::Render();
                ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData())
            );
            al_flip_display();   // blocks until vblank when vsync is honoured
        }

        al_stop_timer(this->update_timer);
    }

    Engine::~Engine() {
        al_destroy_timer(this->update_timer);
        al_destroy_event_queue(this->event_queue);

        al_uninstall_keyboard();
        al_uninstall_system();
    }

    void Engine::start_window(const char* win_name, unsigned short width, unsigned short height) {
        this->display = al_create_display(width, height);
        if (!this->display)
            throw std::runtime_error("Cannot open window");
        turbo::Input::internal().set_display(&turbo::Engine::input, this->display);
        al_set_window_title(this->display, win_name);
        al_register_event_source(this->event_queue, al_get_display_event_source(this->display));
        Shader::init_shaders();
        this->init_shader_library();

        this->init_imgui();   // no-op build without ImGui
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
    }

    void Engine::force_render() {
        // Rendering is now continuous (vsync-paced); kept for API compatibility.
    }

    void Engine::force_logic() {
        this->update = true;
    }

    void Engine::close() {
        this->main_loop = false;
    }

    void Engine::run_script_repl(const std::string& code) {
        if (this->script_engine)
            this->script_engine->run_string(code);
    }

    void Engine::on_render_tick() {
        turbo::Scene* scene = this->scene_manager.get_active_scene();
        if (scene)
            scene->render();
    }

    void Engine::init_imgui() {
#ifdef __TURBO_USE_IMGUI__
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // A real UI font (OpenSans) is the biggest tell that this isn't stock
        // ImGui. Falls back to the built-in font if the file is missing.
        Path* font_path = Path::get_resources_path();
        font_path->append_component("assets");
        font_path->append_component("demo");
        font_path->set_filename("OpenSans-Regular.ttf");
        if (std::filesystem::exists(font_path->c_str()))
            io.Fonts->AddFontFromFileTTF(font_path->c_str(), 18.0f);
        delete font_path;

        // Monospace font for the Lua code editor: prefer a real system mono,
        // otherwise fall back to ImGui's built-in (also monospace) font.
        ImFont* mono = nullptr;
        const char* mono_candidates[] = {
            "C:\\Windows\\Fonts\\consola.ttf",
            "C:\\Windows\\Fonts\\CascadiaMono.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
            "/Library/Fonts/Menlo.ttc",
            "/System/Library/Fonts/Menlo.ttc",
        };
        for (const char* cand : mono_candidates) {
            if (std::filesystem::exists(cand)) {
                mono = io.Fonts->AddFontFromFileTTF(cand, 16.0f);
                if (mono) break;
            }
        }
        if (!mono) mono = io.Fonts->AddFontDefault();
        this->editor.code_font = mono;

        ImGui_ImplAllegro5_Init(this->display);
        debug::DebugWindow::set_color_theme();
#endif
    }

    void Engine::init_shader_library() {
        // Reuse Allegro's default vertex program so the example pixel shaders
        // stay compatible with the fixed attributes/varyings of the pipeline.
        const char* vsrc = al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_VERTEX_SHADER);
        const std::string vertex = vsrc ? vsrc : "";

        // Example: adjustable desaturation.
        const std::string grayscale = R"GLSL(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D al_tex;
uniform bool al_use_tex;
uniform float saturation;
varying vec4 varying_color;
varying vec2 varying_texcoord;
void main() {
    vec4 c = al_use_tex ? texture2D(al_tex, varying_texcoord) * varying_color : varying_color;
    float g = dot(c.rgb, vec3(0.299, 0.587, 0.114));
    c.rgb = mix(vec3(g), c.rgb, saturation);
    gl_FragColor = c;
}
)GLSL";

        // Example: multiplicative colour tint.
        const std::string tint = R"GLSL(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D al_tex;
uniform bool al_use_tex;
uniform vec3 tint;
varying vec4 varying_color;
varying vec2 varying_texcoord;
void main() {
    vec4 c = al_use_tex ? texture2D(al_tex, varying_texcoord) * varying_color : varying_color;
    gl_FragColor = vec4(c.rgb * tint, c.a);
}
)GLSL";

        // Negative colours.
        const std::string invert = R"GLSL(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D al_tex;
uniform bool al_use_tex;
varying vec4 varying_color;
varying vec2 varying_texcoord;
void main() {
    vec4 c = al_use_tex ? texture2D(al_tex, varying_texcoord) * varying_color : varying_color;
    gl_FragColor = vec4(1.0 - c.rgb, c.a);
}
)GLSL";

        // Animated brightness pulse (driven by the per-frame "time" uniform).
        const std::string pulse = R"GLSL(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D al_tex;
uniform bool al_use_tex;
uniform float time;
uniform float speed;
uniform float amount;
varying vec4 varying_color;
varying vec2 varying_texcoord;
void main() {
    vec4 c = al_use_tex ? texture2D(al_tex, varying_texcoord) * varying_color : varying_color;
    float p = 1.0 + amount * sin(time * speed);
    gl_FragColor = vec4(c.rgb * p, c.a);
}
)GLSL";

        // Screen-space scanlines (works on shapes and sprites via gl_FragCoord).
        const std::string scanlines = R"GLSL(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D al_tex;
uniform bool al_use_tex;
uniform float time;
uniform float density;
varying vec4 varying_color;
varying vec2 varying_texcoord;
void main() {
    vec4 c = al_use_tex ? texture2D(al_tex, varying_texcoord) * varying_color : varying_color;
    float s = 0.7 + 0.3 * step(0.0, sin(gl_FragCoord.y * density + time * 6.0));
    gl_FragColor = vec4(c.rgb * s, c.a);
}
)GLSL";

        // Editor-only selection silhouette. Underscore-prefixed so the material
        // inspector hides it; the editor draws it directly for the outline pass.
        const std::string outline = R"GLSL(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D al_tex;
uniform bool al_use_tex;
uniform vec4 outline_color;
varying vec4 varying_color;
varying vec2 varying_texcoord;
void main() {
    float a = al_use_tex ? texture2D(al_tex, varying_texcoord).a : varying_color.a;
    if (a < 0.05) discard;
    gl_FragColor = outline_color;
}
)GLSL";

        this->shaders.default_shader();
        this->shaders.load_source("grayscale", vertex, grayscale);
        this->shaders.load_source("tint", vertex, tint);
        this->shaders.load_source("invert", vertex, invert);
        this->shaders.load_source("pulse", vertex, pulse);
        this->shaders.load_source("scanlines", vertex, scanlines);
        this->shaders.load_source("__outline__", vertex, outline);

        // Sample materials shown in the editor's material picker.
        if (Material* m = this->shaders.create_material("Tint", "tint"))
            m->set_vec3("tint", 1.0f, 0.65f, 0.65f);
        if (Material* m = this->shaders.create_material("Grayscale", "grayscale"))
            m->set_float("saturation", 0.0f);
        this->shaders.create_material("Invert", "invert");
        if (Material* m = this->shaders.create_material("Pulse", "pulse")) {
            m->set_float("time", 0.0f);   // advanced each frame by bind_at()
            m->set_float("speed", 4.0f);
            m->set_float("amount", 0.35f);
        }
        if (Material* m = this->shaders.create_material("Scanlines", "scanlines")) {
            m->set_float("time", 0.0f);
            m->set_float("density", 0.8f);
        }
    }

    void Engine::on_update_tick() {
        // Input is processed every tick so the editor stays responsive even
        // while paused; only the scene logic is gated behind the Play state.
        Engine::input.process_timer_event();
        if (!this->simulating)
            return;
        turbo::Scene* active_scene = this->scene_manager.get_active_scene();
        if (active_scene)
            active_scene->update(UPDATE_DELTA_MS);   // fixed 60 Hz logic step
    }
}