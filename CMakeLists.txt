cmake_minimum_required(VERSION 3.31)

project(SweepMiner)
enable_language(RC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_link_options(-mwindows -municode)

add_executable(
        ${PROJECT_NAME} WIN32
        src/image.cpp
        src/image.h
        src/timer.cpp
        src/timer.h
        src/cell.cpp
        src/cell.h
        src/game.cpp
        src/game.h
        src/main.cpp
)

target_sources(${PROJECT_NAME} PRIVATE src/resources/sweepminer.rc)

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        comctl32
        uxtheme
        gdiplus
)

if (MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE /MANIFEST:NO)
endif ()
