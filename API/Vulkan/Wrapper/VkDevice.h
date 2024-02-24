//
// Created by joran on 02/01/2024.
//

#pragma once
#include <array>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Base.h"

namespace DeerVulkan
{
    struct SPhysicalDeviceInfo final
    {
        VkPhysicalDeviceProperties deviceProperties {};
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties {};
        VkPhysicalDeviceFeatures deviceFeatures {};
        std::vector<VkQueueFamilyProperties> queueFamilyProperties {};
        VkPhysicalDevice device { VK_NULL_HANDLE };

        [[nodiscard]] VkSampleCountFlagBits GetMaxUsableSampleCount() const noexcept
        {
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

    class CDeviceHolder
    {
      public:
        CDeviceHolder() = default;

        virtual ~CDeviceHolder()
        {
            if ( m_device != VK_NULL_HANDLE )
            {
                vkDestroyDevice( m_device, VK_NULL_HANDLE );
            }
        }

      protected:
        VkDevice m_device { VK_NULL_HANDLE };
    };

    class CVkDevice final : public CDeviceHolder, public CRefCounted
    {
      public:
        CVkDevice()
            : CDeviceHolder {}
            , CRefCounted {}
        {
        }

        ~CVkDevice() override
        {
            vkDestroyDescriptorPool( m_device, m_descriptorPool, VK_NULL_HANDLE );
            vkDeviceWaitIdle( m_device );
        }

        BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX14 int32_t Initialize( SPhysicalDeviceInfo deviceInfo, const std::vector<const char*>& layersToEnable, const std::vector<const char*>& extensionsToEnable )
        {
            m_deviceInfo = std::move( deviceInfo );
            VkPhysicalDeviceShaderObjectFeaturesEXT enabledShaderObjectFeatures {
                .sType        = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT,
                .pNext        = VK_NULL_HANDLE,
                .shaderObject = VK_TRUE,
            };
            VkPhysicalDeviceSynchronization2FeaturesKHR synchronization2Features {
                .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
                .pNext            = &enabledShaderObjectFeatures,
                .synchronization2 = VK_TRUE,
            };
            VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timelineSemaphoreFeatures {
                .sType             = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR,
                .pNext             = &synchronization2Features,
                .timelineSemaphore = VK_TRUE,
            };
            const VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature {
                .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                .pNext            = &timelineSemaphoreFeatures,
                .dynamicRendering = VK_TRUE,
            };

            const float queuePriority { 1.0f };
            VkDeviceQueueCreateInfo queueCreateInfo {
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext            = VK_NULL_HANDLE,
                .flags            = 0,
                .queueFamilyIndex = 0,
                .queueCount       = 1,
                .pQueuePriorities = &queuePriority,
            };
            const VkPhysicalDeviceFeatures deviceFeatures {
                .robustBufferAccess                      = VK_FALSE,
                .fullDrawIndexUint32                     = VK_FALSE,
                .imageCubeArray                          = VK_FALSE,
                .independentBlend                        = VK_FALSE,
                .geometryShader                          = VK_FALSE,
                .tessellationShader                      = VK_FALSE,
                .sampleRateShading                       = VK_TRUE,
                .dualSrcBlend                            = VK_FALSE,
                .logicOp                                 = VK_FALSE,
                .multiDrawIndirect                       = VK_FALSE,
                .drawIndirectFirstInstance               = VK_FALSE,
                .depthClamp                              = VK_FALSE,
                .depthBiasClamp                          = VK_FALSE,
                .fillModeNonSolid                        = VK_FALSE,
                .depthBounds                             = VK_FALSE,
                .wideLines                               = VK_FALSE,
                .largePoints                             = VK_FALSE,
                .alphaToOne                              = VK_FALSE,
                .multiViewport                           = VK_FALSE,
                .samplerAnisotropy                       = VK_TRUE,
                .textureCompressionETC2                  = VK_FALSE,
                .textureCompressionASTC_LDR              = VK_FALSE,
                .textureCompressionBC                    = VK_FALSE,
                .occlusionQueryPrecise                   = VK_FALSE,
                .pipelineStatisticsQuery                 = VK_FALSE,
                .vertexPipelineStoresAndAtomics          = VK_FALSE,
                .fragmentStoresAndAtomics                = VK_FALSE,
                .shaderTessellationAndGeometryPointSize  = VK_FALSE,
                .shaderImageGatherExtended               = VK_FALSE,
                .shaderStorageImageExtendedFormats       = VK_FALSE,
                .shaderStorageImageMultisample           = VK_FALSE,
                .shaderStorageImageReadWithoutFormat     = VK_FALSE,
                .shaderStorageImageWriteWithoutFormat    = VK_FALSE,
                .shaderUniformBufferArrayDynamicIndexing = VK_FALSE,
                .shaderSampledImageArrayDynamicIndexing  = VK_FALSE,
                .shaderStorageBufferArrayDynamicIndexing = VK_FALSE,
                .shaderStorageImageArrayDynamicIndexing  = VK_FALSE,
                .shaderClipDistance                      = VK_FALSE,
                .shaderCullDistance                      = VK_FALSE,
                .shaderFloat64                           = VK_FALSE,
                .shaderInt64                             = VK_FALSE,
                .shaderInt16                             = VK_FALSE,
                .shaderResourceResidency                 = VK_FALSE,
                .shaderResourceMinLod                    = VK_FALSE,
                .sparseBinding                           = VK_FALSE,
                .sparseResidencyBuffer                   = VK_FALSE,
                .sparseResidencyImage2D                  = VK_FALSE,
                .sparseResidencyImage3D                  = VK_FALSE,
                .sparseResidency2Samples                 = VK_FALSE,
                .sparseResidency4Samples                 = VK_FALSE,
                .sparseResidency8Samples                 = VK_FALSE,
                .sparseResidency16Samples                = VK_FALSE,
                .sparseResidencyAliased                  = VK_FALSE,
                .variableMultisampleRate                 = VK_FALSE,
                .inheritedQueries                        = VK_FALSE,
            };
            const VkDeviceCreateInfo createInfo {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = &dynamicRenderingFeature,
                .flags = 0,
                .queueCreateInfoCount = 1,
                .pQueueCreateInfos    = &queueCreateInfo,

                .enabledLayerCount   = static_cast<uint32_t>( layersToEnable.size() ),
                .ppEnabledLayerNames = layersToEnable.data(),

                .enabledExtensionCount   = static_cast<uint32_t>( extensionsToEnable.size() ),
                .ppEnabledExtensionNames = extensionsToEnable.data(),

                .pEnabledFeatures = &deviceFeatures,
            };
            if ( !CheckVkResult( vkCreateDevice( m_deviceInfo.device, &createInfo, VK_NULL_HANDLE, &m_device ), "Could not create a device" ) )
            {
                return -1;
            }

            // todo get actual values from file/cook actual values into file
            std::array poolSizes {
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, .descriptorCount = 1024 },
                VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = 1024 },
            };
            const VkDescriptorPoolCreateInfo descriptorPoolInfo {
                .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .pNext         = VK_NULL_HANDLE,
                .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                .maxSets       = 64,
                .poolSizeCount = static_cast<uint32_t>( poolSizes.size() ),
                .pPoolSizes    = poolSizes.data(),
            };
            if ( !CheckVkResult( vkCreateDescriptorPool( m_device, &descriptorPoolInfo, VK_NULL_HANDLE, &m_descriptorPool ) ) )
            {
                return -1;
            }
            return 0;
        }

        void WaitIdle() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkDeviceWaitIdle( m_device );
        }

        BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX14 const VkDevice& VkDevice() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            return m_device;
        }
        BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX14 const SPhysicalDeviceInfo& PhysicalDeviceInfo() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            return m_deviceInfo;
        }
        BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX14 VkDescriptorPool DescriptorPool() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            return m_descriptorPool;
        }

      private:
        SPhysicalDeviceInfo m_deviceInfo {};
        VkDescriptorPool m_descriptorPool { VK_NULL_HANDLE };
    };
}// namespace DeerVulkan
