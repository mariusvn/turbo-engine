
#ifndef __TURBO_MATERIAL_HPP__
#define __TURBO_MATERIAL_HPP__

#include <string>
#include <map>
#include "Uniform.hpp"

namespace turbo {
    class Shader;
    class Texture;

    /**
     * @brief A shader program plus a named set of uniform values.
     *
     * A Material does not own its Shader (shaders are shared and owned by the
     * ShaderLibrary). Uniform values are stored and only pushed to the GPU on
     * bind(), which is the seam an inspector UI will later edit.
     */
    class Material {
    public:
        explicit Material(Shader* shader): m_shader(shader) {}

        void set_float(const std::string& name, float v)                       { m_uniforms[name] = UniformValue::Float(v); }
        void set_int(const std::string& name, int v)                           { m_uniforms[name] = UniformValue::Int(v); }
        void set_bool(const std::string& name, bool v)                         { m_uniforms[name] = UniformValue::Bool(v); }
        void set_vec2(const std::string& name, float x, float y)               { m_uniforms[name] = UniformValue::Vec2(x, y); }
        void set_vec3(const std::string& name, float x, float y, float z)      { m_uniforms[name] = UniformValue::Vec3(x, y, z); }
        void set_vec4(const std::string& name, float x, float y, float z, float w) { m_uniforms[name] = UniformValue::Vec4(x, y, z, w); }
        void set_sampler(const std::string& name, Texture* t, int unit)        { m_uniforms[name] = UniformValue::Sampler(t, unit); }

        /** @brief Bind the shader and push every stored uniform. Call before drawing. */
        bool bind() const;

        /**
         * @brief Refresh the "time" uniform (only if the material declares one)
         * then bind. Lets animated shaders advance without pushing a uniform to
         * shaders that don't use it.
         */
        bool bind_at(float time) {
            auto it = this->m_uniforms.find("time");
            if (it != this->m_uniforms.end())
                it->second = UniformValue::Float(time);
            return this->bind();
        }

        Shader* shader() const { return this->m_shader; }
        const std::map<std::string, UniformValue>& uniforms() const { return this->m_uniforms; }
        std::map<std::string, UniformValue>& uniforms() { return this->m_uniforms; }

        const std::string& name() const { return this->m_name; }
        void set_name(const std::string& name) { this->m_name = name; }

    private:
        Shader* m_shader = nullptr;   // non-owning (owned by the ShaderLibrary)
        std::map<std::string, UniformValue> m_uniforms;
        std::string m_name;
    };
}

#endif
