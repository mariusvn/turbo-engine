
#ifndef __TURBO_ENGINE_SCENEMANAGERDEBUG_HPP__
#define __TURBO_ENGINE_SCENEMANAGERDEBUG_HPP__

#include "./DebugWindow.hpp"
#include "../GameObject.hpp"
#include "../Event.hpp"
#include "../EventHandler.hpp"

namespace debug {
    class SceneManagerDebug: public DebugWindow {
    public:
        SceneManagerDebug();

        void render() override;
        void set_root_gameobject(turbo::GameObject* game_object);

        const char* scene_name = nullptr;
        turbo::GameObject* root_gameobject = nullptr;

    protected:

        void draw_child(turbo::GameObject* gm);
        bool initialized = false;

        const turbo::EventHandler<>* event_handler = nullptr;
    };
}

#endif
