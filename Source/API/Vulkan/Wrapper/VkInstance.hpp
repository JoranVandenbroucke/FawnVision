//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"
#include "VkDevice.hpp"
#include "VkSurface.hpp"

#include <bit>
#include <set>
#include <vector>

namespace DeerVulkan
{
#if defined(_DEBUG)
static auto VKAPI_CALL DebugReport(const VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT /*unused*/, uint64_t /*unused*/, size_t /*unused*/, int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
                                   void* /*unused*/) noexcept -> VkBool32
{
    const char* pDebugType{(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0U                     ? "ERROR"
                               : (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0U             ? "WARNING"
                               : (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0U         ? "INFO"
                               : (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) != 0U ? "PERFORMANCE WARNING"
                               : (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0U               ? "DEBUG"
                                                                                             : "UNKNOWN"};

    std::cout << std::format("[Vulkan DebugLayer, {}, {}, {}]: {}", pDebugType, pLayerPrefix, messageCode, pMessage) << '\n';
    std::cout.flush();
    return VK_FALSE;
}
#endif //_DEBUG && !BL_EDITOR

class CVkInstance final
{
  public:
    constexpr CVkInstance() = default;

    ~CVkInstance()
    {
#if defined(_DEBUG)
        if (m_debugReportCallback != nullptr)
        {
            if (const auto pDestroyCallback{std::bit_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"))})
            {
                pDestroyCallback(m_instance, m_debugReportCallback, nullptr);
            }
        }
#endif
        vkDestroyInstance(m_instance, VK_NULL_HANDLE);
    }

    CVkInstance(const CVkInstance&)                    = delete;
    CVkInstance(CVkInstance&&)                         = delete;
    auto operator=(const CVkInstance&) -> CVkInstance& = delete;
    auto operator=(CVkInstance&&) -> CVkInstance&      = delete;
    [[nodiscard]] constexpr auto Handle() const noexcept -> const VkInstance&
    {
        return m_instance;
    }

    [[nodiscard]] constexpr auto Initialize(const char* pName, const uint32_t appVersion, const char* const* pExtensions, const uint32_t extensionsCount) noexcept -> int32_t
    {
        if (m_instance != nullptr)
        {
            return -1;
        }
        for (uint32_t i{}; i < extensionsCount; ++i)
        {
            if (const char* const pExt{pExtensions[i]}; std::ranges::find_if(m_instanceExtensions,
                                                                             [pExt](const char* pExtension) -> bool
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
        if (!CheckVkResult(InitializeDebugLayerCallback()))
        {
            return -1;
        }
#endif
        if (!CheckVkResult(InitializePhysicalDeviceInfos()))
        {
            return -1;
        }

        return 0;
    }

    [[nodiscard]] constexpr auto CreateDevice(CVkDevice& device, const SQueueFamily& physicalDeviceIndex) const noexcept -> int32_t
    {
        const SPhysicalDeviceInfo& info{m_physicalDevices[physicalDeviceIndex.presentFamily]};
        if (device.Initialize(m_physicalDeviceLayers, m_physicalDeviceExtensions, physicalDeviceIndex, info) != 0)
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr auto GetPhysicalDeviceCount() const noexcept -> size_t
    {
        return m_physicalDevices.size();
    }

    [[nodiscard]] constexpr auto FindBestPhysicalDeviceIndex(const CVkSurface& surface) noexcept -> SQueueFamily
    {
        for (auto& [deviceProperties, deviceMemoryProperties, deviceFeatures, queueFamilyProperties, device] : m_physicalDevices)
        {
            SQueueFamily queueFamily{};
            int32_t index{};
            for (const auto& [queueFlags, queueCount, timestampValidBits, minImageTransferGranularity] : queueFamilyProperties)
            {
                if (queueCount > 0 && (queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0U)
                {
                    const bool supportCompute{(queueFlags & VK_QUEUE_COMPUTE_BIT) != 0U};
                    const bool enoughSize{queueCount >= 2};

                    if (supportCompute && enoughSize)
                    {
                        queueFamily.graphicsFamily = index;
                    }
                }

                VkBool32 presentSupport{};
                vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface.Surface(), &presentSupport);
                if (queueCount > 0 && presentSupport == VK_TRUE)
                {
                    queueFamily.presentFamily = index;
                }
                if (queueFamily.IsComplete())
                {
                    return queueFamily;
                }
                ++index;
            }
        }
        return {};
    }

    void FreeSurface(const CVkSurface& pSurface) const noexcept
    {
        vkDestroySurfaceKHR(m_instance, pSurface.Surface(), VK_NULL_HANDLE);
    }

  private:
    std::vector<VkLayerProperties> m_layerProperties;
    std::vector<VkExtensionProperties> m_extensionProperties;
    std::vector<SPhysicalDeviceInfo> m_physicalDevices;

    VkInstance m_instance{VK_NULL_HANDLE};
    const std::vector<const char*> m_instanceLayers{
#ifdef _DEBUG
        "VK_LAYER_KHRONOS_validation"
#endif
    };
    const std::vector<const char*> m_physicalDeviceLayers{
#ifdef _DEBUG
        "VK_LAYER_KHRONOS_validation"
#endif
    };
    std::vector<const char*> m_instanceExtensions{
#ifdef _DEBUG
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
        VK_KHR_SURFACE_EXTENSION_NAME};
    std::vector<const char*> m_physicalDeviceExtensions{
        VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
        VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
        VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME
    };

#ifdef _DEBUG
    VkDebugReportCallbackEXT m_debugReportCallback{VK_NULL_HANDLE};
    [[nodiscard]] constexpr auto InitializeDebugLayerCallback() noexcept -> VkResult
    {
        if (const auto pCreateCallback = std::bit_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT")))
        {
            constexpr VkDebugReportCallbackCreateInfoEXT debugReportCi{
                .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                .flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
                .pfnCallback = DebugReport,
                .pUserData   = nullptr,
            };
            if (pCreateCallback(m_instance, &debugReportCi, nullptr, &m_debugReportCallback) == VK_SUCCESS)
            {
                return VK_SUCCESS;
            }
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }
#endif

    [[nodiscard]] auto InitializeInstance(const char* pAppName, const uint32_t appVersion, const char* pEngineName, const uint32_t engineVersion) noexcept -> VkResult
    {
        const VkApplicationInfo applicationInfo{
            .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext              = VK_NULL_HANDLE,
            .pApplicationName   = pAppName,
            .applicationVersion = appVersion,
            .pEngineName        = pEngineName,
            .engineVersion      = engineVersion,
            .apiVersion         = VK_API_VERSION_1_3,
        };
        VkInstanceCreateInfo createInfo{
            .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext                   = VK_NULL_HANDLE,
            .flags                   = 0,
            .pApplicationInfo        = &applicationInfo,
            .enabledLayerCount       = static_cast<uint32_t>(m_instanceLayers.size()),
            .ppEnabledLayerNames     = m_instanceLayers.empty() ? VK_NULL_HANDLE : m_instanceLayers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(m_instanceExtensions.size()),
            .ppEnabledExtensionNames = m_instanceExtensions.empty() ? VK_NULL_HANDLE : m_instanceExtensions.data(),
        };
        VkResult result{vkCreateInstance(&createInfo, VK_NULL_HANDLE, &m_instance)};
        if (result == VK_ERROR_LAYER_NOT_PRESENT)
        {
            createInfo.enabledLayerCount   = 0;
            createInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
            result                         = vkCreateInstance(&createInfo, VK_NULL_HANDLE, &m_instance);
        }
        return result;
    }

    [[nodiscard]] auto InitializePhysicalDeviceInfos() noexcept -> VkResult
    {
        uint32_t deviceCount{};
        VkResult result{vkEnumeratePhysicalDevices(m_instance, &deviceCount, VK_NULL_HANDLE)};
        if (result != VK_SUCCESS)
        {
            return result;
        }
        std::vector<VkPhysicalDevice> devices(deviceCount, nullptr);
        result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
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
                .device                = devices[i],
            };
            vkGetPhysicalDeviceProperties(devices[i], &deviceInfo.deviceProperties);
            vkGetPhysicalDeviceMemoryProperties(devices[i], &deviceInfo.deviceMemoryProperties);
            vkGetPhysicalDeviceFeatures(devices[i], &deviceInfo.deviceFeatures);

            m_physicalDevices.emplace_back(deviceInfo);
        }
        return VK_SUCCESS;
    }

    [[nodiscard]] static auto SupportsLayers(const std::array<const char*, 1>& layers) noexcept -> bool
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