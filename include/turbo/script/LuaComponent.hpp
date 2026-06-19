
#ifndef __TURBO_SCRIPT_LUACOMPONENT_HPP__
#define __TURBO_SCRIPT_LUACOMPONENT_HPP__

#include "../Component.hpp"
#include <string>
#include <vector>
#include <memory>

namespace turbo {

    /**
     * @brief A public, inspector-editable field exposed by a Lua script
     * (any number / boolean / string declared on the script table before load()).
     */
    struct ScriptField {
        enum class Type { Number, Bool, String };
        std::string name;
        Type type = Type::Number;
        double number = 0.0;
        bool boolean = false;
        std::string text;
    };

    /**
     * @brief A component whose behaviour is defined by a Lua script, à la Unity.
     *
     * The script is a file returning a table with optional lifecycle methods
     * (load, update, on_enable, on_disable, unload). `self.gameObject` is the
     * owning GameObject. The header keeps sol2/Lua hidden behind a PIMPL so the
     * rest of the engine and the editor can use LuaComponent without pulling in
     * the Lua headers.
     */
    class LuaComponent : public Component {
    public:
        LuaComponent(GameObject* parent, std::string script_path);
        ~LuaComponent();

        void load() override;
        void update(int delta_time) override;
        void on_enable() override;
        void on_disable() override;
        void unload() override;

        /** @brief Did the script load and return a table without error? */
        bool valid() const { return this->valid_; }

        const std::string& script_path() const { return this->path; }
        const std::string& script_name() const { return this->display_name; }

        /** @brief Re-read the script file from disk and re-run it. */
        void reload();

        /** @brief Public fields (with their current values) for the inspector. */
        std::vector<ScriptField> fields() const;
        void set_number(const std::string& name, double value);
        void set_bool(const std::string& name, bool value);
        void set_string(const std::string& name, const std::string& value);

    private:
        struct Script;                 // sol-backed, defined in the .cpp
        std::unique_ptr<Script> impl;
        std::string path;
        std::string display_name;
        bool valid_ = false;
    };
}

#endif
