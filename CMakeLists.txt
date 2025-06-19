cmake_minimum_required(VERSION 3.31)

project(SweepMiner)
enable_language(RC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_link_options(-mwindows -municode)

add_executable(
        ${PROJECT_NAME} WIN32
        src/cell.cpp
        src/game.cpp
        src/image.cpp
        src/timer.cpp
        src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_sources(${PROJECT_NAME} PRIVATE assets/sweepminer.rc)

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        comctl32
        uxtheme
        gdiplus
        winmm
)

if (MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE /MANIFEST:NO)
endif ()
