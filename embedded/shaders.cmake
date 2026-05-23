set(BALBINO_SHADER_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../data")
set(BALBINO_SHADER_EMBEDDED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/embedded")
set(BALBINO_SHADER_GEN_SCRIPT "${BALBINO_SHADER_EMBEDDED_DIR}/gen_shader.sh")

set(BALBINO_SHADER_GLSL
        fullscreen.vert
        fullscreen.frag
        triangle.vert
        triangle.frag
        ui.vert
        ui.frag
)

find_program(BALBINO_GLSLANGVALIDATOR glslangValidator
        HINTS
        "$ENV{VULKAN_SDK}/bin"
        "$ENV{VULKAN_SDK}/Bin"
        REQUIRED
)

set(BALBINO_SHADER_SPV_FILES "")
set(BALBINO_SHADER_GLSL_SOURCES "")

foreach(SHADER IN LISTS BALBINO_SHADER_GLSL)
    set(GLSL_SOURCE "${BALBINO_SHADER_DATA_DIR}/${SHADER}")
    set(SPV_OUTPUT "${BALBINO_SHADER_EMBEDDED_DIR}/${SHADER}.spv")

    if (NOT EXISTS "${GLSL_SOURCE}")
        message(FATAL_ERROR "[FawnVision] Missing shader source: ${GLSL_SOURCE}")
    endif ()

    list(APPEND BALBINO_SHADER_GLSL_SOURCES "${GLSL_SOURCE}")
    list(APPEND BALBINO_SHADER_SPV_FILES "${SPV_OUTPUT}")

    add_custom_command(
            OUTPUT "${SPV_OUTPUT}"
            COMMAND "${BALBINO_GLSLANGVALIDATOR}" -V "${GLSL_SOURCE}" -o "${SPV_OUTPUT}"
            DEPENDS "${GLSL_SOURCE}"
            COMMENT "[FawnVision] Compiling shader ${SHADER}"
            VERBATIM
    )
endforeach()

set(BALBINO_SHADERS_HPP "${BALBINO_SHADER_EMBEDDED_DIR}/shaders.hpp")

add_custom_command(
        OUTPUT "${BALBINO_SHADERS_HPP}"
        COMMAND "${CMAKE_COMMAND}" -E env bash "${BALBINO_SHADER_GEN_SCRIPT}" "${BALBINO_SHADER_EMBEDDED_DIR}"
        DEPENDS
        ${BALBINO_SHADER_SPV_FILES}
        "${BALBINO_SHADER_GEN_SCRIPT}"
        COMMENT "[FawnVision] Generating embedded/shaders.hpp"
        VERBATIM
)

add_custom_target(FawnVision_Shaders
        DEPENDS
        ${BALBINO_SHADER_SPV_FILES}
        "${BALBINO_SHADERS_HPP}"
)

add_dependencies(${PROJECT_NAME} FawnVision_Shaders)
