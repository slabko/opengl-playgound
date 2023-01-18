cmake_minimum_required(VERSION 3.22)

# create `compile_commands.json` for clangd and autocompletion
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-march=native)

add_compile_options(-Wall -Wextra -Wconversion -pedantic -Werror)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)

    # compiler sometimes fail to optimize loops with sanitizers enabled
    add_compile_options(-Wno-pass-failed)
endif ()

project(playground)

file(GLOB TEXTURES examples/scene/textures/*)
file(COPY ${TEXTURES} DESTINATION textures/)

file(GLOB SHADERS examples/scene/GLSL/*)
file(COPY ${SHADERS} DESTINATION GLSL/)

file(GLOB RESOURCES examples/scene/resources/*)
file(COPY ${RESOURCES} DESTINATION resources/)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(PNG REQUIRED)

file(GLOB_RECURSE PLAYGROUND_SRC playground/*.cpp examples/*.cpp)

add_executable(playground
        main.cpp
        ${PLAYGROUND_SRC}
        )

add_subdirectory(thirdparty/imgui)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/gsl)

target_link_libraries(playground
        fmt::fmt
        spdlog::spdlog
        OpenGL::GL
        glm::glm
        SDL2::SDL2
        PNG::PNG
        imgui
        glad
        gsl
        )

# Following GLSL specifications, GLM supports three sets of components:
# x, y, z, w;  r, g, b, a;  and s, t, p, q.
# This is implemented using unions, which leads to complaints from the linter.
# This flag disables these unions leaving only x, y and z components
target_compile_definitions(playground PUBLIC GLM_FORCE_XYZW_ONLY)
