# ===========================================================================
#  turbo_engine
#
#  Self-contained build: dependencies (Allegro 5 + Dear ImGui and ALL of their
#  transitive deps) are fetched automatically through vcpkg in manifest mode.
#  No manual library download or path configuration is required.
#
#  Quick start:
#      cmake -B build
#      cmake --build build
#
#  Dear ImGui debug overlay is OFF by default. Enable it explicitly:
#      cmake -B build -DTURBO_ENABLE_IMGUI=ON
# ===========================================================================
cmake_minimum_required(VERSION 3.21)

# ---------------------------------------------------------------------------
#  Dependency bootstrap
#
#  Unless a toolchain is explicitly supplied, we fetch and bootstrap a pinned
#  vcpkg into the build tree, then point CMake at its toolchain. Pinning the
#  release tag here AND the matching commit in vcpkg.json ("builtin-baseline")
#  keeps dependency resolution fully deterministic and self-contained -- we do
#  NOT depend on any vcpkg that happens to be installed on the machine.
#  This MUST run before the first project() call.
# ---------------------------------------------------------------------------
set(VCPKG_TAG "2024.12.16" CACHE STRING "vcpkg release tag used for bootstrap")

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "turbo_engine: bootstrapping vcpkg (${VCPKG_TAG})")
    include(FetchContent)
    FetchContent_Declare(
        vcpkg
        GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
        GIT_TAG        ${VCPKG_TAG}
        GIT_SHALLOW    TRUE
        # vcpkg has no top-level CMakeLists; never try to add_subdirectory it.
        SOURCE_SUBDIR  cmake-does-not-exist
    )
    FetchContent_MakeAvailable(vcpkg)
    set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake"
        CACHE FILEPATH "Vcpkg toolchain file" FORCE)
endif()

# Dear ImGui debug overlay is optional; selecting it pulls the matching feature
# from vcpkg.json. Must be set before project() so manifest install sees it.
option(TURBO_ENABLE_IMGUI "Build with the Dear ImGui debug overlay" OFF)
if(TURBO_ENABLE_IMGUI)
    list(APPEND VCPKG_MANIFEST_FEATURES "imgui")
endif()

project(turbo_engine VERSION 0.2.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
#  Global settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default single-config generators to Release.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Put every runtime artifact (engine DLL, demo exe, copied dependency DLLs) in
# one directory so the demo can locate everything it links against.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ---------------------------------------------------------------------------
#  Dependencies (resolved by vcpkg)
# ---------------------------------------------------------------------------
find_package(Allegro CONFIG REQUIRED)
if(TURBO_ENABLE_IMGUI)
    find_package(imgui CONFIG REQUIRED)
endif()

# ---------------------------------------------------------------------------
#  turbo_engine library
# ---------------------------------------------------------------------------
set(TURBO_SOURCES
    src/Component.cpp
    src/Engine.cpp
    src/Font.cpp
    src/GameObject.cpp
    src/Input.cpp
    src/Path.cpp
    src/Scene.cpp
    src/SceneManager.cpp
    src/Transform.cpp
    src/components/SimpleControl.cpp
    src/debug_menus/ComponentDebug.cpp
    src/debug_menus/DebugWindow.cpp
    src/debug_menus/EngineDebug.cpp
    src/debug_menus/GameObjectDebug.cpp
    src/debug_menus/SceneManagerDebug.cpp
    src/graphics/CircleShape.cpp
    src/graphics/RectangleShape.cpp
    src/graphics/Shader.cpp
    src/graphics/Sprite.cpp
    src/graphics/Text.cpp
    src/graphics/Texture.cpp
    src/graphics/Tileset.cpp
    src/sound/MusicStream.cpp
    src/sound/Sound.cpp
)

# Built as a STATIC library: the headers expose no dllexport annotations, and a
# static archive sidesteps Windows symbol-visibility and "magic main" pitfalls,
# linking cleanly into the demo (and any consumer) on MSVC/GCC/Clang alike.
add_library(turbo_engine STATIC ${TURBO_SOURCES})
add_library(turbo::engine ALIAS turbo_engine)

target_include_directories(turbo_engine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(turbo_engine PUBLIC
    Allegro::allegro
    Allegro::allegro_font
    Allegro::allegro_ttf
    Allegro::allegro_image
    Allegro::allegro_primitives
    Allegro::allegro_audio
    Allegro::allegro_acodec
)

# ---------------------------------------------------------------------------
#  Dear ImGui debug overlay (optional)
# ---------------------------------------------------------------------------
if(TURBO_ENABLE_IMGUI)
    target_compile_definitions(turbo_engine PUBLIC __TURBO_USE_IMGUI__)
    target_link_libraries(turbo_engine PUBLIC imgui::imgui)

    # The sources include <imgui/imgui.h> and <imgui/imgui_impl_allegro5.h>,
    # but vcpkg installs those headers flat (imgui.h, imgui_impl_allegro5.h).
    # Generate a tiny forwarding shim so the existing include paths resolve
    # without modifying any source file.
    set(_imgui_shim "${CMAKE_BINARY_DIR}/imgui_compat")
    file(WRITE "${_imgui_shim}/imgui/imgui.h"
        "#pragma once\n#include <imgui.h>\n")
    file(WRITE "${_imgui_shim}/imgui/imgui_impl_allegro5.h"
        "#pragma once\n#include <imgui_impl_allegro5.h>\n")
    target_include_directories(turbo_engine PUBLIC "${_imgui_shim}")
endif()

# ---------------------------------------------------------------------------
#  Demo executable
# ---------------------------------------------------------------------------
add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE turbo_engine)

# Make the demo self-contained: deploy the runtime assets next to the
# executable so it resolves them no matter where it is launched from.
add_custom_command(TARGET demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:demo>/assets"
    COMMENT "Deploying assets next to demo")
