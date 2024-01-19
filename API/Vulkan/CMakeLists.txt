cmake_minimum_required(VERSION 3.27)
set(PROJECT_NAME DeerVulkan)
set(ROOT_NAMESPACE ${PROJECT_NAME})
project(${PROJECT_NAME})

# VULKAN_LOCATION needs to be overwritten by used Project
set(VULKAN_LOCATION "" CACHE STRING "The location of Vulkan")

add_library(${PROJECT_NAME}
        Wrapper/Base.h
        Wrapper/VkCommandHandler.cpp
        Wrapper/VkCommandHandler.h
        Wrapper/VkDevice.cpp
        Wrapper/VkDevice.h
        Wrapper/VkFence.cpp
        Wrapper/VkFence.h
        Wrapper/VkInstance.cpp
        Wrapper/VkInstance.h
        Wrapper/VkQueue.cpp
        Wrapper/VkQueue.h
        Wrapper/VkSemaphore.cpp
        Wrapper/VkSemaphore.h
        Wrapper/VkSurface.cpp
        Wrapper/VkSurface.h
        Wrapper/VkSwapChain.cpp
        Wrapper/VkSwapChain.h
        DeerVulkan.h
        DeerVulkan_Core.h
        Instance.cpp
        Instance.h
        Presenter.cpp
        Presenter.h
        RenderPipeline.cpp
        RenderPipeline.h
        Texture.cpp
        Texture.h
        Mesh.cpp
        Mesh.h
)

if (VULKAN_LOCATION STREQUAL "")
    message(FATAL_ERROR "No path to the vulkan sdk is given")
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        ${VULKAN_LOCATION}/include
)
target_link_directories(${PROJECT_NAME} PUBLIC
        ${VULKAN_LOCATION}/lib
)
target_link_libraries(${PROJECT_NAME} PUBLIC
        vulkan-1
)

if (SDL_LOCATION STREQUAL "")
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
    set(SDL_LIB SDL3::SDL3)
else ()
    add_dependencies(${PROJECT_NAME} SDL3::SDL3)
    set(SDL_LIB SDL3::SDL3)

    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL_LOCATION}/include)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL_LIB})
endif ()

target_compile_features(${PROJECT_NAME} PRIVATE
        c_static_assert
        c_std_17
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
