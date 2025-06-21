cmake_minimum_required(VERSION 3.26.0)
project(breakout)

option(FETCH_LIBS OFF)

if (FETCH_LIBS)
    include(FetchContent)

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
endif(FETCH_LIBS)
unset(FETCH_LIBS CACHE)

add_library(flecs STATIC flecs.c)

# find_library(CIMGUI
#     NAMES cimgui.a cimgui
#     PATHS /usr/lib /usr/local/lib)

# if(NOT CIMGUI)
#     message(FATAL_ERROR "cimgui static library not found")
# endif()

# add_library(cimgui STATIC IMPORTED)
# set_target_properties(cimgui PROPERTIES IMPORTED_LOCATION ${CIMGUI})

find_package(logger REQUIRED)

add_executable(${PROJECT_NAME} main.c)

# target_link_libraries(${PROJECT_NAME} PRIVATE raylib cimgui logger::logger)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib logger::logger flecs)

if (NOT MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()
