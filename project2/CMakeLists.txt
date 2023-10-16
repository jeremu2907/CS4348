cmake_minimum_required(VERSION 3.25)
project(proj2)

set(CMAKE_CXX_STANDARD 11)

set(
    MY_SOURCES

    main.cpp
    utils.h
)

add_executable(proj_2 ${MY_SOURCES})
