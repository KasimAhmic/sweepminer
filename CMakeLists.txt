cmake_minimum_required(VERSION 3.16)

project(SweepMiner)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} src/main.cpp
        src/game.cpp
        src/cell.cpp
        src/timer.cpp
        src/mouse.cpp
        src/scaler.cpp
        src/debug_menu.cpp
        src/menu_bar.cpp
        src/menu_bar.hpp)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

find_package(SDL3_ttf CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf)

find_package(SDL3_image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)

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

target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
target_link_libraries(imgui PRIVATE SDL3::SDL3)
target_compile_features(imgui PRIVATE cxx_std_23)

file(COPY assets DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-O3>)
