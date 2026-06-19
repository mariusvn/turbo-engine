#include <turbo/script/LuaComponent.hpp>
#include <turbo/script/ScriptEngine.hpp>
#include <turbo/Engine.hpp>
#include <turbo/GameObject.hpp>

#include <iostream>
#include <utility>

namespace turbo {

    // sol-backed state, hidden from the header.
    struct LuaComponent::Script {
        sol::table instance;
        sol::protected_function fn_update;
        sol::protected_function fn_on_enable;
        sol::protected_function fn_on_disable;
        sol::protected_function fn_unload;
        std::vector<ScriptField> field_descs;   // names + types snapshotted at load
    };

    namespace {
        std::string stem_of(const std::string& path) {
            size_t slash = path.find_last_of("/\\");
            std::string file = (slash == std::string::npos) ? path : path.substr(slash + 1);
            size_t dot = file.find_last_of('.');
            return (dot == std::string::npos) ? file : file.substr(0, dot);
        }
    }

    LuaComponent::LuaComponent(GameObject* parent, std::string script_path)
        : Component(parent), path(std::move(script_path)) {
        this->display_name = stem_of(this->path);
        this->name = this->display_name.c_str();
    }

    LuaComponent::~LuaComponent() = default;

    void LuaComponent::load() {
        this->valid_ = false;
        this->impl = std::make_unique<Script>();

        script::ScriptEngine* engine = turbo::Engine::engine ? turbo::Engine::engine->scripts() : nullptr;
        if (!engine) {
            std::cout << "[lua][error] no script engine available" << std::endl;
            return;
        }
        sol::state& lua = engine->state();

        sol::load_result loaded = lua.load_file(this->path);
        if (!loaded.valid()) {
            sol::error err = loaded;
            std::cout << "[lua][error] " << this->display_name << ": " << err.what() << std::endl;
            return;
        }
        sol::protected_function_result ran = loaded();
        if (!ran.valid()) {
            sol::error err = ran;
            std::cout << "[lua][error] " << this->display_name << ": " << err.what() << std::endl;
            return;
        }
        sol::object ret = ran;
        if (ret.get_type() != sol::type::table) {
            std::cout << "[lua][error] " << this->display_name << " must 'return' a table" << std::endl;
            return;
        }

        this->impl->instance = ret.as<sol::table>();
        sol::table& self = this->impl->instance;

        // Snapshot the public fields (declared before the gameObject is injected).
        for (auto& kv : self) {
            if (kv.first.get_type() != sol::type::string) continue;
            const std::string key = kv.first.as<std::string>();
            const sol::type t = kv.second.get_type();
            ScriptField f;
            f.name = key;
            if (t == sol::type::number)       { f.type = ScriptField::Type::Number; f.number = kv.second.as<double>(); }
            else if (t == sol::type::boolean) { f.type = ScriptField::Type::Bool;   f.boolean = kv.second.as<bool>(); }
            else if (t == sol::type::string)  { f.type = ScriptField::Type::String; f.text = kv.second.as<std::string>(); }
            else continue;
            this->impl->field_descs.push_back(f);
        }

        self["gameObject"] = this->gameObject;
        this->impl->fn_update     = self["update"];
        this->impl->fn_on_enable  = self["on_enable"];
        this->impl->fn_on_disable = self["on_disable"];
        this->impl->fn_unload     = self["unload"];
        this->valid_ = true;

        sol::protected_function fn_load = self["load"];
        if (fn_load.valid()) {
            sol::protected_function_result r = fn_load(self);
            if (!r.valid()) {
                sol::error err = r;
                std::cout << "[lua][error] " << this->display_name << ".load: " << err.what() << std::endl;
            }
        }
    }

    void LuaComponent::update(int delta_time) {
        if (!this->valid_ || !this->impl->fn_update.valid()) return;
        sol::protected_function_result r = this->impl->fn_update(this->impl->instance, delta_time);
        if (!r.valid()) {
            sol::error err = r;
            std::cout << "[lua][error] " << this->display_name << ".update: " << err.what() << std::endl;
            // Stop calling the broken callback so the console isn't flooded.
            this->impl->fn_update = sol::protected_function();
        }
    }

    void LuaComponent::on_enable() {
        if (this->valid_ && this->impl->fn_on_enable.valid())
            this->impl->fn_on_enable(this->impl->instance);
    }

    void LuaComponent::on_disable() {
        if (this->valid_ && this->impl->fn_on_disable.valid())
            this->impl->fn_on_disable(this->impl->instance);
    }

    void LuaComponent::unload() {
        if (this->valid_ && this->impl->fn_unload.valid())
            this->impl->fn_unload(this->impl->instance);
    }

    void LuaComponent::reload() {
        this->unload();
        this->load();
    }

    std::vector<ScriptField> LuaComponent::fields() const {
        std::vector<ScriptField> out;
        if (!this->valid_ || !this->impl) return out;
        const sol::table& self = this->impl->instance;
        for (const ScriptField& desc : this->impl->field_descs) {
            ScriptField f = desc;
            sol::object o = self[desc.name];
            switch (desc.type) {
                case ScriptField::Type::Number: if (o.is<double>())      f.number = o.as<double>(); break;
                case ScriptField::Type::Bool:   if (o.is<bool>())        f.boolean = o.as<bool>(); break;
                case ScriptField::Type::String: if (o.is<std::string>()) f.text = o.as<std::string>(); break;
            }
            out.push_back(f);
        }
        return out;
    }

    void LuaComponent::set_number(const std::string& name, double value) {
        if (this->valid_ && this->impl) this->impl->instance[name] = value;
    }
    void LuaComponent::set_bool(const std::string& name, bool value) {
        if (this->valid_ && this->impl) this->impl->instance[name] = value;
    }
    void LuaComponent::set_string(const std::string& name, const std::string& value) {
        if (this->valid_ && this->impl) this->impl->instance[name] = value;
    }
}
