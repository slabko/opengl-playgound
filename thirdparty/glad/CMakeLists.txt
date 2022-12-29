cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE GLAD_SRC src/*.c)

add_library(glad
        ${GLAD_SRC}
        )

set_source_files_properties(
        ${GLAD_SRC}
        PROPERTIES
        COMPILE_FLAGS "-w"
)

target_include_directories(glad SYSTEM PUBLIC
        include
        )