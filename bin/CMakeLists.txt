add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/"
    VERBATIM
)

add_dependencies(${PROJECT_NAME} copy_assets)

set_target_properties(copy_assets PROPERTIES
    EXCLUDE_FROM_ALL FALSE
)
