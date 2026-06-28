cmake_minimum_required(VERSION 3.30)
project(YourProject) # <--- Update this to change the name of your project

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
        include/App/Scenes/initial_scene.h
        src/App/Scenes/initial_scene.cpp
        include/App/Scenes/scene_2d.h
        src/App/Scenes/scene_2d.cpp
)

# CPM
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)


########################################################################

## Dependencies

### SDL
CPMAddPackage("gh:libsdl-org/SDL#c805a4d")
list(APPEND LIBS SDL3::SDL3)

### spdlog
CPMAddPackage("gh:gabime/spdlog#a297670")
list(APPEND LIBS spdlog::spdlog_header_only)

# EnTT
CPMAddPackage("gh:skypjack/entt@3.16.0")
list(APPEND LIBS EnTT::EnTT)

# SDL_image
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE) # Requires Perl and Nasm for libaom (AVIF image format)
CPMAddPackage("gh:libsdl-org/SDL_image#fcb9d0b")
list(APPEND LIBS SDL3_image::SDL3_image)

# GLM
CPMAddPackage("gh:g-truc/glm#6f14f47")
list(APPEND LIBS glm::glm)

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

# Add a custom command to copy the assets directory
add_custom_command(
        OUTPUT "${ASSETS_DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_SOURCE_DIR}" "${ASSETS_DEST_DIR}"
        COMMENT "Copying assets to output directory"
)
# Add a custom target that depends on the copy command
add_custom_target(copy_assets ALL DEPENDS "${ASSETS_DEST_DIR}")

set_property(DIRECTORY PROPERTY ADDITIONAL_CLEAN_FILES "${ASSETS_DEST_DIR}")
