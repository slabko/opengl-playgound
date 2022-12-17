cmake_minimum_required(VERSION 3.22)

# create `compile_commands.json` for clangd and autocompletion
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-march=native)
# add_compile_options(-ffast-math)

add_compile_options(-Wall -Wextra -Wconversion -pedantic -Werror)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)

    # compiler sometimes fail to optimize loops with sanitizers enabled
    add_compile_options(-Wno-pass-failed)
endif()

project(playground)

configure_file(textures/crate.png textures/crate.png COPYONLY)
configure_file(textures/chest.png textures/chest.png COPYONLY)

configure_file(examples/scene/GLSL/fragment.glsl GLSL/fragment.glsl COPYONLY)
configure_file(examples/scene/GLSL/vertex.glsl GLSL/vertex.glsl COPYONLY)

find_package(Eigen3 3.4 REQUIRED NO_MODULE)
find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(PNG REQUIRED)
find_package(Boost REQUIRED COMPONENTS container)
find_package(TBB REQUIRED)

file(GLOB_RECURSE GLAD_FILES thirdparty/glad/src/*.c)
file(GLOB_RECURSE IMGUI_FILES thirdparty/imgui/src/*.cpp)

add_executable(playground
    main.cpp 

    playground/program.cpp
    playground/texture.cpp
    playground/png.cpp

    examples/quad/quad.cpp
    examples/game_of_life/game_of_life.cpp
    examples/scene/scene.cpp
        examples/scene/shapes/cuboid.cpp
    examples/scene/shapes/sphere.cpp

    ${GLAD_FILES}
    ${IMGUI_FILES}
)

set_source_files_properties(
    ${GLAD_FILES}
    ${IMGUI_FILES}
    PROPERTIES
    COMPILE_FLAGS "-w"
)

target_link_libraries(playground 
    fmt::fmt
    spdlog::spdlog 
    OpenGL::GL 
    glm::glm 
    SDL2::SDL2 
    Eigen3::Eigen
    OpenMP::OpenMP_CXX
    PNG::PNG
    Boost::container
    TBB::tbb # GCC only
)

# Following GLSL specifications, GLM supports three sets of components:
# x, y, z, w;  r, g, b, a;  and s, t, p, q.
# This is implemented using unions, which leads to complaints from the linter.
# This flag disables these unions leaving only x, y and z components
target_compile_definitions(playground PUBLIC GLM_FORCE_XYZW_ONLY)

# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101228, not yet fixed in GCC
target_compile_definitions(playground PUBLIC TBB_SUPPRESS_DEPRECATED_MESSAGES)


target_include_directories(playground SYSTEM PRIVATE
    thirdparty/gsl/include
    thirdparty/glad/include
    thirdparty/imgui/include
)
