
#ifndef __TURBO_SHADER_HPP__
#define __TURBO_SHADER_HPP__

#include <allegro5/allegro5.h>
#include "../Path.hpp"

namespace turbo {
    class Shader {
    public:
        Shader(const Path& pixel_shader_path, const Path& vertex_shader_path);
        Shader();
        ~Shader();

        void set_vertex_shader(const Path& vertex_shader_path);
        void set_pixel_shader(const Path& pixel_shader_path);
        void remove_vertex_shader();
        void remove_pixel_shader();

        static void init_shaders();
        static void set_active_shader(Shader* shader);
        static Shader* get_active_shader();

    protected:
        ALLEGRO_SHADER* shader = nullptr;
        const char* pixel_shader_src = nullptr;
        const char* vertex_shader_src = nullptr;

        static const char* read_file(const Path& path);
        static inline Shader* current_shader = nullptr;
    };
}

#endif
