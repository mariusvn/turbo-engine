#include <turbo/graphics/Shader.hpp>
#include <turbo/graphics/Texture.hpp>
#include <fstream>
#include <sstream>

namespace turbo {
    namespace {
        // al_get_default_shader_source returns a pointer owned by Allegro (or
        // null on failure). Wrap it so we never construct a std::string from a
        // null pointer.
        std::string default_shader_source(ALLEGRO_SHADER_TYPE type) {
            const char* src = al_get_default_shader_source(ALLEGRO_SHADER_GLSL, type);
            return src ? std::string(src) : std::string();
        }
    }

    Shader::Shader(const Path& pixel_shader_path, const Path& vertex_shader_path) {
        this->shader = al_create_shader(ALLEGRO_SHADER_GLSL);
        this->pixel_shader_src = Shader::read_file(pixel_shader_path);
        this->vertex_shader_src = Shader::read_file(vertex_shader_path);
        al_attach_shader_source(this->shader, ALLEGRO_PIXEL_SHADER, this->pixel_shader_src.c_str());
        al_attach_shader_source(this->shader, ALLEGRO_VERTEX_SHADER, this->vertex_shader_src.c_str());
        this->build();
    }

    Shader::Shader() {
        this->shader = al_create_shader(ALLEGRO_SHADER_GLSL);
        this->pixel_shader_src = default_shader_source(ALLEGRO_PIXEL_SHADER);
        this->vertex_shader_src = default_shader_source(ALLEGRO_VERTEX_SHADER);
        al_attach_shader_source(this->shader, ALLEGRO_PIXEL_SHADER, this->pixel_shader_src.c_str());
        al_attach_shader_source(this->shader, ALLEGRO_VERTEX_SHADER, this->vertex_shader_src.c_str());
        this->build();
    }

    Shader::~Shader() {
        al_destroy_shader(this->shader);
    }

    Shader* Shader::from_source(const std::string& vertex_src, const std::string& pixel_src) {
        Shader* s = new Shader();   // starts from the default program
        s->vertex_shader_src = vertex_src;
        s->pixel_shader_src = pixel_src;
        al_attach_shader_source(s->shader, ALLEGRO_VERTEX_SHADER, s->vertex_shader_src.c_str());
        al_attach_shader_source(s->shader, ALLEGRO_PIXEL_SHADER, s->pixel_shader_src.c_str());
        s->build();
        return s;
    }

    bool Shader::build() {
        this->valid_ = al_build_shader(this->shader);
        const char* log = al_get_shader_log(this->shader);
        this->log_ = log ? log : "";
        return this->valid_;
    }

    bool Shader::use() const {
        return al_use_shader(this->shader);
    }

    bool Shader::set_float(const char* name, float v) { return al_set_shader_float(name, v); }
    bool Shader::set_int(const char* name, int v)     { return al_set_shader_int(name, v); }
    bool Shader::set_bool(const char* name, bool v)   { return al_set_shader_bool(name, v); }

    bool Shader::set_vec2(const char* name, float x, float y) {
        float v[2] = {x, y};
        return al_set_shader_float_vector(name, 2, v, 1);
    }
    bool Shader::set_vec3(const char* name, float x, float y, float z) {
        float v[3] = {x, y, z};
        return al_set_shader_float_vector(name, 3, v, 1);
    }
    bool Shader::set_vec4(const char* name, float x, float y, float z, float w) {
        float v[4] = {x, y, z, w};
        return al_set_shader_float_vector(name, 4, v, 1);
    }
    bool Shader::set_sampler(const char* name, Texture* texture, int unit) {
        if (!texture) return false;
        return al_set_shader_sampler(name, static_cast<ALLEGRO_BITMAP*>(texture->__get_texture_bitmap()), unit);
    }

    std::string Shader::read_file(const Path& path) {
        std::ostringstream sstream;
        std::ifstream fs((const char*)path);
        sstream << fs.rdbuf();
        return sstream.str();
    }

    void Shader::set_vertex_shader(const Path &vertex_shader_path) {
        this->vertex_shader_src = Shader::read_file(vertex_shader_path);
        al_attach_shader_source(this->shader, ALLEGRO_VERTEX_SHADER, this->vertex_shader_src.c_str());
        this->build();
    }

    void Shader::set_pixel_shader(const Path &pixel_shader_path) {
        this->pixel_shader_src = Shader::read_file(pixel_shader_path);
        al_attach_shader_source(this->shader, ALLEGRO_PIXEL_SHADER, this->pixel_shader_src.c_str());
        this->build();
    }

    void Shader::remove_vertex_shader() {
        this->vertex_shader_src = default_shader_source(ALLEGRO_VERTEX_SHADER);
        al_attach_shader_source(this->shader, ALLEGRO_VERTEX_SHADER, this->vertex_shader_src.c_str());
        this->build();
    }

    void Shader::remove_pixel_shader() {
        this->pixel_shader_src = default_shader_source(ALLEGRO_PIXEL_SHADER);
        al_attach_shader_source(this->shader, ALLEGRO_PIXEL_SHADER, this->pixel_shader_src.c_str());
        this->build();
    }

    void Shader::init_shaders()
    {
        Shader::current_shader = new Shader();
        al_use_shader(Shader::current_shader->shader);
    }

    void Shader::set_active_shader(Shader *shader) {
        Shader::current_shader = shader;
        al_use_shader(shader->shader);
    }

    Shader* Shader::get_active_shader() {
        return Shader::current_shader;
    }
}
