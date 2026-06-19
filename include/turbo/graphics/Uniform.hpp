
#ifndef __TURBO_UNIFORM_HPP__
#define __TURBO_UNIFORM_HPP__

namespace turbo {
    class Texture;

    /**
     * @brief A single, typed shader uniform value.
     *
     * Materials store these so uniform values can be set at any time (and later
     * edited from an inspector UI) and pushed to the GPU only at bind time.
     */
    struct UniformValue {
        enum class Type { Float, Int, Bool, Vec2, Vec3, Vec4, Sampler };

        Type type = Type::Float;
        float f[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        int i = 0;
        bool b = false;
        Texture* sampler = nullptr;
        int sampler_unit = 0;

        static UniformValue Float(float v)                       { UniformValue u; u.type = Type::Float; u.f[0] = v; return u; }
        static UniformValue Int(int v)                           { UniformValue u; u.type = Type::Int; u.i = v; return u; }
        static UniformValue Bool(bool v)                         { UniformValue u; u.type = Type::Bool; u.b = v; return u; }
        static UniformValue Vec2(float x, float y)               { UniformValue u; u.type = Type::Vec2; u.f[0] = x; u.f[1] = y; return u; }
        static UniformValue Vec3(float x, float y, float z)      { UniformValue u; u.type = Type::Vec3; u.f[0] = x; u.f[1] = y; u.f[2] = z; return u; }
        static UniformValue Vec4(float x, float y, float z, float w) { UniformValue u; u.type = Type::Vec4; u.f[0] = x; u.f[1] = y; u.f[2] = z; u.f[3] = w; return u; }
        static UniformValue Sampler(Texture* t, int unit)        { UniformValue u; u.type = Type::Sampler; u.sampler = t; u.sampler_unit = unit; return u; }
    };
}

#endif
