cmake_minimum_required(VERSION 3.16)
project(SweepMiner)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

set(LIBS)

# SDL
CPMAddPackage(
        NAME SDL3
        GIT_REPOSITORY http://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.16
        VERSION 3.2.16
)
list(APPEND LIBS SDL3::SDL3)

# Aseprite's fork of FreeType
CPMAddPackage(
        NAME freetype
        GIT_REPOSITORY https://github.com/aseprite/freetype2.git
        GIT_TAG VER-2-10-0
        VERSION 2.10.0
)
if (freetype_ADDED)
    add_library(Freetype::Freetype ALIAS freetype)
endif()

# SDL TTF
CPMAddPackage(
        NAME SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.2
        VERSION 3.2.2
)
list(APPEND LIBS SDL3_ttf::SDL3_ttf)

# SDL Image
CPMAddPackage(
        NAME SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.2.4
        VERSION 3.2.4
)
list(APPEND LIBS SDL3_image::SDL3_image)

set(SDL_MAIN_CALLBACK_STANDARD 1)

add_executable(${PROJECT_NAME} src/main.cpp
        src/game.cpp
        src/cell.cpp
        src/timer.cpp)

file(COPY assets DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-O3>)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
