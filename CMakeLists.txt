cmake_minimum_required(VERSION 3.16)

project(SweepMiner)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# SDL configuration
set(SDL_STATIC                  ON  CACHE BOOL "" FORCE)
set(SDL_SHARED                  OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS           OFF CACHE BOOL "" FORCE)

# SDL_ttf configuration
set(SDLTTF_VENDORED             ON  CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ             OFF CACHE BOOL "" FORCE)
set(SDLTTF_PLUTOSVG             OFF CACHE BOOL "" FORCE)

# SDL_image configuration
set(SDLIMAGE_VENDORED           ON  CACHE BOOL "" FORCE)
set(SDLIMAGE_PNG                ON  CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF               OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_BMP                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_GIF                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JPG                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP               OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_GIF                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_LBM                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PCX                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PNM                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_QOI                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_SVG                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TGA                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XCF                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XPM                OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XV                 OFF CACHE BOOL "" FORCE)

# SDL_mixer configuration
set(SDLMIXER_VENDORED           ON  CACHE BOOL "" FORCE)
set(SDLMIXER_WAVE               ON  CACHE BOOL "" FORCE)
set(SDLMIXER_AIFF               OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VOC                OFF CACHE BOOL "" FORCE)
set(SDLMIXER_AU                 OFF CACHE BOOL "" FORCE)
set(SDLMIXER_FLAC               OFF CACHE BOOL "" FORCE)
set(SDLMIXER_GME                OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MOD                OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MP3                OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MIDI               OFF CACHE BOOL "" FORCE)
set(SDLMIXER_OPUS               OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_STB         OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_VORBISFILE  OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_TREMOR      OFF CACHE BOOL "" FORCE)
set(SDLMIXER_WAVPACK            OFF CACHE BOOL "" FORCE)

add_subdirectory(external/SDL       EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf   EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_mixer EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} WIN32 src/main.cpp
        src/game.cpp
        src/game.hpp
        src/context.cpp
        src/context.hpp
        src/resource_manager.cpp
        src/resource_manager.hpp
        src/ui_component.cpp
        src/ui_component.hpp
        src/box.cpp
        src/box.hpp
        src/color.hpp
        src/score_board.cpp
        src/score_board.hpp
        src/counter.cpp
        src/counter.hpp
        src/cell_grid.cpp
        src/cell_grid.hpp
        src/cell.cpp
        src/cell.hpp
        src/pair_hash.hpp
        src/profiler.cpp
        src/profiler.hpp
        src/button.cpp
        src/button.hpp
        src/textures.hpp
        src/events.hpp
        src/timer.cpp
        src/timer.hpp
        src/new_game_button.cpp
        src/new_game_button.hpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE SWEEPMINER_ENABLE_PROFILER=0)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_mixer::SDL3_mixer)

if (APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SWEEPMINER_PLATFORM_MACOS=1)
#    set_target_properties(${PROJECT_NAME} PROPERTIES
#            MACOSX_BUNDLE_GUI_IDENTIFIER "com.ahmic.sweepminer"
#            MACOSX_BUNDLE TRUE
#    )

    target_sources(${PROJECT_NAME} PRIVATE src/menu_bar_mac.mm)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
elseif (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SWEEPMINER_PLATFORM_WINDOWS=1)

    target_sources(${PROJECT_NAME} PRIVATE src/menu_bar_win.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-lcomctl32")
else ()
    target_compile_definitions(${PROJECT_NAME} PRIVATE SWEEPMINER_PLATFORM_OTHER=1)

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

    target_sources(${PROJECT_NAME} PRIVATE src/menu_bar_imgui.cpp)

    target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
    target_link_libraries(imgui PRIVATE SDL3::SDL3)

    target_compile_features(imgui PRIVATE cxx_std_23)

    if (MSVC)
        target_compile_options(imgui PRIVATE $<$<CONFIG:Release>:/O2>)
    else ()
        target_compile_options(imgui PRIVATE $<$<CONFIG:Release>:-O3>)
    endif ()
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/O2>)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-O3>)
endif()
