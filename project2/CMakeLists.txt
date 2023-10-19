cmake_minimum_required(VERSION 3.25)
project(proj2)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

set(
    MY_SOURCES

    main.cpp
    utils.h
)

add_executable(proj_2 ${MY_SOURCES})
