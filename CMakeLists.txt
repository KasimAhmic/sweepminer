cmake_minimum_required(VERSION 3.16)

project(SweepMiner)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SDLTTF_VENDORED ON)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} src/main.cpp
        src/game.cpp
        src/cell.cpp
        src/timer.cpp
        src/mouse.cpp
        src/debug_menu.cpp
        src/menu_bar.cpp)

add_library(imgui STATIC
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_sdl3.cpp
        external/imgui/backends/imgui_impl_sdlrenderer3.cpp)

target_include_directories(imgui PUBLIC
        external/imgui
        external/imgui/backends)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
target_link_libraries(imgui PRIVATE SDL3::SDL3)
target_compile_features(imgui PRIVATE cxx_std_23)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-O3>)
