//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"

#include <array>
#include <vector>

namespace DeerVulkan
{
struct SPhysicalDeviceInfo final
{
    VkPhysicalDeviceProperties deviceProperties{};
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
    VkPhysicalDeviceFeatures deviceFeatures{};
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    VkPhysicalDevice device{VK_NULL_HANDLE};

    [[nodiscard]] constexpr auto GetMaxUsableSampleCount() const noexcept -> VkSampleCountFlagBits
    {
        const VkSampleCountFlags counts{deviceProperties.limits.framebufferColorSampleCounts & deviceProperties.limits.framebufferDepthSampleCounts};
        if ((counts & VK_SAMPLE_COUNT_64_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_32_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_16_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_8_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_4_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_2_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }
        return VK_SAMPLE_COUNT_1_BIT;
    }
};

struct SQueueFamily
{
    int32_t graphicsFamily{-1};
    int32_t presentFamily{-1};

    [[nodiscard]] constexpr auto IsComplete() const noexcept -> bool
    {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

class CVkDevice final
{
public:
    constexpr CVkDevice() = default;

    ~CVkDevice()
    {
        Cleanup();
    }

    CVkDevice(const CVkDevice&) = delete;
    CVkDevice(CVkDevice&&) = delete;
    auto operator=(const CVkDevice&) -> CVkDevice& = delete;
    auto operator=(CVkDevice&&) -> CVkDevice& = delete;


    void Cleanup() noexcept
    {
        if (m_device == VK_NULL_HANDLE)
        {
            return;
        }
        vkDestroyDescriptorPool(m_device, m_descriptorPool, VK_NULL_HANDLE);
        WaitIdle();
        vkDestroyDevice(m_device, VK_NULL_HANDLE);
        m_device = VK_NULL_HANDLE;
    }

    void WaitIdle() const noexcept
    {
        vkDeviceWaitIdle(m_device);
    }

    [[nodiscard]] constexpr auto Initialize(const std::vector<const char*>& layersToEnable, const std::vector<const char*>& extensionsToEnable, const SQueueFamily& queueFamily,
                                            const SPhysicalDeviceInfo& physicalDeviceInfo) noexcept -> int32_t
    {
        m_deviceInfo = physicalDeviceInfo;
        VkPhysicalDeviceShaderObjectFeaturesEXT enabledShaderObjectFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT,
            .pNext = VK_NULL_HANDLE,
            .shaderObject = VK_TRUE,
        };
        VkPhysicalDeviceSynchronization2FeaturesKHR synchronization2Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
            .pNext = &enabledShaderObjectFeatures,
            .synchronization2 = VK_TRUE,
        };
        VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timelineSemaphoreFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR,
            .pNext = &synchronization2Features,
            .timelineSemaphore = VK_TRUE,
        };
        const VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
            .pNext = &timelineSemaphoreFeatures,
            .dynamicRendering = VK_TRUE,
        };

