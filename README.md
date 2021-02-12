<h1 align="center">Turbo Engine</h1>
<h3 align="center">A C++ game engine using Allegro 5</h3>

----

<div align="center">
<a href="https://github.com/mariusvn/turbo-engine/releases">Releases</a>
-
<a href="https://mariusvn.github.io/turbo-engine/">Documentation</a>
</div>

----

#### Basic program using Turbo Engine

Here is the most basic usage of Turbo Engine:

```cpp
#include <turbo/Engine.hpp>

int main()
{
    turbo::Engine engine = turbo::Engine();
    turbo::Scene scene = turbo::Scene();
    engine.scene_manager.register_scene(&scene, "Hello");
    engine.scene_manager.set_active_scene("Hello");
    engine.start_window("Hello World", 800, 600);
    engine.loop();
    engine.stop_window();
    return 0;
}
```

#### Basic CMAKE to compile it

```cmake
cmake_minimum_required(VERSION 3.17)
project(turbo_demo)

set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED TURBO_PATH)
    set(TURBO_PATH ../libs/TurboEngine/0.6)
endif()

file(GLOB SRC src/*.cpp src/**/*.cpp)


add_executable(turbo_demo ${SRC})


target_include_directories(turbo_demo PUBLIC include)
target_include_directories(turbo_demo PUBLIC ${TURBO_PATH}/include)
target_link_directories(turbo_demo PUBLIC ${TURBO_PATH}/lib)
target_link_libraries(turbo_demo turbo_engine)
```

The static version contains the allegro library. If you are using the shared library you still have to install the allegro library