//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include <set>
#include <vector>

#include "../DeerVulkan_Core.h"
#include "VkDevice.h"
#include "VkSurface.h"

namespace DeerVulkan
{
#if defined( _DEBUG )
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReport( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* )
    {
        const char* debugType { flags & VK_DEBUG_REPORT_ERROR_BIT_EXT                         ? "ERROR"
                                        : flags & VK_DEBUG_REPORT_WARNING_BIT_EXT             ? "WARNING"
                                        : flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT         ? "INFO"
                                        : flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ? "PERFORMANCE WARNING"
                                        : flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT               ? "DEBUG"
                                                                                              : "UNKNOWN" };

        std::cout << std::format( "[Vulkan DebugLayer, {}, {}, {}]: {}", debugType, pLayerPrefix, messageCode, pMessage ) << std::endl;
        return VK_FALSE;
    }
#endif//_DEBUG && !BL_EDITOR

class CInstanceHolder
{
public:
    CInstanceHolder() = default;

    ~CInstanceHolder()
    {
        if (m_instanceHandle != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_instanceHandle, VK_NULL_HANDLE);
        }
    }

    [[nodiscard]] auto Handle() const noexcept -> const VkInstance&
    {
        return m_instanceHandle;
    }

protected:
    VkInstance m_instanceHandle{VK_NULL_HANDLE};
};

class CVkInstance : public CInstanceHolder
{
public:
    CVkInstance() = default;

    ~CVkInstance()
    {
#if defined( _DEBUG )
            if ( m_debugReportCallback )
            {
                if ( const auto pDestroyCallback { reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>( vkGetInstanceProcAddr( m_instanceHandle, "vkDestroyDebugReportCallbackEXT" ) ) } )
                {
                    pDestroyCallback( m_instanceHandle, m_debugReportCallback, nullptr );
                }
            }
#endif
    }

    [[nodiscard]] auto Initialize(const char* pName, const uint32_t appVersion, const char* const* pExtensions, const uint32_t extensionsCount) -> int32_t
    {
        if (m_instanceHandle != nullptr)
        {
            return -1;
        }
        for (uint32_t i{}; i < extensionsCount; ++i)
        {
            if (const char* const pExt{pExtensions[i]}; std::ranges::find_if(m_instanceExtensions,
                                                                             [ pExt ](const char* pExtension)-> bool
                                                                             {
                                                                                 return std::strcmp(pExt, pExtension) == 0;
                                                                             })
                == m_instanceExtensions.cend())
            {
                m_instanceExtensions.emplace_back(pExt);
            }
        }

        if (!CheckVkResult(InitializeInstance(pName, appVersion, "BalbinoEngine", VK_MAKE_VERSION(0, 1, 0))))
        {
            return -1;
        }
#ifdef _DEBUG
            InitializeDebugLayerCallback();
#endif
        if (!CheckVkResult(InitializePhysicalDeviceInfos()))
        {
            return -1;
        }

        return 0;
    }

    [[nodiscard]] auto CreateDevice(CVkDevice*& device, const size_t physicalDeviceIndex) const noexcept -> int32_t
    {
        const SPhysicalDeviceInfo& info{m_physicalDevices[std::max(static_cast<size_t>(0), std::min(physicalDeviceIndex, m_physicalDevices.size()))]};
        auto* pTemp{new CVkDevice{}};
        if (pTemp->Initialize(info, m_physicalDeviceLayers, m_physicalDeviceExtensions) != 0)
        {
            return -1;
        }
        device = pTemp;
        return 0;
    }

    [[nodiscard]] auto GetPhysicalDeviceCount() const noexcept -> size_t
    {
        return m_physicalDevices.size();
    }

