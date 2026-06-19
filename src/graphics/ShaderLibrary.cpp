#include <turbo/graphics/ShaderLibrary.hpp>
#include <turbo/graphics/Shader.hpp>
#include <turbo/Path.hpp>
#include <iostream>

namespace turbo {
    namespace {
        void report(const std::string& name, Shader* s) {
            if (s->valid()) {
                std::cout << "[ShaderLibrary][info] Shader '" << name << "' compiled" << std::endl;
            } else {
                std::cout << "[ShaderLibrary][error] Shader '" << name << "' failed to build";
                if (!s->log().empty()) std::cout << ": " << s->log();
                std::cout << std::endl;
            }
        }
    }

    Shader* ShaderLibrary::load_source(const std::string& name,
                                       const std::string& vertex_src,
                                       const std::string& pixel_src) {
        Shader* s = Shader::from_source(vertex_src, pixel_src);
        report(name, s);
        if (!s->valid()) {
            delete s;
            return nullptr;
        }
        return this->add(name, s);
    }

    Shader* ShaderLibrary::load_files(const std::string& name,
                                      const Path& vertex_path,
                                      const Path& pixel_path) {
        // Shader's file constructor takes (pixel, vertex).
        Shader* s = new Shader(pixel_path, vertex_path);
        report(name, s);
        if (!s->valid()) {
            delete s;
            return nullptr;
        }
        return this->add(name, s);
    }

    Shader* ShaderLibrary::add(const std::string& name, Shader* shader) {
        this->m_shaders[name] = std::unique_ptr<Shader>(shader);
        return shader;
    }

    Shader* ShaderLibrary::get(const std::string& name) const {
        auto it = this->m_shaders.find(name);
        return it != this->m_shaders.end() ? it->second.get() : nullptr;
    }

    bool ShaderLibrary::has(const std::string& name) const {
        return this->m_shaders.find(name) != this->m_shaders.end();
    }

    Shader* ShaderLibrary::default_shader() {
        if (!this->has("__default__"))
            this->add("__default__", new Shader());
        return this->get("__default__");
    }

    Material* ShaderLibrary::create_material(const std::string& name, const std::string& shader_name) {
        Shader* s = this->get(shader_name);
        if (!s) {
            std::cout << "[ShaderLibrary][error] Material '" << name
                      << "' references unknown shader '" << shader_name << "'" << std::endl;
            return nullptr;
        }
        Material* m = new Material(s);
        m->set_name(name);
        this->m_materials[name] = std::unique_ptr<Material>(m);
        return m;
    }

    Material* ShaderLibrary::material(const std::string& name) const {
        auto it = this->m_materials.find(name);
        return it != this->m_materials.end() ? it->second.get() : nullptr;
    }

    std::vector<std::string> ShaderLibrary::shader_names() const {
        std::vector<std::string> names;
        names.reserve(this->m_shaders.size());
        for (const auto& entry : this->m_shaders) names.push_back(entry.first);
        return names;
    }

    std::vector<std::string> ShaderLibrary::material_names() const {
        std::vector<std::string> names;
        names.reserve(this->m_materials.size());
        for (const auto& entry : this->m_materials) names.push_back(entry.first);
        return names;
    }
}
