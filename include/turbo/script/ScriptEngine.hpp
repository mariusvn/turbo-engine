
#ifndef __TURBO_SCRIPT_SCRIPTENGINE_HPP__
#define __TURBO_SCRIPT_SCRIPTENGINE_HPP__

// This header pulls in sol2/Lua, so it must only be included by the scripting
// translation units (ScriptEngine.cpp / LuaComponent.cpp). The rest of the
// engine sees ScriptEngine only through a forward declaration.
#include <sol/sol.hpp>
#include <functional>
#include <string>
#include <vector>

namespace turbo::script {

    /**
     * @brief Owns the Lua VM and exposes the engine API to scripts.
     *
     * A single Lua state is shared by every LuaComponent; each component runs
     * its script in its own returned table, so instances don't collide.
     */
    class ScriptEngine {
    public:
        ScriptEngine();

        ScriptEngine(const ScriptEngine&) = delete;
        ScriptEngine& operator=(const ScriptEngine&) = delete;

        /** @brief The shared Lua state (used by LuaComponent to load scripts). */
        sol::state& state() { return this->lua; }

        /**
         * @brief Queue a structural change (spawn/destroy) to run after the
         * current update pass, so the scene tree isn't mutated while iterated.
         */
        void defer(std::function<void()> fn) { this->deferred.push_back(std::move(fn)); }

        /** @brief Execute and clear all queued deferred commands. */
        void run_deferred();

        /**
         * @brief Execute a chunk of Lua (the editor REPL). Errors and non-nil
         * results are printed to the console.
         */
        void run_string(const std::string& code);

    private:
        void bind_api();

        sol::state lua;
        std::vector<std::function<void()>> deferred;
    };
}

#endif