        constexpr VkPhysicalDeviceFeatures deviceFeatures{
            .robustBufferAccess = VK_FALSE,
            .fullDrawIndexUint32 = VK_FALSE,
            .imageCubeArray = VK_FALSE,
            .independentBlend = VK_FALSE,
            .geometryShader = VK_FALSE,
            .tessellationShader = VK_FALSE,
            .sampleRateShading = VK_TRUE,
            .dualSrcBlend = VK_FALSE,
            .logicOp = VK_FALSE,
            .multiDrawIndirect = VK_FALSE,
            .drawIndirectFirstInstance = VK_FALSE,
            .depthClamp = VK_FALSE,
            .depthBiasClamp = VK_FALSE,
            .fillModeNonSolid = VK_FALSE,
            .depthBounds = VK_FALSE,
            .wideLines = VK_FALSE,
            .largePoints = VK_FALSE,
            .alphaToOne = VK_FALSE,
            .multiViewport = VK_FALSE,
            .samplerAnisotropy = VK_TRUE,
            .textureCompressionETC2 = VK_FALSE,
            .textureCompressionASTC_LDR = VK_FALSE,
            .textureCompressionBC = VK_FALSE,
            .occlusionQueryPrecise = VK_FALSE,
            .pipelineStatisticsQuery = VK_FALSE,
            .vertexPipelineStoresAndAtomics = VK_FALSE,
            .fragmentStoresAndAtomics = VK_FALSE,
            .shaderTessellationAndGeometryPointSize = VK_FALSE,
            .shaderImageGatherExtended = VK_FALSE,
            .shaderStorageImageExtendedFormats = VK_FALSE,
            .shaderStorageImageMultisample = VK_FALSE,
            .shaderStorageImageReadWithoutFormat = VK_FALSE,
            .shaderStorageImageWriteWithoutFormat = VK_FALSE,
            .shaderUniformBufferArrayDynamicIndexing = VK_FALSE,
            .shaderSampledImageArrayDynamicIndexing = VK_FALSE,
            .shaderStorageBufferArrayDynamicIndexing = VK_FALSE,
            .shaderStorageImageArrayDynamicIndexing = VK_FALSE,
            .shaderClipDistance = VK_FALSE,
            .shaderCullDistance = VK_FALSE,
            .shaderFloat64 = VK_FALSE,
            .shaderInt64 = VK_FALSE,
            .shaderInt16 = VK_FALSE,
            .shaderResourceResidency = VK_FALSE,
            .shaderResourceMinLod = VK_FALSE,
            .sparseBinding = VK_FALSE,
            .sparseResidencyBuffer = VK_FALSE,
            .sparseResidencyImage2D = VK_FALSE,
            .sparseResidencyImage3D = VK_FALSE,
            .sparseResidency2Samples = VK_FALSE,
            .sparseResidency4Samples = VK_FALSE,
            .sparseResidency8Samples = VK_FALSE,
            .sparseResidency16Samples = VK_FALSE,
            .sparseResidencyAliased = VK_FALSE,
            .variableMultisampleRate = VK_FALSE,
            .inheritedQueries = VK_FALSE,
        };
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::vector queueFamilies{queueFamily.graphicsFamily};
        std::vector<std::vector<float>> queuePriorties{{1.0F, 1.0F}};
        if (queueFamily.graphicsFamily != queueFamily.presentFamily)
        {
            queueFamilies.emplace_back(queueFamily.presentFamily);
            queuePriorties.push_back({1.0F});
        }
        else
        {
            queuePriorties.back().emplace_back(1.0F);
        }
        for (int i = 0; i < queueFamilies.size(); i++)
        {
            // Create a graphics queue
            VkDeviceQueueCreateInfo queue_create_info = {};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queueFamily.graphicsFamily;
            queue_create_info.queueCount = static_cast<uint32_t>(queuePriorties[i].size());

            queue_create_info.pQueuePriorities = queuePriorties[i].data();
            queueCreateInfos.push_back(queue_create_info);
        }
        const VkDeviceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &dynamicRenderingFeature,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.empty() ? VK_NULL_HANDLE : queueCreateInfos.data(),

            .enabledLayerCount = static_cast<uint32_t>(layersToEnable.size()),
            .ppEnabledLayerNames = layersToEnable.data(),

            .enabledExtensionCount = static_cast<uint32_t>(extensionsToEnable.size()),
            .ppEnabledExtensionNames = extensionsToEnable.data(),

            .pEnabledFeatures = &deviceFeatures,
        };
        if (!CheckVkResult(vkCreateDevice(m_deviceInfo.device, &createInfo, VK_NULL_HANDLE, &m_device), "Could not create a device"))
        {
            return -1;
        }

        // todo get actual values from file/cook actual values into file
        constexpr uint32_t poolSize{1024U};
        std::array poolSizes{
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, .descriptorCount = poolSize},
            VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = poolSize},
        };
        const VkDescriptorPoolCreateInfo descriptorPoolInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .maxSets = 64,
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes = poolSizes.data(),
        };
        if (!CheckVkResult(vkCreateDescriptorPool(m_device, &descriptorPoolInfo, VK_NULL_HANDLE, &m_descriptorPool)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr auto Device() const noexcept -> const VkDevice&
    {
        return m_device;
    }

    [[nodiscard]] constexpr auto PhysicalDeviceInfo() const noexcept -> const SPhysicalDeviceInfo&
    {
        return m_deviceInfo;
    }

    [[nodiscard]] constexpr auto DescriptorPool() const noexcept -> VkDescriptorPool
    {
        return m_descriptorPool;
    }

private:
    SPhysicalDeviceInfo m_deviceInfo{};
    VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
};

} // namespace DeerVulkan