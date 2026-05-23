#pragma once
#include "../deer_vulkan_core.hpp"
#include "dispatch.hpp"
#include "physical_device.hpp"

namespace deer_vulkan
{
struct Device
{
    vk::Device device{nullptr};
    vk::DescriptorPool descriptorPool{nullptr};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, PhysicalDevice& physicalDevice, Device& device) noexcept -> vk_status
{
    const bool sharedFamily{(physicalDevice.graphicsQueueFamily == physicalDevice.presentQueueFamily)};

    std::vector<vk::DeviceQueueCreateInfo> queueCIs;
    if (sharedFamily)
    {
        // All three queues from the same family — request distinct indices if available
        static constexpr std::array priorities{1.0f, 1.0f, 1.0f};
        queueCIs.push_back({
            .sType            = vk::StructureType::eDeviceQueueCreateInfo,
            .queueFamilyIndex = physicalDevice.graphicsQueueFamily,
            .queueCount       = priorities.size(),
            .pQueuePriorities = priorities.data(),
        });
    }
    else
    {
        static constexpr float priority{1.0f};
        queueCIs.push_back({
            .sType            = vk::StructureType::eDeviceQueueCreateInfo,
            .queueFamilyIndex = physicalDevice.graphicsQueueFamily,
            .queueCount       = 1U,
            .pQueuePriorities = &priority,
        });

        // Dedicated compute family if available
        if (physicalDevice.computeQueueFamily != physicalDevice.graphicsQueueFamily)
        {
            queueCIs.push_back({
                .sType            = vk::StructureType::eDeviceQueueCreateInfo,
                .queueFamilyIndex = physicalDevice.computeQueueFamily,
                .queueCount       = 1U,
                .pQueuePriorities = &priority,
            });
        }

        if (physicalDevice.presentQueueFamily != physicalDevice.graphicsQueueFamily && physicalDevice.presentQueueFamily != physicalDevice.computeQueueFamily)
        {
            queueCIs.push_back({
                .sType            = vk::StructureType::eDeviceQueueCreateInfo,
                .queueFamilyIndex = physicalDevice.presentQueueFamily,
                .queueCount       = 1U,
                .pQueuePriorities = &priority,
            });
        }
    }

    constexpr vk::Bool32 vkBoolTrue{static_cast<vk::Bool32>(true)};
    constexpr vk::Bool32 vkBoolFalse{static_cast<vk::Bool32>(false)};

    constexpr vk::Bool32 samplerMirrorClampToEdge{vkBoolFalse};
    constexpr vk::Bool32 drawIndirectCount{vkBoolFalse};
    constexpr vk::Bool32 storageBuffer8BitAccess{vkBoolFalse};
    constexpr vk::Bool32 uniformAndStorageBuffer8BitAccess{vkBoolFalse};
    constexpr vk::Bool32 storagePushConstant8{vkBoolFalse};
    constexpr vk::Bool32 shaderBufferInt64Atomics{vkBoolFalse};
    constexpr vk::Bool32 shaderSharedInt64Atomics{vkBoolFalse};
    constexpr vk::Bool32 shaderFloat16{vkBoolFalse};
    constexpr vk::Bool32 shaderInt8{vkBoolFalse};
    constexpr vk::Bool32 descriptorIndexing{vkBoolTrue};
    constexpr vk::Bool32 shaderInputAttachmentArrayDynamicIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderUniformTexelBufferArrayDynamicIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageTexelBufferArrayDynamicIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderUniformBufferArrayNonUniformIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderSampledImageArrayNonUniformIndexing{vkBoolTrue};
    constexpr vk::Bool32 shaderStorageBufferArrayNonUniformIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageImageArrayNonUniformIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderInputAttachmentArrayNonUniformIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderUniformTexelBufferArrayNonUniformIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageTexelBufferArrayNonUniformIndexing{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingUniformBufferUpdateAfterBind{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingSampledImageUpdateAfterBind{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingStorageImageUpdateAfterBind{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingStorageBufferUpdateAfterBind{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingUniformTexelBufferUpdateAfterBind{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingStorageTexelBufferUpdateAfterBind{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingUpdateUnusedWhilePending{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingPartiallyBound{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingVariableDescriptorCount{vkBoolTrue};
    constexpr vk::Bool32 runtimeDescriptorArray{vkBoolTrue};
    constexpr vk::Bool32 samplerFilterMinmax{vkBoolFalse};
    constexpr vk::Bool32 scalarBlockLayout{vkBoolFalse};
    constexpr vk::Bool32 imagelessFramebuffer{vkBoolFalse};
    constexpr vk::Bool32 uniformBufferStandardLayout{vkBoolFalse};
    constexpr vk::Bool32 shaderSubgroupExtendedTypes{vkBoolFalse};
    constexpr vk::Bool32 separateDepthStencilLayouts{vkBoolFalse};
    constexpr vk::Bool32 hostQueryReset{vkBoolFalse};
    constexpr vk::Bool32 timelineSemaphore{vkBoolTrue};
    constexpr vk::Bool32 bufferDeviceAddress{vkBoolTrue};
    constexpr vk::Bool32 bufferDeviceAddressCaptureReplay{vkBoolFalse};
    constexpr vk::Bool32 bufferDeviceAddressMultiDevice{vkBoolFalse};
    constexpr vk::Bool32 vulkanMemoryModel{vkBoolFalse};
    constexpr vk::Bool32 vulkanMemoryModelDeviceScope{vkBoolFalse};
    constexpr vk::Bool32 vulkanMemoryModelAvailabilityVisibilityChains{vkBoolFalse};
    constexpr vk::Bool32 shaderOutputViewportIndex{vkBoolFalse};
    constexpr vk::Bool32 shaderOutputLayer{vkBoolFalse};
    constexpr vk::Bool32 subgroupBroadcastDynamicId{vkBoolFalse};

    constexpr std::array deviceExtensions{
        vk::KHRSwapchainExtensionName,        vk::EXTShaderObjectExtensionName,     vk::EXTExtendedDynamicStateExtensionName, vk::EXTVertexInputDynamicStateExtensionName,
        vk::KHRDynamicRenderingExtensionName, vk::KHRSynchronization2ExtensionName, vk::KHRTimelineSemaphoreExtensionName,
    };
    vk::PhysicalDeviceVulkan12Features enabledVk12Features{
        .sType                                              = vk::StructureType::ePhysicalDeviceVulkan12Features,
        .pNext                                              = nullptr,
        .samplerMirrorClampToEdge                           = samplerMirrorClampToEdge,
        .drawIndirectCount                                  = drawIndirectCount,
        .storageBuffer8BitAccess                            = storageBuffer8BitAccess,
        .uniformAndStorageBuffer8BitAccess                  = uniformAndStorageBuffer8BitAccess,
        .storagePushConstant8                               = storagePushConstant8,
        .shaderBufferInt64Atomics                           = shaderBufferInt64Atomics,
        .shaderSharedInt64Atomics                           = shaderSharedInt64Atomics,
        .shaderFloat16                                      = shaderFloat16,
        .shaderInt8                                         = shaderInt8,
        .descriptorIndexing                                 = descriptorIndexing,
        .shaderInputAttachmentArrayDynamicIndexing          = shaderInputAttachmentArrayDynamicIndexing,
        .shaderUniformTexelBufferArrayDynamicIndexing       = shaderUniformTexelBufferArrayDynamicIndexing,
        .shaderStorageTexelBufferArrayDynamicIndexing       = shaderStorageTexelBufferArrayDynamicIndexing,
        .shaderUniformBufferArrayNonUniformIndexing         = shaderUniformBufferArrayNonUniformIndexing,
        .shaderSampledImageArrayNonUniformIndexing          = shaderSampledImageArrayNonUniformIndexing,
        .shaderStorageBufferArrayNonUniformIndexing         = shaderStorageBufferArrayNonUniformIndexing,
        .shaderStorageImageArrayNonUniformIndexing          = shaderStorageImageArrayNonUniformIndexing,
        .shaderInputAttachmentArrayNonUniformIndexing       = shaderInputAttachmentArrayNonUniformIndexing,
        .shaderUniformTexelBufferArrayNonUniformIndexing    = shaderUniformTexelBufferArrayNonUniformIndexing,
        .shaderStorageTexelBufferArrayNonUniformIndexing    = shaderStorageTexelBufferArrayNonUniformIndexing,
        .descriptorBindingUniformBufferUpdateAfterBind      = descriptorBindingUniformBufferUpdateAfterBind,
        .descriptorBindingSampledImageUpdateAfterBind       = descriptorBindingSampledImageUpdateAfterBind,
        .descriptorBindingStorageImageUpdateAfterBind       = descriptorBindingStorageImageUpdateAfterBind,
        .descriptorBindingStorageBufferUpdateAfterBind      = descriptorBindingStorageBufferUpdateAfterBind,
        .descriptorBindingUniformTexelBufferUpdateAfterBind = descriptorBindingUniformTexelBufferUpdateAfterBind,
        .descriptorBindingStorageTexelBufferUpdateAfterBind = descriptorBindingStorageTexelBufferUpdateAfterBind,
        .descriptorBindingUpdateUnusedWhilePending          = descriptorBindingUpdateUnusedWhilePending,
        .descriptorBindingPartiallyBound                    = descriptorBindingPartiallyBound,
        .descriptorBindingVariableDescriptorCount           = descriptorBindingVariableDescriptorCount,
        .runtimeDescriptorArray                             = runtimeDescriptorArray,
        .samplerFilterMinmax                                = samplerFilterMinmax,
        .scalarBlockLayout                                  = scalarBlockLayout,
        .imagelessFramebuffer                               = imagelessFramebuffer,
        .uniformBufferStandardLayout                        = uniformBufferStandardLayout,
        .shaderSubgroupExtendedTypes                        = shaderSubgroupExtendedTypes,
        .separateDepthStencilLayouts                        = separateDepthStencilLayouts,
        .hostQueryReset                                     = hostQueryReset,
        .timelineSemaphore                                  = timelineSemaphore,
        .bufferDeviceAddress                                = bufferDeviceAddress,
        .bufferDeviceAddressCaptureReplay                   = bufferDeviceAddressCaptureReplay,
        .bufferDeviceAddressMultiDevice                     = bufferDeviceAddressMultiDevice,
        .vulkanMemoryModel                                  = vulkanMemoryModel,
        .vulkanMemoryModelDeviceScope                       = vulkanMemoryModelDeviceScope,
        .vulkanMemoryModelAvailabilityVisibilityChains      = vulkanMemoryModelAvailabilityVisibilityChains,
        .shaderOutputViewportIndex                          = shaderOutputViewportIndex,
        .shaderOutputLayer                                  = shaderOutputLayer,
        .subgroupBroadcastDynamicId                         = subgroupBroadcastDynamicId,
    };

    constexpr vk::Bool32 robustImageAccess{vkBoolFalse};
    constexpr vk::Bool32 inlineUniformBlock{vkBoolFalse};
    constexpr vk::Bool32 descriptorBindingInlineUniformBlockUpdateAfterBind{vkBoolFalse};
    constexpr vk::Bool32 pipelineCreationCacheControl{vkBoolFalse};
    constexpr vk::Bool32 privateData{vkBoolFalse};
    constexpr vk::Bool32 shaderDemoteToHelperInvocation{vkBoolFalse};
    constexpr vk::Bool32 shaderTerminateInvocation{vkBoolFalse};
    constexpr vk::Bool32 subgroupSizeControl{vkBoolFalse};
    constexpr vk::Bool32 computeFullSubgroups{vkBoolFalse};
    constexpr vk::Bool32 synchronization2{vkBoolTrue};
    constexpr vk::Bool32 textureCompressionASTC_HDR{vkBoolFalse};
    constexpr vk::Bool32 shaderZeroInitializeWorkgroupMemory{vkBoolFalse};
    constexpr vk::Bool32 dynamicRendering{vkBoolTrue};
    constexpr vk::Bool32 shaderIntegerDotProduct{vkBoolFalse};
    constexpr vk::Bool32 maintenance4{vkBoolTrue};
    vk::PhysicalDeviceVulkan13Features enabledVk13Features{
        .sType                                              = vk::StructureType::ePhysicalDeviceVulkan13Features,
        .pNext                                              = &enabledVk12Features,
        .robustImageAccess                                  = robustImageAccess,
        .inlineUniformBlock                                 = inlineUniformBlock,
        .descriptorBindingInlineUniformBlockUpdateAfterBind = descriptorBindingInlineUniformBlockUpdateAfterBind,
        .pipelineCreationCacheControl                       = pipelineCreationCacheControl,
        .privateData                                        = privateData,
        .shaderDemoteToHelperInvocation                     = shaderDemoteToHelperInvocation,
        .shaderTerminateInvocation                          = shaderTerminateInvocation,
        .subgroupSizeControl                                = subgroupSizeControl,
        .computeFullSubgroups                               = computeFullSubgroups,
        .synchronization2                                   = synchronization2,
        .textureCompressionASTC_HDR                         = textureCompressionASTC_HDR,
        .shaderZeroInitializeWorkgroupMemory                = shaderZeroInitializeWorkgroupMemory,
        .dynamicRendering                                   = dynamicRendering,
        .shaderIntegerDotProduct                            = shaderIntegerDotProduct,
        .maintenance4                                       = maintenance4,
    };

    constexpr vk::Bool32 globalPriorityQuery{vkBoolFalse};
    constexpr vk::Bool32 shaderSubgroupRotate{vkBoolFalse};
    constexpr vk::Bool32 shaderSubgroupRotateClustered{vkBoolFalse};
    constexpr vk::Bool32 shaderFloatControls2{vkBoolFalse};
    constexpr vk::Bool32 shaderExpectAssume{vkBoolFalse};
    constexpr vk::Bool32 rectangularLines{vkBoolFalse};
    constexpr vk::Bool32 bresenhamLines{vkBoolFalse};
    constexpr vk::Bool32 smoothLines{vkBoolFalse};
    constexpr vk::Bool32 stippledRectangularLines{vkBoolFalse};
    constexpr vk::Bool32 stippledBresenhamLines{vkBoolFalse};
    constexpr vk::Bool32 stippledSmoothLines{vkBoolFalse};
    constexpr vk::Bool32 vertexAttributeInstanceRateDivisor{vkBoolFalse};
    constexpr vk::Bool32 vertexAttributeInstanceRateZeroDivisor{vkBoolFalse};
    constexpr vk::Bool32 indexTypeUint8{vkBoolFalse};
    constexpr vk::Bool32 dynamicRenderingLocalRead{vkBoolFalse};
    constexpr vk::Bool32 maintenance5{vkBoolTrue};
    constexpr vk::Bool32 maintenance6{vkBoolTrue};
    constexpr vk::Bool32 pipelineProtectedAccess{vkBoolFalse};
    constexpr vk::Bool32 pipelineRobustness{vkBoolFalse};
    constexpr vk::Bool32 hostImageCopy{vkBoolFalse};
    constexpr vk::Bool32 pushDescriptor{vkBoolFalse};
    vk::PhysicalDeviceVulkan14Features enabledVk14Features{
        .sType                                  = vk::StructureType::ePhysicalDeviceVulkan14Features,
        .pNext                                  = &enabledVk13Features,
        .globalPriorityQuery                    = globalPriorityQuery,
        .shaderSubgroupRotate                   = shaderSubgroupRotate,
        .shaderSubgroupRotateClustered          = shaderSubgroupRotateClustered,
        .shaderFloatControls2                   = shaderFloatControls2,
        .shaderExpectAssume                     = shaderExpectAssume,
        .rectangularLines                       = rectangularLines,
        .bresenhamLines                         = bresenhamLines,
        .smoothLines                            = smoothLines,
        .stippledRectangularLines               = stippledRectangularLines,
        .stippledBresenhamLines                 = stippledBresenhamLines,
        .stippledSmoothLines                    = stippledSmoothLines,
        .vertexAttributeInstanceRateDivisor     = vertexAttributeInstanceRateDivisor,
        .vertexAttributeInstanceRateZeroDivisor = vertexAttributeInstanceRateZeroDivisor,
        .indexTypeUint8                         = indexTypeUint8,
        .dynamicRenderingLocalRead              = dynamicRenderingLocalRead,
        .maintenance5                           = maintenance5,
        .maintenance6                           = maintenance6,
        .pipelineProtectedAccess                = pipelineProtectedAccess,
        .pipelineRobustness                     = pipelineRobustness,
        .hostImageCopy                          = hostImageCopy,
        .pushDescriptor                         = pushDescriptor,
    };

    constexpr vk::Bool32 robustBufferAccess{vkBoolFalse};
    constexpr vk::Bool32 fullDrawIndexUint32{vkBoolFalse};
    constexpr vk::Bool32 imageCubeArray{vkBoolFalse};
    constexpr vk::Bool32 independentBlend{vkBoolFalse};
    constexpr vk::Bool32 geometryShader{vkBoolFalse};
    constexpr vk::Bool32 tessellationShader{vkBoolFalse};
    constexpr vk::Bool32 sampleRateShading{vkBoolFalse};
    constexpr vk::Bool32 dualSrcBlend{vkBoolFalse};
    constexpr vk::Bool32 logicOp{vkBoolFalse};
    constexpr vk::Bool32 multiDrawIndirect{vkBoolFalse};
    constexpr vk::Bool32 drawIndirectFirstInstance{vkBoolFalse};
    constexpr vk::Bool32 depthClamp{vkBoolFalse};
    constexpr vk::Bool32 depthBiasClamp{vkBoolFalse};
    constexpr vk::Bool32 fillModeNonSolid{vkBoolFalse};
    constexpr vk::Bool32 depthBounds{vkBoolFalse};
    constexpr vk::Bool32 wideLines{vkBoolFalse};
    constexpr vk::Bool32 largePoints{vkBoolFalse};
    constexpr vk::Bool32 alphaToOne{vkBoolFalse};
    constexpr vk::Bool32 multiViewport{vkBoolFalse};
    constexpr vk::Bool32 samplerAnisotropy{vkBoolTrue};
    constexpr vk::Bool32 textureCompressionETC2{vkBoolFalse};
    constexpr vk::Bool32 textureCompressionASTC_LDR{vkBoolFalse};
    constexpr vk::Bool32 textureCompressionBC{vkBoolFalse};
    constexpr vk::Bool32 occlusionQueryPrecise{vkBoolFalse};
    constexpr vk::Bool32 pipelineStatisticsQuery{vkBoolFalse};
    constexpr vk::Bool32 vertexPipelineStoresAndAtomics{vkBoolFalse};
    constexpr vk::Bool32 fragmentStoresAndAtomics{vkBoolFalse};
    constexpr vk::Bool32 shaderTessellationAndGeometryPointSize{vkBoolFalse};
    constexpr vk::Bool32 shaderImageGatherExtended{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageImageExtendedFormats{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageImageMultisample{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageImageReadWithoutFormat{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageImageWriteWithoutFormat{vkBoolFalse};
    constexpr vk::Bool32 shaderUniformBufferArrayDynamicIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderSampledImageArrayDynamicIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageBufferArrayDynamicIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderStorageImageArrayDynamicIndexing{vkBoolFalse};
    constexpr vk::Bool32 shaderClipDistance{vkBoolFalse};
    constexpr vk::Bool32 shaderCullDistance{vkBoolFalse};
    constexpr vk::Bool32 shaderFloat64{vkBoolFalse};
    constexpr vk::Bool32 shaderInt64{vkBoolFalse};
    constexpr vk::Bool32 shaderInt16{vkBoolFalse};
    constexpr vk::Bool32 shaderResourceResidency{vkBoolFalse};
    constexpr vk::Bool32 shaderResourceMinLod{vkBoolFalse};
    constexpr vk::Bool32 sparseBinding{vkBoolFalse};
    constexpr vk::Bool32 sparseResidencyBuffer{vkBoolFalse};
    constexpr vk::Bool32 sparseResidencyImage2D{vkBoolFalse};
    constexpr vk::Bool32 sparseResidencyImage3D{vkBoolFalse};
    constexpr vk::Bool32 sparseResidency2Samples{vkBoolFalse};
    constexpr vk::Bool32 sparseResidency4Samples{vkBoolFalse};
    constexpr vk::Bool32 sparseResidency8Samples{vkBoolFalse};
    constexpr vk::Bool32 sparseResidency16Samples{vkBoolFalse};
    constexpr vk::Bool32 sparseResidencyAliased{vkBoolFalse};
    constexpr vk::Bool32 variableMultisampleRate{vkBoolFalse};
    constexpr vk::Bool32 inheritedQueries{vkBoolFalse};
    vk::PhysicalDeviceFeatures enabledVk10Features{
        .robustBufferAccess                      = robustBufferAccess,
        .fullDrawIndexUint32                     = fullDrawIndexUint32,
        .imageCubeArray                          = imageCubeArray,
        .independentBlend                        = independentBlend,
        .geometryShader                          = geometryShader,
        .tessellationShader                      = tessellationShader,
        .sampleRateShading                       = sampleRateShading,
        .dualSrcBlend                            = dualSrcBlend,
        .logicOp                                 = logicOp,
        .multiDrawIndirect                       = multiDrawIndirect,
        .drawIndirectFirstInstance               = drawIndirectFirstInstance,
        .depthClamp                              = depthClamp,
        .depthBiasClamp                          = depthBiasClamp,
        .fillModeNonSolid                        = fillModeNonSolid,
        .depthBounds                             = depthBounds,
        .wideLines                               = wideLines,
        .largePoints                             = largePoints,
        .alphaToOne                              = alphaToOne,
        .multiViewport                           = multiViewport,
        .samplerAnisotropy                       = samplerAnisotropy,
        .textureCompressionETC2                  = textureCompressionETC2,
        .textureCompressionASTC_LDR              = textureCompressionASTC_LDR,
        .textureCompressionBC                    = textureCompressionBC,
        .occlusionQueryPrecise                   = occlusionQueryPrecise,
        .pipelineStatisticsQuery                 = pipelineStatisticsQuery,
        .vertexPipelineStoresAndAtomics          = vertexPipelineStoresAndAtomics,
        .fragmentStoresAndAtomics                = fragmentStoresAndAtomics,
        .shaderTessellationAndGeometryPointSize  = shaderTessellationAndGeometryPointSize,
        .shaderImageGatherExtended               = shaderImageGatherExtended,
        .shaderStorageImageExtendedFormats       = shaderStorageImageExtendedFormats,
        .shaderStorageImageMultisample           = shaderStorageImageMultisample,
        .shaderStorageImageReadWithoutFormat     = shaderStorageImageReadWithoutFormat,
        .shaderStorageImageWriteWithoutFormat    = shaderStorageImageWriteWithoutFormat,
        .shaderUniformBufferArrayDynamicIndexing = shaderUniformBufferArrayDynamicIndexing,
        .shaderSampledImageArrayDynamicIndexing  = shaderSampledImageArrayDynamicIndexing,
        .shaderStorageBufferArrayDynamicIndexing = shaderStorageBufferArrayDynamicIndexing,
        .shaderStorageImageArrayDynamicIndexing  = shaderStorageImageArrayDynamicIndexing,
        .shaderClipDistance                      = shaderClipDistance,
        .shaderCullDistance                      = shaderCullDistance,
        .shaderFloat64                           = shaderFloat64,
        .shaderInt64                             = shaderInt64,
        .shaderInt16                             = shaderInt16,
        .shaderResourceResidency                 = shaderResourceResidency,
        .shaderResourceMinLod                    = shaderResourceMinLod,
        .sparseBinding                           = sparseBinding,
        .sparseResidencyBuffer                   = sparseResidencyBuffer,
        .sparseResidencyImage2D                  = sparseResidencyImage2D,
        .sparseResidencyImage3D                  = sparseResidencyImage3D,
        .sparseResidency2Samples                 = sparseResidency2Samples,
        .sparseResidency4Samples                 = sparseResidency4Samples,
        .sparseResidency8Samples                 = sparseResidency8Samples,
        .sparseResidency16Samples                = sparseResidency16Samples,
        .sparseResidencyAliased                  = sparseResidencyAliased,
        .variableMultisampleRate                 = variableMultisampleRate,
        .inheritedQueries                        = inheritedQueries,
    };
    vk::PhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeatures{
        .sType        = vk::StructureType::ePhysicalDeviceShaderObjectFeaturesEXT,
        .pNext        = &enabledVk14Features,
        .shaderObject = vkBoolTrue,
    };
    vk::DeviceCreateInfo deviceCI{
        .sType                   = vk::StructureType::eDeviceCreateInfo,
        .pNext                   = &shaderObjectFeatures,
        .flags                   = {},
        .queueCreateInfoCount    = static_cast<std::uint32_t>(queueCIs.size()),
        .pQueueCreateInfos       = queueCIs.data(),
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = nullptr,
        .enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures        = &enabledVk10Features,

    };

    device.device = physicalDevice.physicalDevice.createDevice(deviceCI, nullptr, dispatch.dispatch);
    if (!device.device)
    {
        std::println("could not create device");
        return vk_status::null_value;
    }

    // todo : set these using config files or so
    // the packer/converter that converts scenes to a binary format should know how many things are needed in total
    std::array poolSizes{
        vk::DescriptorPoolSize{vk::DescriptorType::eSampler, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, 32},
        vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, 32},
    };
    vk::DescriptorPoolCreateInfo descPoolCI{
        .sType         = vk::StructureType::eDescriptorPoolCreateInfo,
        .pNext         = nullptr,
        .flags         = {},
        .maxSets       = 32,
        .poolSizeCount = poolSizes.size(),
        .pPoolSizes    = poolSizes.data(),
    };

    device.descriptorPool = device.device.createDescriptorPool(descPoolCI, nullptr, dispatch.dispatch);
    if (!device.descriptorPool)
    {
        std::println("could not create descriptor pool");
        return vk_status::null_value;
    }

    return vk_status::ok;
}

inline auto Cleanup(const Dispatch& dispatch, Device& device) noexcept -> void
{
    if (device.device == nullptr)
    {
        return;
    }
    if (device.descriptorPool == nullptr)
    {
        return;
    }

    device.device.destroyDescriptorPool(device.descriptorPool, nullptr, dispatch.dispatch);
    device.device.destroy(nullptr, dispatch.dispatch);
    device.descriptorPool = nullptr;
    device.device         = nullptr;
}
} // namespace deer_vulkan
