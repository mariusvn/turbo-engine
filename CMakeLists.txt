cmake_minimum_required(VERSION 3.8)
project(turbo_engine)
set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED CMAKE_BINARY_DIR)
    set(CMAKE_BINARY_DIR "./out/build/standard")
endif()

if (NOT DEFINED MSVC_LIB_PATH)
    set(MSVC_LIB_PATH "../libs/Allegro5-msvc")
endif()

if (NOT DEFINED MING_LIB_PATH)
    set(MING_LIB_PATH "../libs/Allegro5")
endif()

if (NOT DEFINED MING_STATIC_LINK_PATH)
    set(MING_STATIC_LINK_PATH "../libs/Allegro5-static")
endif()


FILE(GLOB src src/*.cpp src/**/*.cpp)
set(req_libs allegro allegro_ttf allegro_font allegro_image allegro_primitives)
set(req_libs_static allegro-static allegro_ttf-static allegro_font-static allegro_image-static allegro_primitives-static)

include_directories(./include)

add_library(turbo_engine_static STATIC ${src})
add_library(turbo_engine SHARED ${src})

string(REPLACE ";" "\n" display_src "${src}")
message("\nSources:\n${display_src}\n")

if (MSVC)
    include_directories(${MSVC_LIB_PATH}/include)
    link_directories(${MSVC_LIB_PATH}/lib)
elseif(WIN32)
    target_include_directories(turbo_engine PUBLIC ${MING_LIB_PATH}/include)
    target_link_directories(turbo_engine PUBLIC ${MING_LIB_PATH}/lib)
    target_include_directories(turbo_engine_static PUBLIC ${MING_STATIC_LINK_PATH}/include)
    target_link_directories(turbo_engine_static PUBLIC ${MING_STATIC_LINK_PATH}/lib)
endif()

add_executable(demo main.cpp)

if (MSVC)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/${MSVC_LIB_PATH}/bin" src_path)
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
elseif(WIN32)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/${MING_LIB_PATH}/bin" src_path)
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

    add_custom_target(combined_turbo_allegro
            COMMAND ar -x $<TARGET_FILE:turbo_engine_static>
            COMMAND ar -qcs ./libturbo_engine-static-merged.a *.obj
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS turbo_engine_static)

    foreach(file_lib ${req_libs_static})
        add_custom_command(TARGET combined_turbo_allegro PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/${MING_STATIC_LINK_PATH}/lib/lib${file_lib}.a" "${CMAKE_BINARY_DIR}/lib${file_lib}.a"
                COMMAND ar -x lib${file_lib}.a
                COMMENT "Unbuild lib${file_lib}.a ...")
        add_custom_command(TARGET combined_turbo_allegro POST_BUILD
                COMMAND del lib${file_lib}.a
                COMMENT "Deleting lib${file_lib}.a ...")
    endforeach()
    add_custom_command(TARGET combined_turbo_allegro POST_BUILD
            COMMAND del *.obj
            COMMENT "Deleting OBJ files ...")
endif()

target_link_libraries(turbo_engine ${req_libs})
target_link_libraries(turbo_engine_static PUBLIC ${req_libs_static})

add_library(c STATIC IMPORTED GLOBAL)
add_dependencies(c combined)

set_target_properties(c
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libturbo_engine-static-merged.a
        )

target_link_libraries(demo turbo_engine)