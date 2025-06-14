cmake_minimum_required(VERSION 3.26.0)
project(breakout)

option(FETCH_LIBS OFF)

if (FETCH_LIBS)
    include(FetchContent)

    FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(box2d)

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(raylib)
else()
    find_package(raylib REQUIRED)
    find_package(box2d REQUIRED)
endif(FETCH_LIBS)
unset(FETCH_LIBS CACHE)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib box2d)

if (NOT MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()
