cmake_minimum_required(VERSION 3.16)

project(SweepMiner)

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

add_executable(${PROJECT_NAME} src/main.cpp
        src/game.cpp
        src/box.cpp
        src/button.cpp
        src/cell.cpp
        src/timer.cpp
        src/mouse.cpp
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
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_mixer::SDL3_mixer)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
target_link_libraries(imgui PRIVATE SDL3::SDL3)
target_compile_features(imgui PRIVATE cxx_std_23)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/O2>)
    target_compile_options(imgui PRIVATE $<$<CONFIG:Release>:/O2>)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-O3>)
    target_compile_options(imgui PRIVATE $<$<CONFIG:Release>:-O3>)
endif()
