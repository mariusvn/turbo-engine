#include <turbo/graphics/Material.hpp>
#include <turbo/graphics/Shader.hpp>

namespace turbo {
    bool Material::bind() const {
        if (!this->m_shader) return false;
        if (!this->m_shader->use()) return false;
        for (const auto& entry : this->m_uniforms) {
            const std::string& name = entry.first;
            const UniformValue& u = entry.second;
            switch (u.type) {
                case UniformValue::Type::Float:   this->m_shader->set_float(name.c_str(), u.f[0]); break;
                case UniformValue::Type::Int:     this->m_shader->set_int(name.c_str(), u.i); break;
                case UniformValue::Type::Bool:    this->m_shader->set_bool(name.c_str(), u.b); break;
                case UniformValue::Type::Vec2:    this->m_shader->set_vec2(name.c_str(), u.f[0], u.f[1]); break;
                case UniformValue::Type::Vec3:    this->m_shader->set_vec3(name.c_str(), u.f[0], u.f[1], u.f[2]); break;
                case UniformValue::Type::Vec4:    this->m_shader->set_vec4(name.c_str(), u.f[0], u.f[1], u.f[2], u.f[3]); break;
                case UniformValue::Type::Sampler: this->m_shader->set_sampler(name.c_str(), u.sampler, u.sampler_unit); break;
            }
        }
        return true;
    }
}
