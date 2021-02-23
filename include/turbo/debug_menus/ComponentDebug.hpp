
#ifndef __TURBO_ENGINE_COMPONENTDEBUG_HPP__
#define __TURBO_ENGINE_COMPONENTDEBUG_HPP__

#include "DebugWindow.hpp"
#include <vector>
#include <turbo/EventHandler.hpp>
#include "../Component.hpp"
#include "Inspector.hpp"

namespace debug {
    class ComponentDebug: public DebugWindow {
    public:
        explicit ComponentDebug(turbo::Component* component);

        void render() override;

    private:

        struct func_holder {
            inspector_types type;
            void (*func)(debug::InspectorObserver& obs);
        };

        static void render(debug::InspectorObserver& obs);
        static void render_float(debug::InspectorObserver& obs);
        static void render_int(debug::InspectorObserver& obs);
        static void render_cstring(debug::InspectorObserver& obs);
        static void render_int_vector(debug::InspectorObserver& obs);
        static void render_float_vector(debug::InspectorObserver& obs);
        static void render_multiple_float(debug::InspectorObserver& obs);
        static void render_multiple_int(debug::InspectorObserver& obs);
        static void render_float_range(debug::InspectorObserver& obs);
        static void render_int_range(debug::InspectorObserver& obs);
        static void render_ui_text(debug::InspectorObserver& obs);
        static void render_ui_separator(debug::InspectorObserver& obs);

        inline static const ComponentDebug::func_holder renderers[] {
            {FLOAT, &render_float},
            {INT, &render_int},
            {CSTRING, &render_cstring},
            {INT_VECTOR, &render_int_vector},
            {FLOAT_VECTOR, &render_float_vector},
            {FLOAT2, &render_multiple_float},
            {FLOAT3, &render_multiple_float},
            {FLOAT4, &render_multiple_float},
            {INT2, &render_multiple_int},
            {INT3, &render_multiple_int},
            {INT4, &render_multiple_int},
            {FLOAT_RANGE, &render_float_range},
            {INT_RANGE, &render_int_range},
            {UI_TEXT, &render_ui_text},
            {UI_SEPARATOR, &render_ui_separator},
            {FLOAT, nullptr}
        };

        inline static std::vector<turbo::Component*> opened_wins{};

        turbo::Component* component = nullptr;
        std::vector<debug::InspectorObserver*>* observers = nullptr;
        turbo::EventHandler<>* eventHandler = new turbo::EventHandler<>([this]{this->render();});
    };
}


#endif
