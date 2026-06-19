
#ifndef __TURBO_SHADERLIBRARY_HPP__
#define __TURBO_SHADERLIBRARY_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Material.hpp"

namespace turbo {
    class Shader;
    class Path;

    /**
     * @brief Owns named shader programs and materials.
     *
     * This is the registry the engine/editor talk to instead of juggling raw
     * Shader pointers. Shaders are owned here (unique_ptr); materials reference
     * them. Loading reports compile/link failures (and returns null) so a
     * broken shader never silently produces a blank screen.
     */
    class ShaderLibrary {
    public:
        ShaderLibrary() = default;

        /** @brief Compile GLSL source and register it. Null + logged on failure. */
        Shader* load_source(const std::string& name,
                            const std::string& vertex_src,
                            const std::string& pixel_src);

        /** @brief Compile GLSL from files and register it. Null + logged on failure. */
        Shader* load_files(const std::string& name,
                           const Path& vertex_path,
                           const Path& pixel_path);

        /** @brief Register an already-built shader (takes ownership). */
        Shader* add(const std::string& name, Shader* shader);

        Shader* get(const std::string& name) const;
        bool has(const std::string& name) const;

        /** @brief The default Allegro program, created on first use. Always valid. */
        Shader* default_shader();

        /** @brief Create a material bound to a registered shader (null if missing). */
        Material* create_material(const std::string& name, const std::string& shader_name);
        Material* material(const std::string& name) const;

        std::vector<std::string> shader_names() const;
        std::vector<std::string> material_names() const;

    private:
        std::map<std::string, std::unique_ptr<Shader>> m_shaders;
        std::map<std::string, std::unique_ptr<Material>> m_materials;
    };
}

#endif
