cmake_minimum_required(VERSION 3.22)

# create `compile_commands.json` for clangd and autocompletion
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-march=native)
add_compile_options(-ffast-math)
add_compile_options(-Wall)

project(playground)

configure_file(textures/crate.png textures/crate.png COPYONLY)
configure_file(textures/chest.png textures/chest.png COPYONLY)

find_package(Eigen3 3.4 REQUIRED NO_MODULE)
find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(PNG REQUIRED)

add_executable(playground 
    main.cpp 

    playground/program.cpp
    playground/texture.cpp
    playground/png.cpp

    examples/quad.cpp
    examples/cube.cpp
    examples/game_of_life.cpp

    thirdparty/glad/src/glad.c 
    thirdparty/imgui/src/imgui.cpp
    thirdparty/imgui/src/imgui_demo.cpp
    thirdparty/imgui/src/imgui_draw.cpp
    thirdparty/imgui/src/imgui_tables.cpp
    thirdparty/imgui/src/imgui_widgets.cpp
    thirdparty/imgui/src/imgui_impl_opengl3.cpp
    thirdparty/imgui/src/imgui_impl_sdl.cpp
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
)

target_include_directories(playground PRIVATE 
    thirdparty/glad/include 
    thirdparty/imgui/include
)
