cmake_minimum_required(VERSION 4.3.0)
set(CMAKE_CXX_COMPILER_IMPORT_STD ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
project(FawnVision VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_MODULE_STD 1)

option(BALBINO_VULKAN "Build with the Renderer with Vulkan" ON)

if (BALBINO_VULKAN)
    include(source/api/vulkan.cmake)
endif ()

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
        PRIVATE
        ${VULKAN_SOURCE_FILES}
        PRIVATE
        FILE_SET vulkan_headers TYPE HEADERS
        BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/source/api/vulkan
        FILES
        ${VULKAN_HEADER_FILES}

        FILE_SET util_headers TYPE HEADERS
        BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/source/headers
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/source/headers/architecture.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/headers/assert.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/headers/compiler.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/headers/enum_helpers.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/headers/platform.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/headers/todo.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/headers/utilities.hpp

        PUBLIC
        FILE_SET fawnvision_module TYPE CXX_MODULES
        BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/ui/deer_ui.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/buffer.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/descriptor.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/fawn_enums.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/fawn_vision.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/mesh.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/renderer.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/render_graph.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/render_pass.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/render_pass_context.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/shader.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/texture.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/source/interface/window.ixx

        PUBLIC
        FILE_SET embedded_headers TYPE HEADERS
        BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/embedded
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/embedded/images.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/embedded/shaders.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/source
        ${PROJECT_SOURCE_DIR}/source/headers

        PUBLIC
        ${PROJECT_SOURCE_DIR}/embedded
)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BALBINO_DEBUG> )
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
    CXX_SCAN_FOR_MODULES  ON
)

include(compiler_select_flags)
include(compiler_select_sdl)
include(compiler_select_linker)
include(setup_sdl3)
include(setup_fawnalgebra)
set_compile_flags(${PROJECT_NAME})
select_best_stl(${PROJECT_NAME})
select_best_linker(${PROJECT_NAME})
setup_sdl3(${PROJECT_NAME})
setup_fawnalgebra(${PROJECT_NAME})

add_subdirectory(font)
add_dependencies(${PROJECT_NAME} DeerFont::DeerFont)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(${CMAKE_CURRENT_SOURCE_DIR}/embedded/shaders.cmake)

if (BALBINO_VULKAN)
    setup_vulkan(${PROJECT_NAME})
endif ()

install(FILES
        $<TARGET_FILE:SDL3::SDL3>
        DESTINATION ${CMAKE_BINARY_DIR}  # Install the DLLs in the binary directory
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        ${CMAKE_BINARY_DIR}
)
