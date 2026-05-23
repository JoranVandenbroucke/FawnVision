#pragma once
#include "../deer_vulkan_core.hpp"
#include "dispatch.hpp"

#include "headers/platform.hpp"

namespace deer_vulkan
{
struct Instance
{
    vk::Instance instance{nullptr};

#if defined(BALBINO_DEBUG)
    vk::DebugUtilsMessengerEXT debugUtilsMessenger{nullptr};
#endif
};

#if defined(BALBINO_DEBUG)
vk::Bool32 DebugMessageFunc(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, vk::DebugUtilsMessengerCallbackDataEXT const*, void*);
#endif

#if defined(BALBINO_OS_LINUX) || defined(BALBINO_OS_UNIXENVIRONMENT)
enum class LinuxDisplayServer : std::uint8_t
{
#    if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    Wayland,
#    endif
#    if defined(VK_USE_PLATFORM_XCB_KHR)
    Xcb,
#    endif
#    if defined(VK_USE_PLATFORM_XLIB_KHR)
    Xlib,
#    endif
    Unknown
};

[[nodiscard]] constexpr auto DetectLinuxDisplayServer() noexcept -> LinuxDisplayServer
{
#    if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    if (std::getenv("WAYLAND_DISPLAY") != nullptr)
    {
        return LinuxDisplayServer::Wayland;
    }
#    endif
#    if defined(VK_USE_PLATFORM_XCB_KHR)
    if (std::getenv("DISPLAY") != nullptr)
    {
        return LinuxDisplayServer::Xcb;
    }
#    endif
#    if defined(VK_USE_PLATFORM_XLIB_KHR)
    if (std::getenv("DISPLAY") != nullptr)
    {
        return LinuxDisplayServer::Xlib;
    }
#    endif
    return LinuxDisplayServer::Unknown;
}
#endif // BALBINO_OS_LINUX || BALBINO_OS_UNIXENVIRONMENT

[[nodiscard]] inline auto Initialize(Dispatch& dispatch, Instance& instance) noexcept -> vk_status
{
    try
    {
        // --- Build required extension list ---
        std::vector enabledExtensions{vk::KHRSurfaceExtensionName};

#if defined(VK_USE_PLATFORM_WIN32_KHR)
        enabledExtensions.push_back(vk::KHRWin32SurfaceExtensionName);
#endif
#if defined(VK_USE_PLATFORM_MACOS_MVK)
        enabledExtensions.push_back(vk::MVKMacosSurfaceExtensionName);
#endif
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        enabledExtensions.push_back(vk::KHRAndroidSurfaceExtensionName);
#endif

        switch (DetectLinuxDisplayServer())
        {
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
        case LinuxDisplayServer::Wayland: enabledExtensions.push_back(vk::KHRWaylandSurfaceExtensionName); break;
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
        case LinuxDisplayServer::Xcb: enabledExtensions.push_back(vk::KHRXcbSurfaceExtensionName); break;
#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
        case LinuxDisplayServer::Xlib: enabledExtensions.push_back(vk::KHRXlibSurfaceExtensionName); break;
#endif
        case LinuxDisplayServer::Unknown: break;
        }

#if defined(BALBINO_DEBUG)
        {
            if (const std::vector<vk::ExtensionProperties> extensionProperties = vk::enumerateInstanceExtensionProperties(nullptr, dispatch.dispatch);
                std::ranges::none_of(extensionProperties,
                                     [](const vk::ExtensionProperties& ep)
                                     {
                                         return std::strcmp(ep.extensionName.data(), vk::EXTDebugUtilsExtensionName) == 0;
                                     }))
            {
                std::println("[WARN] Cannot find {} extension, debug messenger unavailable.", vk::EXTDebugUtilsExtensionName);
            }
            else
            {
                enabledExtensions.push_back(vk::EXTDebugUtilsExtensionName);
            }
        }
#endif
        // todo: get this from config file
        vk::ApplicationInfo applicationInfo{
            .sType              = vk::StructureType::eApplicationInfo,
            .pNext              = nullptr,
            .pApplicationName   = "Deer",
            .applicationVersion = vk::makeVersion(0, 0, 1),
            .pEngineName        = "Balbino",
            .engineVersion      = vk::makeVersion(0, 3, 0),
            .apiVersion         = vk::ApiVersion14,
        };
        // Instead of ppEnabledLayerNames, use VkValidationFeaturesEXT in pNext chain
        constexpr std::array enables{
            vk::ValidationFeatureEnableEXT::eBestPractices,
            vk::ValidationFeatureEnableEXT::eDebugPrintf,
            vk::ValidationFeatureEnableEXT::eSynchronizationValidation,
        };

        const vk::ValidationFeaturesEXT validationFeatures{
            .sType                          = vk::StructureType::eValidationFeaturesEXT,
            .pNext                          = nullptr,
            .enabledValidationFeatureCount  = static_cast<uint32_t>(enables.size()),
            .pEnabledValidationFeatures     = enables.data(),
            .disabledValidationFeatureCount = 0,
            .pDisabledValidationFeatures    = nullptr,
        };
        const vk::InstanceCreateInfo createInfo{
            .sType                   = vk::StructureType::eInstanceCreateInfo,
            .pNext                   = &validationFeatures,
            .flags                   = {},
            .pApplicationInfo        = &applicationInfo,
            .enabledLayerCount       = 0,
            .ppEnabledLayerNames     = nullptr,
            .enabledExtensionCount   = static_cast<std::uint32_t>(enabledExtensions.size()),
            .ppEnabledExtensionNames = enabledExtensions.data(),
        };

        instance.instance = vk::createInstance(createInfo, nullptr, dispatch.dispatch);
        dispatch.dispatch.init(instance.instance);

#if defined(BALBINO_DEBUG)
        constexpr vk::DebugUtilsMessageSeverityFlagsEXT severityFlags{vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                                                                      | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError};
        constexpr vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags{vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                                                                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
                                                                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation};

        constexpr vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{
            .sType           = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT,
            .pNext           = nullptr,
            .flags           = {},
            .messageSeverity = severityFlags,
            .messageType     = messageTypeFlags,
            .pfnUserCallback = &DebugMessageFunc,
            .pUserData       = nullptr,

        };
        instance.debugUtilsMessenger = instance.instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCI, nullptr, dispatch.dispatch);
#endif
    }
    catch (vk::SystemError const& err)
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        return vk_status::unknown;
    }
    return vk_status::ok;
}

inline auto Cleanup(const Dispatch& dispatch, Instance& instance) noexcept -> void
{
#if defined(BALBINO_DEBUG)
    instance.instance.destroyDebugUtilsMessengerEXT(instance.debugUtilsMessenger, nullptr, dispatch.dispatch);
    instance.debugUtilsMessenger = nullptr;
#endif

    instance.instance.destroy(nullptr, dispatch.dispatch);
    instance.instance = nullptr;
}
} // namespace deer_vulkan
