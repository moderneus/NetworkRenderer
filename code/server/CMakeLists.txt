cmake_minimum_required(VERSION 4.2)

set(PROJECT Server)

project(${PROJECT})

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(${PROJECT} ${SRC})

target_compile_options(${PROJECT} PRIVATE --std=c++20 -g -O1 -Wall -Werror)
target_include_directories(${PROJECT} PRIVATE include)
target_include_directories(${PROJECT} SYSTEM PRIVATE ../libs/fmt/include ../libs/glm ../libs/SDL/include ../libs/vma/include ../libs/volk)
target_link_libraries(${PROJECT} PRIVATE fmt::fmt glm::glm SDL3::SDL3 volk::volk)

find_program(GLSLC glslc REQUIRED)

set(SHADERS_SRC_DIR ${CMAKE_SOURCE_DIR}/server/shaders)
set(SHADERS_BIN_DIR ${CMAKE_BINARY_DIR}/server/shaders)

file(GLOB_RECURSE SHADER_SOURCES
    ${SHADERS_SRC_DIR}/*.vert
    ${SHADERS_SRC_DIR}/*.frag
)

set(SHADER_BINARIES "")

foreach(SHADER ${SHADER_SOURCES})
    file(RELATIVE_PATH REL_PATH ${SHADERS_SRC_DIR} ${SHADER})

    set(OUTPUT ${SHADERS_BIN_DIR}/${REL_PATH}.spv)

    get_filename_component(OUTPUT_DIR ${OUTPUT} DIRECTORY)
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    add_custom_command(
        OUTPUT ${OUTPUT}
        COMMAND ${GLSLC} ${SHADER} -o ${OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${REL_PATH}"
    )

    list(APPEND SHADER_BINARIES ${OUTPUT})
endforeach()

add_custom_target(shaders ALL
    DEPENDS ${SHADER_BINARIES}
)

add_dependencies(${TARGET} shaders)
