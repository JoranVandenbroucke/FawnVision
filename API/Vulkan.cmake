set_property(GLOBAL PROPERTY VULKAN_API_WRAPPER_FILES "")

function(GetVulkanWrapperFiles)
    set(VULKAN_API_WRAPPER_FILES
            Vulkan/Wrapper/Base.h
            Vulkan/Wrapper/VkCommandHandler.h
            Vulkan/Wrapper/VkDevice.h
            Vulkan/Wrapper/VkFence.h
            Vulkan/Wrapper/VkImageView.h
            Vulkan/Wrapper/VkInstance.h
            Vulkan/Wrapper/VkQueue.h
            Vulkan/Wrapper/VkSemaphore.h
            Vulkan/Wrapper/VkShader.h
            Vulkan/Wrapper/VkSurface.h
            Vulkan/Wrapper/VkSwapChain.h

            Vulkan/DeerVulkan.h
            Vulkan/DeerVulkan_Core.h
            Vulkan/Instance.cpp
            Vulkan/Instance.h
            Vulkan/Material.cpp
            Vulkan/Material.h
            Vulkan/Presenter.cpp
            Vulkan/Presenter.h
            Vulkan/Shader.cpp
            Vulkan/Shader.h
            Vulkan/Texture.cpp
            Vulkan/Texture.h
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