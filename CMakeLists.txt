cmake_minimum_required(VERSION 3.26.0)
project(breakout)

find_package(raylib REQUIRED)
find_package(box2d REQUIRED)

add_subdirectory(bin)
add_subdirectory(lib)
