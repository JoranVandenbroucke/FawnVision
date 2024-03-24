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
static auto VKAPI_CALL DebugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT /*unused*/, uint64_t /*unused*/, size_t /*unused*/, int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
                                   void* /*unused*/) BALBINO_NOEXCEPT_SINCE_CXX11->VkBool32
{
    const char* debugType{(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0U                     ? "ERROR"
                              : (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0U             ? "WARNING"
                              : (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0U         ? "INFO"
                              : (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) != 0U ? "PERFORMANCE WARNING"
                              : (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0U               ? "DEBUG"
                                                                                            : "UNKNOWN"};

    std::cout << std::format("[Vulkan DebugLayer, {}, {}, {}]: {}", debugType, pLayerPrefix, messageCode, pMessage) << '\n';
    std::cout.flush();
    return VK_FALSE;
}
#endif //_DEBUG && !BL_EDITOR

class CVkInstance final
{
  public:
    BALBINO_CONSTEXPR_SINCE_CXX20 CVkInstance() = default;
    BALBINO_CONSTEXPR_SINCE_CXX20 ~CVkInstance();
    CVkInstance(const CVkInstance&)                    = delete;
    CVkInstance(CVkInstance&&)                         = delete;
    auto operator=(const CVkInstance&) -> CVkInstance& = delete;
    auto operator=(CVkInstance&&) -> CVkInstance&      = delete;

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Initialize(const char* pName, uint32_t appVersion, const char* const* pExtensions, uint32_t extensionsCount) BALBINO_NOEXCEPT_SINCE_CXX11->int32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto CreateDevice(CVkDevice*& device, const SQueueFamily& physicalDeviceIndex) const BALBINO_NOEXCEPT_SINCE_CXX11->int32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetPhysicalDeviceCount() const BALBINO_NOEXCEPT_SINCE_CXX11->size_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto FindBestPhysicalDeviceIndex(const CVkSurface* pSurface) BALBINO_NOEXCEPT_SINCE_CXX11->SQueueFamily;
    BALBINO_CONSTEXPR_SINCE_CXX23 void FreeSurface(const CVkSurface* pSurface) const BALBINO_NOEXCEPT_SINCE_CXX11;

  private:
    std::vector<VkLayerProperties> m_layerProperties;
    std::vector<VkExtensionProperties> m_extensionProperties;
    std::vector<SPhysicalDeviceInfo> m_physicalDevices;

    VkInstance m_instanceHandle{VK_NULL_HANDLE};
#ifdef _DEBUG
    const std::vector<const char*> m_instanceLayers{"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> m_physicalDeviceLayers{"VK_LAYER_KHRONOS_validation"};
    std::vector<const char*> m_instanceExtensions{VK_KHR_SURFACE_EXTENSION_NAME, "VK_EXT_debug_report"};
    std::vector<const char*> m_physicalDeviceExtensions{VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME, VK_EXT_SHADER_OBJECT_EXTENSION_NAME, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
                                                        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,      VK_KHR_SWAPCHAIN_EXTENSION_NAME,     VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
                                                        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME};
    VkDebugReportCallbackEXT m_debugReportCallback{VK_NULL_HANDLE};
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto InitializeDebugLayerCallback() BALBINO_NOEXCEPT_SINCE_CXX11 -> VkResult;
#else
    const std::vector<const char*> m_instanceLayers;
    const std::vector<const char*> m_physicalDeviceLayers;
    std::vector<const char*> m_instanceExtensions{VK_KHR_SURFACE_EXTENSION_NAME};
    std::vector<const char*> m_physicalDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_DEBUG_MARKER_EXTENSION_NAME, "VK_KHR_dynamic_rendering"};
#endif

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto InitializeInstance(const char* pAppName, uint32_t appVersion, const char* pEngineName, uint32_t engineVersion) BALBINO_NOEXCEPT_SINCE_CXX11->VkResult;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto InitializePhysicalDeviceInfos() BALBINO_NOEXCEPT_SINCE_CXX11->VkResult;

    static BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto SupportsLayers(const std::array<const char*, 1>& layers) BALBINO_NOEXCEPT_SINCE_CXX11->bool;
};
BALBINO_CONSTEXPR_SINCE_CXX20 CVkInstance::~CVkInstance()
{
#if defined(_DEBUG)
    if (m_debugReportCallback != nullptr)
    {
        if (const auto pDestroyCallback{std::bit_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instanceHandle, "vkDestroyDebugReportCallbackEXT"))})
        {
            pDestroyCallback(m_instanceHandle, m_debugReportCallback, nullptr);
        }
    }
#endif
    vkDestroyInstance(m_instanceHandle, VK_NULL_HANDLE);
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkInstance::Initialize(const char* pName, const uint32_t appVersion, const char* const* pExtensions, const uint32_t extensionsCount) BALBINO_NOEXCEPT_SINCE_CXX11 -> int32_t
{
    if (m_instanceHandle != nullptr)
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
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkInstance::CreateDevice(CVkDevice*& device, const SQueueFamily& physicalDeviceIndex) const BALBINO_NOEXCEPT_SINCE_CXX11 -> int32_t
{
    const SPhysicalDeviceInfo& info{m_physicalDevices[physicalDeviceIndex.presentFamily]};
    auto* pTemp{new CVkDevice{info}};
    if (pTemp->Initialize(m_physicalDeviceLayers, m_physicalDeviceExtensions, physicalDeviceIndex) != 0)
    {
        return -1;
    }
    device = pTemp;
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkInstance::GetPhysicalDeviceCount() const BALBINO_NOEXCEPT_SINCE_CXX11 -> size_t
{
    return m_physicalDevices.size();
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkInstance::FindBestPhysicalDeviceIndex(const CVkSurface* pSurface) BALBINO_NOEXCEPT_SINCE_CXX11 -> SQueueFamily
{
    for (auto& [deviceProperties, deviceMemoryProperties, deviceFeatures, queueFamilyProperties, device] : m_physicalDevices)
    {
        SQueueFamily queueFamily{};
        int32_t index{};
        for (const auto& [queueFlags, queueCount, timestampValidBits, minImageTransferGranularity] : queueFamilyProperties)
        {
            if (queueCount > 0 && (queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u)
            {
                const bool supportCompute{(queueFlags & VK_QUEUE_COMPUTE_BIT) != 0};
                const bool enoughSize{queueCount >= 2};

                if (supportCompute && enoughSize)
                {
                    queueFamily.graphicsFamily = index;
                }
            }

            VkBool32 presentSupport{};
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, pSurface->Handle(), &presentSupport);
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
BALBINO_CONSTEXPR_SINCE_CXX23 void CVkInstance::FreeSurface(const CVkSurface* pSurface) const BALBINO_NOEXCEPT_SINCE_CXX11
{
    vkDestroySurfaceKHR(m_instanceHandle, pSurface->Handle(), VK_NULL_HANDLE);
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkInstance::InitializeDebugLayerCallback() BALBINO_NOEXCEPT_SINCE_CXX11 -> VkResult
{
    if (const auto pCreateCallback = std::bit_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instanceHandle, "vkCreateDebugReportCallbackEXT")))
    {
        constexpr VkDebugReportCallbackCreateInfoEXT debugReportCi{
            .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
            .pfnCallback = DebugReport,
            .pUserData   = BALBINO_NULL,
        };
        if (pCreateCallback(m_instanceHandle, &debugReportCi, nullptr, &m_debugReportCallback) == VK_SUCCESS)
        {
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkInstance::InitializeInstance(const char* pAppName, const uint32_t appVersion, const char* pEngineName, const uint32_t engineVersion) BALBINO_NOEXCEPT_SINCE_CXX11 -> VkResult
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
    VkResult result{vkCreateInstance(&createInfo, VK_NULL_HANDLE, &m_instanceHandle)};
    if (result == VK_ERROR_LAYER_NOT_PRESENT)
    {
        createInfo.enabledLayerCount   = 0;
        createInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
        result                         = vkCreateInstance(&createInfo, VK_NULL_HANDLE, &m_instanceHandle);
    }
    return result;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkInstance::InitializePhysicalDeviceInfos() BALBINO_NOEXCEPT_SINCE_CXX11 -> VkResult
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
            .device                = devices[i],
        };
        vkGetPhysicalDeviceProperties(devices[i], &deviceInfo.deviceProperties);
        vkGetPhysicalDeviceMemoryProperties(devices[i], &deviceInfo.deviceMemoryProperties);
        vkGetPhysicalDeviceFeatures(devices[i], &deviceInfo.deviceFeatures);

        m_physicalDevices.emplace_back(deviceInfo);
    }
    return VK_SUCCESS;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkInstance::SupportsLayers(const std::array<const char*, 1>& layers) BALBINO_NOEXCEPT_SINCE_CXX11 -> bool
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
} // namespace DeerVulkan