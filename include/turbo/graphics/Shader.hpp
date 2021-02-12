
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

        /**
         * Load the specified GLSL vertex shader file
         * @param vertex_shader_path path to the shader file
         */
        void set_vertex_shader(const Path& vertex_shader_path);

        /**
         * Set the specified GLSL pixel shader file
         * @param pixel_shader_path path to the shader file
         */
        void set_pixel_shader(const Path& pixel_shader_path);

        /**
         * Reset the vertex shader to the default shader
         */
        void remove_vertex_shader();

        /**
         * Reset the pixel shader to the default shader
         */
        void remove_pixel_shader();

        /** already initialized internally, do not call this */
        static void init_shaders();

        /**
         * Set the active shader
         * @param shader target shader
         */
        static void set_active_shader(Shader* shader);

        /**
         * Retrieve the current shader
         * @return
         */
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