    auto FindBestPhysicalDeviceIndex(const CVkSurface* pSurface) -> int32_t
    {
        for (auto& [deviceProperties, deviceMemoryProperties, deviceFeatures, queueFamilyProperties, device] : m_physicalDevices)
        {
            for (uint32_t j = 0; j < static_cast<uint32_t>(queueFamilyProperties.size()); ++j)
            {
                VkBool32 supportsPresent{VK_FALSE};
                vkGetPhysicalDeviceSurfaceSupportKHR(device, j, pSurface->Handle(), &supportsPresent);
                if (supportsPresent != VK_FALSE && (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0U && deviceFeatures.samplerAnisotropy != 0U)
                {
                    return static_cast<int32_t>(j);
                }
            }
        }
        return -1;
    }

    void FreeSurface(const CVkSurface* pSurface) const noexcept
    {
        vkDestroySurfaceKHR(m_instanceHandle, pSurface->Handle(), VK_NULL_HANDLE);
    }

private:
    std::vector<VkLayerProperties> m_layerProperties;
    std::vector<VkExtensionProperties> m_extensionProperties;
    std::vector<SPhysicalDeviceInfo> m_physicalDevices;

#ifdef _DEBUG
        const std::vector<const char*> m_instanceLayers { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> m_physicalDeviceLayers { "VK_LAYER_KHRONOS_validation" };
        std::vector<const char*> m_instanceExtensions { VK_KHR_SURFACE_EXTENSION_NAME, "VK_EXT_debug_report" };
        std::vector<const char*> m_physicalDeviceExtensions {
            VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
            VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
            VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME
        };
        VkDebugReportCallbackEXT m_debugReportCallback { VK_NULL_HANDLE };
        VkResult InitializeDebugLayerCallback()
        {
            if ( const auto pCreateCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>( vkGetInstanceProcAddr( m_instanceHandle, "vkCreateDebugReportCallbackEXT" ) ) )
            {
                constexpr VkDebugReportCallbackCreateInfoEXT debugReportCi {
                        .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                        .flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
                        .pfnCallback = DebugReport,
                        .pUserData   = BALBINO_NULL,
                };
                if ( pCreateCallback( m_instanceHandle, &debugReportCi, nullptr, &m_debugReportCallback ) == VK_SUCCESS )
                {
                    return VK_SUCCESS;
                }
            }
            return VK_ERROR_INITIALIZATION_FAILED;
        }
#else
    const std::vector<const char*> m_instanceLayers;
    const std::vector<const char*> m_physicalDeviceLayers;
    std::vector<const char*> m_instanceExtensions{VK_KHR_SURFACE_EXTENSION_NAME};
    std::vector<const char*> m_physicalDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_DEBUG_MARKER_EXTENSION_NAME, "VK_KHR_dynamic_rendering"};
#endif

    auto InitializeInstance(const char* pAppName, const uint32_t appVersion, const char* pEngineName, const uint32_t engineVersion) -> VkResult
    {
        const VkApplicationInfo applicationInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = VK_NULL_HANDLE,
            .pApplicationName = pAppName,
            .applicationVersion = appVersion,
            .pEngineName = pEngineName,
            .engineVersion = engineVersion,
            .apiVersion = VK_API_VERSION_1_3,
        };
        VkInstanceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(m_instanceLayers.size()),
            .ppEnabledLayerNames = m_instanceLayers.empty() ? VK_NULL_HANDLE : m_instanceLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(m_instanceExtensions.size()),
            .ppEnabledExtensionNames = m_instanceExtensions.empty() ? VK_NULL_HANDLE : m_instanceExtensions.data(),
        };
        VkResult result{vkCreateInstance(&createInfo, VK_NULL_HANDLE, &m_instanceHandle)};
        if (result == VK_ERROR_LAYER_NOT_PRESENT)
        {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
            result = vkCreateInstance(&createInfo, VK_NULL_HANDLE, &m_instanceHandle);
        }
        return result;
    }

    auto InitializePhysicalDeviceInfos() -> VkResult
    {
        uint32_t deviceCount{};
        VkResult result{vkEnumeratePhysicalDevices(m_instanceHandle, &deviceCount, VK_NULL_HANDLE)};
        if (result != VK_SUCCESS)
        {
            return result;
        }
        std::vector<VkPhysicalDevice> devices(deviceCount, BALBINO_NULL);
        result = vkEnumeratePhysicalDevices(m_instanceHandle, &deviceCount, devices.data());
        if (result != VK_SUCCESS)
        {
            return result;
        }

        m_physicalDevices.reserve(deviceCount);
        for (uint32_t i{}; i < deviceCount; ++i)
        {
            uint32_t queueFamilyCount{};
            vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilies.data());

            uint32_t extensionCount{};
            result = vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);
            if (result != VK_SUCCESS)
            {
                return result;
            }
            std::vector<VkExtensionProperties> extensions(extensionCount);
            result = vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, extensions.data());
            if (result != VK_SUCCESS)
            {
                return result;
            }

            std::set<std::string> requiredExtensions(m_physicalDeviceExtensions.begin(), m_physicalDeviceExtensions.end());
            for (const auto& [extensionName, specVersion] : extensions)
            {
                requiredExtensions.erase(extensionName);
            }

            if (!requiredExtensions.empty())
            {
                continue;
            }

            SPhysicalDeviceInfo deviceInfo{
                .queueFamilyProperties = queueFamilies,
                .device = devices[i],
            };
            vkGetPhysicalDeviceProperties(devices[i], &deviceInfo.deviceProperties);
            vkGetPhysicalDeviceMemoryProperties(devices[i], &deviceInfo.deviceMemoryProperties);
            vkGetPhysicalDeviceFeatures(devices[i], &deviceInfo.deviceFeatures);

            m_physicalDevices.emplace_back(deviceInfo);
        }
        return VK_SUCCESS;
    }

    static auto SupportsLayers(const std::array<const char*, 1>& layers) -> bool
    {
        uint32_t layerCount{};
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const auto& [layerName, specVersion, implementationVersion, description] : availableLayers)
        {
            if (std::strcmp(layerName, layers[0]) == 0)
            {
                return true;
            }
        }

        return false;
    }
};
} // namespace DeerVulkan