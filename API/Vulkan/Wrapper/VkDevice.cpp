//
// Created by joran on 02/01/2024.
//

#include "VkDevice.h"
#include "../DeerVulkan_Core.h"

namespace DeerVulkan
{
    int32_t CVkDevice::Initialize( SPhysicalDeviceInfo deviceInfo, const std::vector<const char*>& layersToEnable, const std::vector<const char*>& extensionsToEnable )
    {
        m_deviceInfo = std::move( deviceInfo );

        constexpr float queuePriority{1.0f};
        VkDeviceQueueCreateInfo queueCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = 0,
                    .queueCount = 1,
                    .pQueuePriorities = &queuePriority,
        };
        constexpr VkPhysicalDeviceFeatures deviceFeatures{
                .sampleRateShading = VK_TRUE,
                .samplerAnisotropy = VK_TRUE,
        };
        constexpr VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                .dynamicRendering = VK_TRUE,
        };
        const VkDeviceCreateInfo createInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = &dynamicRenderingFeature,

                .queueCreateInfoCount = 1,
                .pQueueCreateInfos = &queueCreateInfo,

                .enabledLayerCount = static_cast<uint32_t>(layersToEnable.size()),
                .ppEnabledLayerNames = layersToEnable.data(),

                .enabledExtensionCount = static_cast<uint32_t>(extensionsToEnable.size()),
                .ppEnabledExtensionNames = extensionsToEnable.data(),

                .pEnabledFeatures = &deviceFeatures,
        };
        if ( !CheckVkResult( vkCreateDevice( m_deviceInfo.device, &createInfo, VK_NULL_HANDLE, &m_device ), "Could not create a device" ) )
        {
            return -1;
        }
        return 0;
    }
}// namespace DeerVulkan
