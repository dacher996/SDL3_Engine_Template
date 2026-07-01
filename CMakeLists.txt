cmake_minimum_required(VERSION 3.30)
project(YourProject) # <--- Update this to change the name of your project

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

########################################################################

## Setup

# Project Libraries
set(LIBS)

# Source files
file(GLOB_RECURSE ENGINE_SOURCES
        include/Engine/*.h
        src/Engine/*.cpp)

set(SOURCES src/main.cpp
        ${ENGINE_SOURCES}
        include/App/Core/components.h
        include/App/Scenes/initial_scene.h
        src/App/Scenes/initial_scene.cpp
        include/App/Scenes/scene_2d.h
        src/App/Scenes/scene_2d.cpp
        include/App/Core/Parsers/inhouse_spritesheet_parser.h
        src/App/Core/Parsers/inhouse_spritesheet_parser.cpp
)

########################################################################

## Dependencies

### SDL
find_package(SDL3 CONFIG REQUIRED)
list(APPEND LIBS SDL3::SDL3)

### spdlog
find_package(spdlog CONFIG REQUIRED)
list(APPEND LIBS spdlog::spdlog_header_only)

# EnTT
find_package(EnTT CONFIG REQUIRED)
list(APPEND LIBS EnTT::EnTT)

# SDL_image
find_package(SDL3_image CONFIG REQUIRED)
list(APPEND LIBS SDL3_image::SDL3_image)

# GLM
find_package(glm CONFIG REQUIRED)
list(APPEND LIBS glm::glm)

# nlohmann/json
find_package(nlohmann_json CONFIG REQUIRED)
list(APPEND LIBS nlohmann_json::nlohmann_json)

# ImGui
find_package(imgui CONFIG REQUIRED)
list(APPEND LIBS imgui::imgui)

########################################################################

## Executables

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE>
        $<$<CONFIG:Release>:RELEASE_MODE>
)

########################################################################

## Copy assets to the output directory

set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets/output")
set(ASSETS_DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

file(COPY ${ASSETS_SOURCE_DIR} DESTINATION ${ASSETS_DEST_DIR})