//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"

#include <SDL3/SDL_vulkan.h>

#include <array>
#include <vector>

namespace DeerVulkan
{
struct SVkInstance
{
    VkInstance instance{VK_NULL_HANDLE};
#ifdef _DEBUG
    VkDebugReportCallbackEXT debugMessenger{VK_NULL_HANDLE};
#endif
};

#ifdef _DEBUG
static auto VKAPI_CALL DebugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT /*unused*/, uint64_t /*unused*/, std::size_t /*unused*/, int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
                                   void* /*unused*/) noexcept -> VkBool32
{
    auto debugType = "UNKNOWN";
    if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0U)
    {
        debugType = "ERROR";
    }
    else if ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0U)
    {
        debugType = "WARNING";
    }
    else if ((flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0U)
    {
        debugType = "INFO";
    }
    else if ((flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) != 0U)
    {
        debugType = "PERFORMANCE WARNING";
    }
    else if ((flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0U)
    {
        debugType = "DEBUG";
    }

    std::cout << std::format("[Vulkan DebugLayer, {}, {}, {}]: {}", debugType, pLayerPrefix, messageCode, pMessage) << '\n';
    std::cout.flush();
    return VK_FALSE;
}
inline auto InitializeDebugLayerCallback(SVkInstance& instance) -> VkResult
{
    if (const auto pCreateCallback = std::bit_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance.instance, "vkCreateDebugReportCallbackEXT")))
    {
        constexpr VkDebugReportCallbackCreateInfoEXT debugReportCi{
            .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext       = nullptr,
            .flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
            .pfnCallback = DebugReport,
            .pUserData   = nullptr,
        };
        if (pCreateCallback(instance.instance, &debugReportCi, nullptr, &instance.debugMessenger) == VK_SUCCESS)
        {
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}
#endif

inline auto InitializeInstance(SVkInstance& instance, const char* pAppName, const uint32_t appVersion, const char* const* pExtensions, const uint32_t extensionsCount) noexcept -> VkResult
{
#ifdef _DEBUG
    constexpr std::array layers{"VK_LAYER_KHRONOS_validation"};
    std::vector extensions{VK_KHR_SURFACE_EXTENSION_NAME, "VK_EXT_debug_report"};
#else
    constexpr std::array<const char*, 0> layers;
    std::vector extensions{VK_KHR_SURFACE_EXTENSION_NAME};
#endif
    for (uint32_t i{}; i < extensionsCount; ++i)
    {
        if (const char* const pExt{pExtensions[i]}; std::ranges::find_if(extensions,
                                                                         [pExt](const char* pExtension)
                                                                         {
                                                                             return std::strcmp(pExt, pExtension) == 0;
                                                                         })
            == extensions.cend())
        {
            extensions.emplace_back(pExt);
        }
    }

    const VkApplicationInfo applicationInfo{
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = VK_NULL_HANDLE,
        .pApplicationName   = pAppName,
        .applicationVersion = appVersion,
        .pEngineName        = "Balbino",
        .engineVersion      = VK_MAKE_VERSION(0, 2, 0),
        .apiVersion         = VK_API_VERSION_1_3,
    };
    VkInstanceCreateInfo createInfo{
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = VK_NULL_HANDLE,
        .flags                   = 0,
        .pApplicationInfo        = &applicationInfo,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.empty() ? VK_NULL_HANDLE : layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.empty() ? VK_NULL_HANDLE : extensions.data(),
    };
    VkResult result{vkCreateInstance(&createInfo, VK_NULL_HANDLE, &instance.instance)};
    if (result == VK_ERROR_LAYER_NOT_PRESENT)
    {
        createInfo.enabledLayerCount   = 0;
        createInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
        result                         = vkCreateInstance(&createInfo, VK_NULL_HANDLE, &instance.instance);
    }
    return result;
}


inline auto InitialzieInstance(SVkInstance& instance) -> int32_t
{
    uint32_t extensionCount{};
    if (char const* const* pExtensions{SDL_Vulkan_GetInstanceExtensions(&extensionCount)}; InitializeInstance(instance, "Deer", VK_MAKE_VERSION(0, 0, 0), pExtensions, extensionCount) != 0)
    {
        return -1;
    }
#ifdef _DEBUG
    if (InitializeDebugLayerCallback(instance) != 0)
    {
        return -1;
    }
#endif
    return 0;
}

inline void Cleanup(SVkInstance& instance)
{
#ifdef _DEBUG
    if (instance.debugMessenger != nullptr)
    {
        if (const auto pDestroyCallback{std::bit_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance.instance, "vkDestroyDebugReportCallbackEXT"))})
        {
            pDestroyCallback(instance.instance, instance.debugMessenger, nullptr);
        }
    }
#endif
    vkDestroyInstance(instance.instance, nullptr);
    instance.instance = VK_NULL_HANDLE;
}
} // namespace DeerVulkan