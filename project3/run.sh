#!/bin/sh
clear

g++ \
    utils.cpp \
    Disk.cpp \
    FileSystem.cpp \
    main.cpp \
    -std=c++11 -o main \
    && ./main $1