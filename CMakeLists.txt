cmake_minimum_required(VERSION 3.26)

project(
    polaris
    VERSION 0.1.0
    DESCRIPTION "Basic Raytracer"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_VULKAN "Enable vulkan render backend" OFF)

# Collect sources and headers
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cxx"
    "${PROJECT_SOURCE_DIR}/src/*.cc"
)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.hh"
)

# vk subdir is handled by own cmake file
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*/src/vk/.*")
list(FILTER PROJECT_HEADERS EXCLUDE REGEX ".*/src/vk/.*")

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

if(USE_VULKAN)
    set(PROJECT_VK_COMPONENT_NAME "polaris_vk")
    add_subdirectory("${PROJECT_SOURCE_DIR}/src/vk")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_VK_COMPONENT_NAME})
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_VULKAN)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE tbb)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive- /Zc:preprocessor /utf-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS})
