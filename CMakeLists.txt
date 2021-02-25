cmake_minimum_required(VERSION 3.0)
project(turbo_engine)
set(CMAKE_CXX_STANDARD 17)


### VARIABLES ###

# Build path
if (NOT DEFINED CMAKE_BINARY_DIR)
    set(CMAKE_BINARY_DIR "./out/build/standard")
endif()

# Allegro library path
if (NOT DEFINED LIBALLEG_PATH)
    set(LIBALLEG_PATH "../libs/Allegro5")
endif()

# ImGUI library path
if (NOT DEFINED LIBIMGUI_PATH)
    set(LIBIMGUI_PATH "../libs/imgui")
endif()


### PROJECT STRUCTURE ###

# Sources path
FILE(GLOB src src/*.cpp src/**/*.cpp)

# Main library definition
add_library(turbo_engine SHARED ${src})

# Include directories path
target_include_directories(turbo_engine PUBLIC ./include)


### DEFINING LIBRARIES ###

# Required allegro libs
set(req_libs allegro allegro_ttf allegro_font allegro_image allegro_primitives)

if(WIN32)
    # Adding allegro include directory
    target_include_directories(turbo_engine PUBLIC ${LIBALLEG_PATH}/include)

    # Adding allegro library directory
    target_link_directories(turbo_engine PUBLIC ${LIBALLEG_PATH}/lib)
endif()

### POST BUILD DLL MOVING COMMANDS ###

# Moving libraries is only required on windows
if (WIN32)
    # Getting the allegro binary location
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/${LIBALLEG_PATH}/bin" src_path)
    # Getting the destination location
    file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" target_path)
    # A little message, it's beautiful <3
    message( "Setting up post-build commands")
    # For all selected libraries
    foreach(file_lib ${req_libs})
        # Moving message
        message( "${src_path}\\${file_lib}-5.2.dll to ${target_path}\\${file_lib}-5.2.dll")
        # Command definition
        add_custom_command(TARGET turbo_engine POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${src_path}/${file_lib}-5.2.dll"
                "${CMAKE_BINARY_DIR}/${file_lib}-5.2.dll"
                COMMENT "Copy ${src_path}/${file_lib}-5.2.dll to ${target_path}/${file_lib}-5.2.dll")
    endforeach(file_lib)
    # Again a message, so lovely
    message( "Post-build commands are setup")
endif()

### IMGUI LINKING ###

# Getting build type in lowercase to avoid the case sensitivity
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
# Only if we're in debug
if ("${CMAKE_BUILD_TYPE_LOWER}" STREQUAL "debug")
    # Getting the imgui binary path
    if (WIN32)
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/${LIBIMGUI_PATH}/bin/libimgui.dll" libfile)
    else()
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/${LIBIMGUI_PATH}/lib/libimgui.so" libfile)
    endif()
    # OwO OMG! again a message, so so cute UwU
    message("Debug mode detected, implement IMGUI")
    # Notify the compiler that it should define the imgui macro
    add_compile_definitions(__TURBO_USE_IMGUI__)
    # Add the include directory to the project
    include_directories(${LIBIMGUI_PATH}/include)
    # Append the imgui lib to the required library
    list(APPEND req_libs ${libfile})
    # Adding the post build command to move the imgui binary
    add_custom_command(TARGET turbo_engine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${libfile}
            "${CMAKE_BINARY_DIR}/libimgui.dll"
            COMMENT "Copy ${libfile} to ${CMAKE_BINARY_DIR}/libimgui.dll")
endif()

### LINKING LIBRARIES ###

target_link_libraries(turbo_engine ${req_libs})

### DEMO PROJECT ###

add_executable(demo main.cpp)
target_link_libraries(demo turbo_engine)
