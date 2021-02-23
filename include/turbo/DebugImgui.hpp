

#ifndef __TURBO_ENGINE_DEBUGIMGUI_HPP__
#define __TURBO_ENGINE_DEBUGIMGUI_HPP__

#ifdef __TURBO_USE_IMGUI__

    #include <imgui/imgui.h>
    #include <imgui/imgui_impl_allegro5.h>
    #include "debug_menus/EngineDebug.hpp"
    #include "debug_menus/SceneManagerDebug.hpp"

    #define ONLYIMGUI(expr) expr;

#else
    #define ONLYIMGUI(expr)
#endif

#endif
