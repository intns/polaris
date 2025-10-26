find_package(Vulkan REQUIRED)
if(NOT Vulkan_GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found - required for shader compilation")
endif()

set(SHADER_DIR "${PROJECT_SOURCE_DIR}/src/vk/shaders")
set(SPIRV_OUT_DIR "${CMAKE_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY "${SPIRV_OUT_DIR}")

file(GLOB_RECURSE GLSL_SHADERS
    RELATIVE "${SHADER_DIR}"
    "${SHADER_DIR}/*.vert"
    "${SHADER_DIR}/*.frag"
    "${SHADER_DIR}/*.comp"
)

file(GLOB_RECURSE VK_COMPONENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

file(GLOB_RECURSE VK_COMPONENT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hh"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

set(SPIRV_BINARIES "")
foreach(SHADER_PATH ${GLSL_SHADERS})
    set(INPUT_SHADER "${SHADER_DIR}/${SHADER_PATH}")
    set(SPIRV "${SPIRV_OUT_DIR}/${SHADER_PATH}.spv")

    get_filename_component(OUT_DIR ${SPIRV} DIRECTORY)
    file(MAKE_DIRECTORY "${OUT_DIR}")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${INPUT_SHADER} -o ${SPIRV}
        DEPENDS ${INPUT_SHADER}
        COMMENT "Compiling ${SHADER_PATH}"
        VERBATIM
    )

    list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach()

set(SHADER_GENERATED_CPP "${CMAKE_BINARY_DIR}/generated/shaders_generated.cpp")

add_custom_command(
    OUTPUT ${SHADER_GENERATED_CPP}
    COMMAND ${CMAKE_COMMAND} -DOUTPUT="${SHADER_GENERATED_CPP}"
    -DSPIRV_BINARIES="${SPIRV_BINARIES}"
    -DSHADER_DIR="${SHADER_DIR}"
    -DSPIRV_OUT_DIR="${SPIRV_OUT_DIR}"
    -P ${PROJECT_SOURCE_DIR}/cmake/generate_shaders.cmake
    DEPENDS ${SPIRV_BINARIES}
    COMMENT "Embedding shader bytecode into binary"
)

if(DEFINED BUILD_GLFW OR BUILD_GLFW)
    include(FetchContent)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG latest
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
    set(GLFW_INSTALL OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(glfw)
else()
    find_package(glfw3 REQUIRED)
endif()

add_library(${PROJECT_VK_COMPONENT_NAME}
    ${SHADER_GENERATED_CPP}
    VulkanApp.cpp
)

target_include_directories(${PROJECT_VK_COMPONENT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(${PROJECT_VK_COMPONENT_NAME} PRIVATE Vulkan::Vulkan glfw)
target_compile_features(${PROJECT_VK_COMPONENT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_VK_COMPONENT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${VK_COMPONENT_SOURCES} ${VK_COMPONENT_HEADERS})
