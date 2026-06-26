cmake_minimum_required(VERSION 3.30)
project(YourProject) # <--- Update this to change the name of your project

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

########################################################################

## Setup

# Project Libraries
set(LIBS)

# Source files
file(GLOB_RECURSE ENGINE_SOURCES src/Engine/*.cpp include/Engine/*.h)

set(SOURCES src/main.cpp
        ${ENGINE_SOURCES}
        include/Engine/Core/scene_interface.h
        include/Engine/Core/app_event.h
        include/Engine/Layers/scene_manager.h
        include/Engine/Core/layer_interface.h
        src/Engine/Layers/scene_manager.cpp
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

########################################################################

## Executables
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Copy assets
file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE>
        $<$<CONFIG:Release>:RELEASE_MODE>
)