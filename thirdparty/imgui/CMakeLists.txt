cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE IMGUI_SRC src/*.cpp)

add_library(imgui
        ${IMGUI_SRC}
        )

set_source_files_properties(
        ${IMGUI_SRC}
        PROPERTIES
        COMPILE_FLAGS "-w"
)

find_package(SDL2 REQUIRED)
target_link_libraries(imgui
        SDL2::SDL2
        )

target_include_directories(imgui SYSTEM PUBLIC
        include
        )