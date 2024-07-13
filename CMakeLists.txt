cmake_minimum_required(VERSION 3.28)
project(FawnVision VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

option(BALBINO_VULKAN "Build with the Renderer with Vulkan" ON)
set(SDL_TEST_LIBRARY OFF CACHE STRING "Build the SDL3_test library")
set(TARGET_OUTPUT ${PROJECT_NAME} CACHE STRING "Overwrite this with the exe/base project name")

if (BALBINO_VULKAN)
    include(Source/API/Vulkan.cmake)
endif ()

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
        PRIVATE
        ${VULKAN_API_FILES}
        Source/Headers/architecture.hpp
        Source/Headers/assert.hpp
        Source/Headers/compiler.hpp
        Source/Headers/enum_helpers.hpp
        Source/Headers/platform.hpp
        Source/Headers/todo.hpp
        Source/Headers/utilities.hpp

        PUBLIC
        FILE_SET CXX_MODULES FILES
        Source/Interface/FawnVision.ixx
        Source/Interface/RenderGraph.ixx
        Source/Interface/RenderPass.ixx
        Source/Interface/RenderTypes.ixx
        Source/Interface/Renderer.ixx
        Source/Interface/Shader.ixx
        Source/Interface/SharedObjectPool.ixx
        Source/Interface/Window.ixx
        Source/Interface/WindowConstants.ixx
        Source/Interface/WindowsTypes.ixx
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE Source Source/Headers)


if (NOT TARGET SDL3::SDL3)
    find_package(SDL3 QUIET PATHS "${CMAKE_SOURCE_DIR}/3rdParty/SDL")
    if (NOT SDL3_FOUND)
        if (EXISTS "${CMAKE_SOURCE_DIR}/3rdParty/SDL/CMakeLists.txt")
            add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/SDL SDL EXCLUDE_FROM_ALL)
        elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL/CMakeLists.txt")
            add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SDL/CMakeLists.txt SDL EXCLUDE_FROM_ALL)
        else ()
            include(FetchContent)
            set(SDL_SHARED TRUE CACHE BOOL "Build a SDL shared library (if available)")
            set(SDL_STATIC TRUE CACHE BOOL "Build a SDL static library (if available)")
            FetchContent_Declare(
                    SDL
                    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
                    GIT_TAG main  # Replace this with a particular git tag or git hash
                    GIT_SHALLOW TRUE
                    GIT_PROGRESS TRUE
            )
            FetchContent_MakeAvailable(SDL)
            set_property(DIRECTORY "${CMAKE_BINARY_DIR}/_deps/sdl-src" PROPERTY EXCLUDE_FROM_ALL TRUE)
        endif ()
    endif ()
else ()
    add_dependencies(${PROJECT_NAME} SDL3::SDL3)
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL3_INCLUDE_DIRS})
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)
set(SDL_LIB SDL3::SDL3)

if (BALBINO_VULKAN)
ConfigVulkanWrapper(${PROJECT_NAME})
endif ()

target_compile_options(${PROJECT_NAME}
PUBLIC
$<$<CXX_COMPILER_ID:GNU>:-fmodules-ts>
$<$<CXX_COMPILER_ID:CLANG>:-stdlib=libc++>
PRIVATE
$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /MP $<$<CONFIG:Debug>:${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT} ${DEFAULT_CXX_EXCEPTION_HANDLING}>>
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
