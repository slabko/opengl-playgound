cmake_minimum_required(VERSION 3.22)

add_library(gsl INTERFACE)

target_include_directories(gsl SYSTEM INTERFACE
        include
        )