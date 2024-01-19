//
// Created by joran on 02/01/2024.
//

#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

#include "VkSurface.h"

namespace DeerVulkan
{
    class CVkSurface;
    class CVkDevice;

    struct SPhysicalDeviceInfo final
    {
        VkPhysicalDeviceProperties deviceProperties {};
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties {};
        VkPhysicalDeviceFeatures deviceFeatures {};
        std::vector<VkQueueFamilyProperties> queueFamilyProperties {};
        VkPhysicalDevice device { VK_NULL_HANDLE };

        [[nodiscard]] VkSampleCountFlagBits GetMaxUsableSampleCount() const noexcept        {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties( device, &physicalDeviceProperties );

            const VkSampleCountFlags counts { physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts };
            if ( counts & VK_SAMPLE_COUNT_64_BIT )
            {
                return VK_SAMPLE_COUNT_64_BIT;
            }
            if ( counts & VK_SAMPLE_COUNT_32_BIT )
            {
                return VK_SAMPLE_COUNT_32_BIT;
            }
            if ( counts & VK_SAMPLE_COUNT_16_BIT )
            {
                return VK_SAMPLE_COUNT_16_BIT;
            }
            if ( counts & VK_SAMPLE_COUNT_8_BIT )
            {
                return VK_SAMPLE_COUNT_8_BIT;
            }
            if ( counts & VK_SAMPLE_COUNT_4_BIT )
            {
                return VK_SAMPLE_COUNT_4_BIT;
            }
            if ( counts & VK_SAMPLE_COUNT_2_BIT )
            {
                return VK_SAMPLE_COUNT_2_BIT;
            }
            return VK_SAMPLE_COUNT_1_BIT;
        }
    };

    class CInstanceHolder
    {
    public:
        CInstanceHolder() = default;

        ~CInstanceHolder()
        {
            if ( m_instanceHandle != VK_NULL_HANDLE )
            {
                vkDestroyInstance( m_instanceHandle, VK_NULL_HANDLE );
            }
        }

        [[nodiscard]] const VkInstance& GetHandle() const noexcept        {
            return m_instanceHandle;
        }

    protected:
        VkInstance m_instanceHandle { VK_NULL_HANDLE };
    };

    class CVkInstance : public CInstanceHolder
    {
    public:
        CVkInstance()
            : CInstanceHolder {}
        {}

        ~CVkInstance();
        [[nodiscard]] int32_t Initialize( const char* name, uint32_t appVersion, const char* const* extensions, uint32_t extensionsCount );
        [[nodiscard]] int32_t CreateDevice( CVkDevice*& device, size_t physicalDeviceIndex ) const noexcept;

        [[nodiscard]] size_t GetPhysicalDeviceCount() const noexcept
        {
            return m_physicalDevices.size();
        }

        int32_t FindBestPhysicalDeviceIndex( const CVkSurface* surface );
        void FreeSurface( const CVkSurface* surface ) const noexcept
        {
            vkDestroySurfaceKHR( m_instanceHandle, surface->GetHandle(), VK_NULL_HANDLE );
        };

    private:
        std::vector<VkLayerProperties> m_layerProperties;
        std::vector<VkExtensionProperties> m_extensionProperties;
        std::vector<SPhysicalDeviceInfo> m_physicalDevices;

#ifdef _DEBUG
        const std::vector<const char*> m_instanceLayers { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> m_physicalDeviceLayers { "VK_LAYER_KHRONOS_validation" };
        std::vector<const char*> m_instanceExtensions { VK_KHR_SURFACE_EXTENSION_NAME, "VK_EXT_debug_report" };
        std::vector<const char*> m_physicalDeviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_dynamic_rendering" };
        VkDebugReportCallbackEXT m_debugReportCallback { VK_NULL_HANDLE };
        VkResult InitializeDebugLayerCallback();
#else
        const std::vector<const char*> enabledInstanceLayers {};
        const std::vector<const char*> enabledPhysicalDeviceLayers {};
        std::vector<const char*> enabledInstanceExtensions { VK_KHR_SURFACE_EXTENSION_NAME };
        std::vector<const char*> m_deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_DEBUG_MARKER_EXTENSION_NAME, "VK_KHR_dynamic_rendering" };
#endif

        VkResult InitializeInstance( const char* appName, uint32_t appVersion, const char* engineName, uint32_t engineVersion );
        VkResult InitializePhysicalDeviceInfos();

        static bool SupportsLayers( const std::array<const char*, 1>& layers );
    };
}// namespace DeerVulkan
