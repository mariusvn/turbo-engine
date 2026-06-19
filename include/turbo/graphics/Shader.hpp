
#ifndef __TURBO_SHADER_HPP__
#define __TURBO_SHADER_HPP__

#include <allegro5/allegro5.h>
#include <string>
#include "../Path.hpp"

namespace turbo {
    class Texture;

    class Shader {
    public:
        Shader(const Path& pixel_shader_path, const Path& vertex_shader_path);
        Shader();
        ~Shader();

        /**
         * @brief Build a shader directly from GLSL source strings.
         * @return a shader whose valid()/log() report the compile/link result.
         */
        static Shader* from_source(const std::string& vertex_src, const std::string& pixel_src);

        /** @brief Whether the last build (compile + link) succeeded. */
        bool valid() const { return this->valid_; }

        /** @brief Driver log of the last build (empty on success on most drivers). */
        const std::string& log() const { return this->log_; }

        /** @brief Bind this shader as the active one on the current display. */
        bool use() const;

        /** @brief Underlying Allegro shader handle. */
        ALLEGRO_SHADER* native() const { return this->shader; }

        // Uniform setters. Allegro applies them to the *currently bound* shader,
        // so call use() (or Material::bind()) first.
        bool set_float(const char* name, float v);
        bool set_int(const char* name, int v);
        bool set_bool(const char* name, bool v);
        bool set_vec2(const char* name, float x, float y);
        bool set_vec3(const char* name, float x, float y, float z);
        bool set_vec4(const char* name, float x, float y, float z, float w);
        bool set_sampler(const char* name, Texture* texture, int unit);

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
        std::string pixel_shader_src;
        std::string vertex_shader_src;
        bool valid_ = false;
        std::string log_;

        /** @brief (Re)link the shader and capture valid()/log(). */
        bool build();

        static std::string read_file(const Path& path);
        static inline Shader* current_shader = nullptr;
    };
}

#endif
