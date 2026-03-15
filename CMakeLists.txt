cmake_minimum_required(VERSION 3.28)
project(FawnVision VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BALBINO_VULKAN "Build with the Renderer with Vulkan" ON)

if (BALBINO_VULKAN)
    include(source/API/Vulkan.cmake)
endif ()

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
        PRIVATE
        ${VULKAN_API_FILES}
        source/Headers/architecture.hpp
        source/Headers/assert.hpp
        source/Headers/compiler.hpp
        source/Headers/enum_helpers.hpp
        source/Headers/platform.hpp
        source/Headers/todo.hpp
        source/Headers/utilities.hpp
)
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET fawnvision_module TYPE CXX_MODULES FILES
        source/Interface/UI/DeerUI.ixx
        source/Interface/Buffer.ixx
        source/Interface/Descriptor.ixx
        source/Interface/FawnEnums.ixx
        source/Interface/FawnVision.ixx
        source/Interface/Mesh.ixx
        source/Interface/Renderer.ixx
        source/Interface/RenderGraph.ixx
        source/Interface/RenderPass.ixx
        source/Interface/RenderPassContext.ixx
        source/Interface/Shader.ixx
        source/Interface/Texture.ixx
        source/Interface/Window.ixx
)

add_subdirectory(Font)
add_dependencies(${PROJECT_NAME} DeerFont::DeerFont)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE Source source/Headers)

include(setup_sdl3)
include(setup_fawnalgebra)
setup_sdl3(${PROJECT_NAME})
setup_fawnalgebra(${PROJECT_NAME})

if (BALBINO_VULKAN)
    ConfigVulkanWrapper(${PROJECT_NAME})
endif ()

target_compile_options(${PROJECT_NAME}
        PRIVATE
        $<$<CONFIG:Debug>:${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT} ${DEFAULT_CXX_EXCEPTION_HANDLING}>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /MP >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
        PUBLIC
        $<$<CXX_COMPILER_ID:GNU>:-fmodules-ts>
        # todo remove -Wdepricated -Wdepricated-declarations
        #todo --embed-dir=${CMAKE_SOURCE_DIR}/Embedded/
        $<$<CXX_COMPILER_ID:Clang>: -fembed-bitcode>
)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-DBALBINO_DEBUG>)
target_compile_features(${PROJECT_NAME} PRIVATE
        cxx_aggregate_default_initializers
        cxx_alignof
        cxx_attributes
        cxx_auto_type
        cxx_binary_literals
        cxx_constexpr
        cxx_contextual_conversions
        cxx_defaulted_functions
        cxx_defaulted_move_initializers
        cxx_delegating_constructors
        cxx_deleted_functions
        cxx_enum_forward_declarations
        cxx_explicit_conversions
        cxx_final
        cxx_generalized_initializers
        cxx_generic_lambdas
        cxx_inheriting_constructors
        cxx_inline_namespaces
        cxx_lambda_init_captures
        cxx_lambdas
        cxx_long_long_type
        cxx_noexcept
        cxx_nonstatic_member_init
        cxx_nullptr
        cxx_override
        cxx_range_for
        cxx_raw_string_literals
        cxx_right_angle_brackets
        cxx_rvalue_references
        cxx_sizeof_member
        cxx_static_assert
        cxx_std_23
        cxx_strong_enums
        cxx_thread_local
        cxx_uniform_initialization
        cxx_unrestricted_unions
        cxx_user_literals
)

install(FILES
        $<TARGET_FILE:SDL3::SDL3>
        DESTINATION ${CMAKE_BINARY_DIR}  # Install the DLLs in the binary directory
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        ${CMAKE_BINARY_DIR}
)
