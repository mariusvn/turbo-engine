cmake_minimum_required(VERSION 3.8)
project(turbo_engine)
set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED CMAKE_BINARY_DIR)
    set(CMAKE_BINARY_DIR "./out/build/standard")
endif()


FILE(GLOB src src/*.cpp src/**/*.cpp)
set(req_libs allegro allegro_ttf allegro_font allegro_image allegro_primitives)

if (MSVC)
    include_directories(../libs/Allegro5-msvc/include)
    link_directories(../libs/Allegro5-msvc/lib)
else()
    include_directories(../libs/Allegro5/include)
    link_directories(../libs/Allegro5/lib)
endif()

include_directories(./include)

add_library(turbo_engine SHARED ${src})
add_executable(demo main.cpp)

if (MSVC)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/../libs/Allegro5-msvc/bin" src_path)
    file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" target_path)
    message( "Setting up post-build commands")
    foreach(file_lib ${req_libs})
        message( "${src_path}\\${file_lib}-5.2.dll to ${target_path}\\${file_lib}-5.2.dll")
        add_custom_command(TARGET turbo_engine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${src_path}\\${file_lib}-5.2.dll"
            "${CMAKE_BINARY_DIR}\\${file_lib}-5.2.dll"
                COMMENT "Copy ${src_path}/${file_lib}-5.2.dll to ${target_path}/${file_lib}-5.2.dll")
    endforeach(file_lib)
    message( "Post-build commands are setup")
else()
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/../libs/Allegro5/bin" src_path)
    file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" target_path)
    message( "Setting up post-build commands")
    foreach(file_lib ${req_libs})
        message( "${src_path}\\${file_lib}-5.2.dll to ${target_path}\\${file_lib}-5.2.dll")
        add_custom_command(TARGET turbo_engine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${src_path}\\${file_lib}-5.2.dll"
            "${CMAKE_BINARY_DIR}\\${file_lib}-5.2.dll"
                COMMENT "Copy ${src_path}/${file_lib}-5.2.dll to ${target_path}/${file_lib}-5.2.dll")
    endforeach(file_lib)
    message( "Post-build commands are setup")
endif()

target_link_libraries(turbo_engine ${req_libs})
target_link_libraries(demo turbo_engine)