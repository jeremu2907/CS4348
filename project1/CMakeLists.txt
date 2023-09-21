cmake_minimum_required(VERSION 3.25)
project(proj1)

set(CMAKE_CXX_STANDARD 11)

set(
    MY_SOURCES

    main.cpp
    Memory.cpp
    Memory.h
)

add_executable(proj_1 ${MY_SOURCES})
