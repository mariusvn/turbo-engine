
#ifndef __TURBO_ENGINE_GAMEOBJECTDEBUG_HPP__
#define __TURBO_ENGINE_GAMEOBJECTDEBUG_HPP__

#include "./DebugWindow.hpp"
#include "../GameObject.hpp"
#include "../Engine.hpp"

namespace debug {
    class GameObjectDebug: public DebugWindow {
    public:
        explicit GameObjectDebug(turbo::GameObject* gameobject);

        void render() override;

    protected:
        turbo::GameObject* gameobject;
        turbo::EventHandler<>* eventHandler = new turbo::EventHandler<>([this]{this->render();});
        std::string win_name = "";
        static inline std::vector<std::string> current_instances;
    };
}

#endif
