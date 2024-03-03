cmake_minimum_required(VERSION 3.27)
set(PROJECT_NAME FawnVision)
set(ROOT_NAMESPACE ${PROJECT_NAME})
project(${PROJECT_NAME})

option(BALBINO_VULKAN "Build with the Renderer with Vulkan" ON)
option(BALBINO_BUILD_SDL "Build SDL (OFF when just include or depend on it" ON)
set(SDL_TEST_LIBRARY OFF CACHE STRING "Build the SDL3_test library")
set(SDL_LOCATION "" CACHE STRING "The location of SDL3")
set(TARGET_OUTPUT ${PROJECT_NAME} CACHE STRING "Overwrite this with the exe/base project name")

set(API_FILES)
if (BALBINO_VULKAN)
    include(API/Vulkan.cmake)
    GetVulkanWrapperFiles()
    get_property(VULKAN_API_WRAPPER_FILES GLOBAL PROPERTY VULKAN_API_WRAPPER_FILES)
    set(API_FILES ${VULKAN_API_WRAPPER_FILES})
endif ()
add_library(${PROJECT_NAME}
        Includes/FawnVision.hpp
        Includes/FawnVision_Core.hpp
        Includes/FawnVision_Error.hpp
        Includes/FawnVision_Material.hpp
        Includes/FawnVision_Renderer.hpp
        Includes/FawnVision_Shader.hpp
        Includes/FawnVision_Window.hpp
        Includes/FawnVision_WindowEvents.hpp
        Includes/Versions.hpp

        Source/Error.cpp
        Source/Error.hpp
        Source/Material.cpp
        Source/ObjectDefinitions.hpp
        Source/Renderer.cpp
        Source/Shader.cpp
        Source/Window.cpp
        Source/WindowEvents.cpp
        Source/WindowEvents.hpp

        ${API_FILES}
)
target_include_directories(${PROJECT_NAME} PRIVATE
        Includes
)

if (SDL_LOCATION STREQUAL "")
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
    set(SDL_LIB SDL3::SDL3)
else ()
    if (BALBINO_BUILD_SDL)
        add_subdirectory(${SDL_LOCATION} SDL3)
    endif ()
    add_dependencies(${PROJECT_NAME} SDL3::SDL3)
    set(SDL_LIB SDL3::SDL3)

    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL_LOCATION}/include)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL_LIB})
endif ()


if (BALBINO_VULKAN)
    ConfigVulkanWrapper(${PROJECT_NAME})
endif ()

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX /MP $<$<CONFIG:Debug>:${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT} ${DEFAULT_CXX_EXCEPTION_HANDLING}>
        >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_compile_features(${PROJECT_NAME} PRIVATE
        c_function_prototypes
        c_restrict
        c_static_assert
        c_std_17
        c_variadic_macros
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
        $<TARGET_FILE:${SDL_LIB}>
        DESTINATION ${TARGET_OUTPUT}  # Install the DLLs in the binary directory
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${SDL_LIB}>
        $<TARGET_FILE_DIR:${TARGET_OUTPUT}>
)
