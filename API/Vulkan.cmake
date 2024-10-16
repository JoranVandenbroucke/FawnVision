set_property(GLOBAL PROPERTY VULKAN_API_WRAPPER_FILES "")

function(GetVulkanWrapperFiles)
    set(VULKAN_API_WRAPPER_FILES
            Vulkan/Wrapper/VkCommandPool.hpp
            Vulkan/Wrapper/VkDevice.hpp
            Vulkan/Wrapper/VkFence.hpp
            Vulkan/Wrapper/VkImageView.hpp
            Vulkan/Wrapper/VkInstance.hpp
            Vulkan/Wrapper/VkQueue.hpp
            Vulkan/Wrapper/VkSemaphore.hpp
            Vulkan/Wrapper/VkShader.hpp
            Vulkan/Wrapper/VkSurface.hpp
            Vulkan/Wrapper/VkSwapChain.hpp

            Vulkan/DeerVulkan.hpp
            Vulkan/DeerVulkan_Core.hpp
            Vulkan/Instance.cpp
            Vulkan/Instance.hpp
            Vulkan/Material.cpp
            Vulkan/Material.hpp
            Vulkan/Presenter.cpp
            Vulkan/Presenter.hpp
            Vulkan/Shader.cpp
            Vulkan/Shader.hpp
            Vulkan/Texture.cpp
            Vulkan/Texture.hpp
    )
    list(TRANSFORM VULKAN_API_WRAPPER_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/API/")
    set_property(GLOBAL APPEND PROPERTY VULKAN_API_WRAPPER_FILES ${VULKAN_API_WRAPPER_FILES})
endfunction()

function(ConfigVulkanWrapper TARGET_NAME)
    # VULKAN_LOCATION needs to be overwritten by used Project
    set(VULKAN_LOCATION "" CACHE STRING "The location of Vulkan")
    if (VULKAN_LOCATION STREQUAL "")
        message(FATAL_ERROR "No path to the vulkan sdk is given")
    endif ()

    target_include_directories(${TARGET_NAME} PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/API/Vulkan
            ${VULKAN_LOCATION}/include
    )
    target_link_directories(${TARGET_NAME} PUBLIC
            ${VULKAN_LOCATION}/lib
    )
    target_link_libraries(${TARGET_NAME} PUBLIC
            vulkan-1
    )
    target_compile_definitions(${TARGET_NAME} PUBLIC BALBINO_VULKAN)

    if (SDL_LOCATION STREQUAL "")
        find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
        set(SDL_LIB SDL3::SDL3)
    else ()
        add_dependencies(${TARGET_NAME} SDL3::SDL3)
        set(SDL_LIB SDL3::SDL3)

        target_include_directories(${TARGET_NAME} PRIVATE ${SDL_LOCATION}/include)
        target_link_libraries(${TARGET_NAME} PRIVATE ${SDL_LIB})
    endif ()
endfunction()