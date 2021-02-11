#include <turbo/graphics/Shader.hpp>
#include <fstream>
#include <sstream>

namespace turbo {
    Shader::Shader(const Path& pixel_shader_path, const Path& vertex_shader_path) {
        this->shader = al_create_shader(ALLEGRO_SHADER_GLSL);
        this->pixel_shader_src = Shader::read_file(pixel_shader_path);
        this->vertex_shader_src = Shader::read_file(vertex_shader_path);
        al_attach_shader_source(this->shader, ALLEGRO_PIXEL_SHADER, this->pixel_shader_src);
        al_attach_shader_source(this->shader, ALLEGRO_VERTEX_SHADER, this->vertex_shader_src);
        al_build_shader(this->shader);
    }

    Shader::Shader() {
        this->shader = al_create_shader(ALLEGRO_SHADER_GLSL);
        this->pixel_shader_src = al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_PIXEL_SHADER);
        this->vertex_shader_src = al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_VERTEX_SHADER);
        al_attach_shader_source(this->shader, ALLEGRO_PIXEL_SHADER, this->pixel_shader_src);
        al_attach_shader_source(this->shader, ALLEGRO_VERTEX_SHADER, this->vertex_shader_src);
        al_build_shader(this->shader);
    }

    Shader::~Shader() {
        al_destroy_shader(this->shader);
    }

    const char * Shader::read_file(const Path& path) {
        std::ostringstream sstream;
        std::ifstream fs((const char*)path);
        sstream << fs.rdbuf();
        const std::string str(sstream.str());
        const char* ptr = str.c_str();
        return ptr;
    }

    void Shader::set_vertex_shader(const Path &vertex_shader_path) {
        this->vertex_shader_src = Shader::read_file(vertex_shader_path);
        al_attach_shader_source(this->shader, ALLEGRO_VERTEX_SHADER, this->vertex_shader_src);
        al_build_shader(this->shader);
    }

    void Shader::set_pixel_shader(const Path &pixel_shader_path) {
        this->pixel_shader_src = Shader::read_file(pixel_shader_path);
        al_attach_shader_source(this->shader, ALLEGRO_PIXEL_SHADER, this->pixel_shader_src);
        al_build_shader(this->shader);
    }

    void Shader::remove_vertex_shader() {
        this->vertex_shader_src = al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_VERTEX_SHADER);
        al_attach_shader_source(this->shader, ALLEGRO_VERTEX_SHADER, this->vertex_shader_src);
        al_build_shader(this->shader);
    }

    void Shader::remove_pixel_shader() {
        this->pixel_shader_src = al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_PIXEL_SHADER);
        al_attach_shader_source(this->shader, ALLEGRO_PIXEL_SHADER, this->pixel_shader_src);
        al_build_shader(this->shader);

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