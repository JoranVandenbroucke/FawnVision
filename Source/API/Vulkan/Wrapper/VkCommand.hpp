//
// Copyright (c) 2024.
// Author: Joran.
//

#pragma once
#include "../DeerVulkan_Core.hpp"
#include "VkDevice.hpp"
#include "VkShader.hpp"
#include "VkSwapChain.hpp"

namespace DeerVulkan
{
struct SVkCommandPool
{
    VkCommandPool commandPool;
};
struct SVkCommandBuffer
{
    VkCommandBuffer commandBuffer;
    uint32_t count;

    PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{VK_NULL_HANDLE};
    PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT{VK_NULL_HANDLE};
    PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR{VK_NULL_HANDLE};
    PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT{VK_NULL_HANDLE};
    PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT{VK_NULL_HANDLE};
};

inline auto InitializeFunctions(const SVkInstance& instance, SVkCommandBuffer& commandBuffer)
{
    commandBuffer.vkCmdBeginRenderingKHR             = std::bit_cast<PFN_vkCmdBeginRenderingKHR>(vkGetInstanceProcAddr(instance.instance, "vkCmdBeginRenderingKHR"));
    commandBuffer.vkCmdBindShadersEXT                = std::bit_cast<PFN_vkCmdBindShadersEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdBindShadersEXT"));
    commandBuffer.vkCmdEndRenderingKHR               = std::bit_cast<PFN_vkCmdEndRenderingKHR>(vkGetInstanceProcAddr(instance.instance, "vkCmdEndRenderingKHR"));
    commandBuffer.vkCmdSetAlphaToCoverageEnableEXT   = std::bit_cast<PFN_vkCmdSetAlphaToCoverageEnableEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetAlphaToCoverageEnableEXT"));
    commandBuffer.vkCmdSetColorBlendEnableEXT        = std::bit_cast<PFN_vkCmdSetColorBlendEnableEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetColorBlendEnableEXT"));
    commandBuffer.vkCmdSetColorBlendEquationEXT      = std::bit_cast<PFN_vkCmdSetColorBlendEquationEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetColorBlendEquationEXT"));
    commandBuffer.vkCmdSetColorWriteMaskEXT          = std::bit_cast<PFN_vkCmdSetColorWriteMaskEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetColorWriteMaskEXT"));
    commandBuffer.vkCmdSetCullModeEXT                = std::bit_cast<PFN_vkCmdSetCullModeEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetCullModeEXT"));
    commandBuffer.vkCmdSetDepthBiasEnableEXT         = std::bit_cast<PFN_vkCmdSetDepthBiasEnableEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetDepthBiasEnableEXT"));
    commandBuffer.vkCmdSetDepthCompareOpEXT          = std::bit_cast<PFN_vkCmdSetDepthCompareOpEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetDepthCompareOpEXT"));
    commandBuffer.vkCmdSetDepthTestEnableEXT         = std::bit_cast<PFN_vkCmdSetDepthTestEnableEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetDepthTestEnableEXT"));
    commandBuffer.vkCmdSetDepthWriteEnableEXT        = std::bit_cast<PFN_vkCmdSetDepthWriteEnableEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetDepthWriteEnableEXT"));
    commandBuffer.vkCmdSetFrontFaceEXT               = std::bit_cast<PFN_vkCmdSetFrontFaceEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetFrontFaceEXT"));
    commandBuffer.vkCmdSetPolygonModeEXT             = std::bit_cast<PFN_vkCmdSetPolygonModeEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetPolygonModeEXT"));
    commandBuffer.vkCmdSetPrimitiveRestartEnableEXT  = std::bit_cast<PFN_vkCmdSetPrimitiveRestartEnableEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetPrimitiveRestartEnableEXT"));
    commandBuffer.vkCmdSetPrimitiveTopologyEXT       = std::bit_cast<PFN_vkCmdSetPrimitiveTopologyEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetPrimitiveTopologyEXT"));
    commandBuffer.vkCmdSetRasterizationSamplesEXT    = std::bit_cast<PFN_vkCmdSetRasterizationSamplesEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetRasterizationSamplesEXT"));
    commandBuffer.vkCmdSetRasterizerDiscardEnableEXT = std::bit_cast<PFN_vkCmdSetRasterizerDiscardEnableEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetRasterizerDiscardEnableEXT"));
    commandBuffer.vkCmdSetSampleMaskEXT              = std::bit_cast<PFN_vkCmdSetSampleMaskEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetSampleMaskEXT"));
    commandBuffer.vkCmdSetScissorWithCountEXT        = std::bit_cast<PFN_vkCmdSetScissorWithCountEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetScissorWithCountEXT"));
    commandBuffer.vkCmdSetStencilTestEnableEXT       = std::bit_cast<PFN_vkCmdSetStencilTestEnableEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetStencilTestEnableEXT"));
    commandBuffer.vkCmdSetVertexInputEXT             = std::bit_cast<PFN_vkCmdSetVertexInputEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetVertexInputEXT"));
    commandBuffer.vkCmdSetViewportWithCountEXT       = std::bit_cast<PFN_vkCmdSetViewportWithCountEXT>(vkGetInstanceProcAddr(instance.instance, "vkCmdSetViewportWithCountEXT"));

    if (!commandBuffer.vkCmdBeginRenderingKHR || !commandBuffer.vkCmdBindShadersEXT || !commandBuffer.vkCmdEndRenderingKHR || !commandBuffer.vkCmdSetAlphaToCoverageEnableEXT || !commandBuffer.vkCmdSetColorBlendEnableEXT
        || !commandBuffer.vkCmdSetColorBlendEquationEXT || !commandBuffer.vkCmdSetColorWriteMaskEXT || !commandBuffer.vkCmdSetCullModeEXT || !commandBuffer.vkCmdSetDepthBiasEnableEXT || !commandBuffer.vkCmdSetDepthCompareOpEXT
        || !commandBuffer.vkCmdSetDepthTestEnableEXT || !commandBuffer.vkCmdSetDepthWriteEnableEXT || !commandBuffer.vkCmdSetFrontFaceEXT || !commandBuffer.vkCmdSetPolygonModeEXT || !commandBuffer.vkCmdSetPrimitiveRestartEnableEXT
        || !commandBuffer.vkCmdSetPrimitiveTopologyEXT || !commandBuffer.vkCmdSetRasterizationSamplesEXT || !commandBuffer.vkCmdSetRasterizerDiscardEnableEXT || !commandBuffer.vkCmdSetSampleMaskEXT
        || !commandBuffer.vkCmdSetScissorWithCountEXT || !commandBuffer.vkCmdSetStencilTestEnableEXT || !commandBuffer.vkCmdSetVertexInputEXT || !commandBuffer.vkCmdSetViewportWithCountEXT)
    {
        return -1;
    }
    return 0;
}

inline auto Initialize(SVkCommandPool& commandPool, const SVkDevice& device, const uint32_t queueFamilyIndex)
{
    if (const VkCommandPoolCreateInfo createInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext            = VK_NULL_HANDLE,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndex,
        };
        !CheckVkResult(vkCreateCommandPool(device.device, &createInfo, VK_NULL_HANDLE, &commandPool.commandPool)))
    {
        return -1;
    }

    return 0;
}
inline auto CreateCommandBuffer(const SVkInstance& instance, const SVkDevice& device, const SVkCommandPool& commandPool, const uint32_t count, SVkCommandBuffer& commandBuffer)
{
    if (const VkCommandBufferAllocateInfo allocInfo{
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = nullptr,
            .commandPool        = commandPool.commandPool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count,
        };
        !CheckVkResult(vkAllocateCommandBuffers(device.device, &allocInfo, &commandBuffer.commandBuffer)))
    {
        return -1;
    }
    commandBuffer.count = count;
    return InitializeFunctions(instance, commandBuffer);
}
inline void Cleanup(const SVkDevice& device, SVkCommandPool& commandPool)
{
    vkDestroyCommandPool(device.device, commandPool.commandPool, nullptr);
    commandPool.commandPool = nullptr;
}
inline void CleanupBuffer(const SVkDevice& device, const SVkCommandPool& commandPool, SVkCommandBuffer& commandBuffer)
{
    vkFreeCommandBuffers(device.device, commandPool.commandPool, commandBuffer.count, &commandBuffer.commandBuffer);
    commandBuffer.commandBuffer = nullptr;
}
inline auto BeginCommand(const SVkCommandBuffer& commandBuffer) -> int32_t
{

    if (constexpr VkCommandBufferBeginInfo commandBufferBeginInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = VK_NULL_HANDLE,
            .flags            = 0,       // Optional
            .pInheritanceInfo = nullptr, // Optional
        };
        !CheckVkResult(vkBeginCommandBuffer(commandBuffer.commandBuffer, &commandBufferBeginInfo)))
    {
        return -1;
    }
    return 0;
}
inline auto EndCommand(const SVkCommandBuffer& commandBuffer) -> int32_t
{
    if (!CheckVkResult(vkEndCommandBuffer(commandBuffer.commandBuffer)))
    {
        return -1;
    }
    return 0;
}
inline void BeginRender(const SVkCommandBuffer& commandBuffer, const SVkSwapChain& swapChain)
{
    commandBuffer.vkCmdBeginRenderingKHR(commandBuffer.commandBuffer, &swapChain.renderingInfo);
}
inline void EndRender(const SVkCommandBuffer& commandBuffer)
{
    commandBuffer.vkCmdEndRenderingKHR(commandBuffer.commandBuffer);
}
inline void SetViewport(const SVkCommandBuffer& commandBuffer, const float xPos, const float yPos, const float width, const float height, const float minDepth, const float maxDepth)
{
    const VkViewport viewport{.x = xPos, .y = yPos, .width = width, .height = height, .minDepth = minDepth, .maxDepth = maxDepth};
    commandBuffer.vkCmdSetViewportWithCountEXT(commandBuffer.commandBuffer, 1, &viewport);
}
inline void SetScissor(const SVkCommandBuffer& commandBuffer, const uint32_t scissorWidth, const uint32_t scissorHeight, const int32_t scissorX, const int32_t scissorY)
{
    const VkRect2D scissor{.offset = {scissorX, scissorY}, .extent = {scissorWidth, scissorHeight}};
    commandBuffer.vkCmdSetScissorWithCountEXT(commandBuffer.commandBuffer, 1, &scissor);
}
inline void SetAlphaToCoverageEnable(const SVkCommandBuffer& commandBuffer, const bool enable)
{
    commandBuffer.vkCmdSetAlphaToCoverageEnableEXT(commandBuffer.commandBuffer, static_cast<VkBool32>(enable));
}
inline void SetColorBlendEnable(const SVkCommandBuffer& commandBuffer, const bool enable)
{
    const VkBool32 enableColorBlend{static_cast<const VkBool32>(enable)};
    commandBuffer.vkCmdSetColorBlendEnableEXT(commandBuffer.commandBuffer, 0, 1, &enableColorBlend);
}
inline void SetColorBlendEquation(const SVkCommandBuffer& commandBuffer, uint32_t srcColorBlendFactor, uint32_t dstColorBlendFactor, uint32_t colorBlendOp, uint32_t srcAlphaBlendFactor, uint32_t dstAlphaBlendFactor, uint32_t alphaBlendOp)
{
    const VkColorBlendEquationEXT blendEquation{static_cast<VkBlendFactor>(srcColorBlendFactor), static_cast<VkBlendFactor>(dstColorBlendFactor), static_cast<VkBlendOp>(colorBlendOp),
                                                static_cast<VkBlendFactor>(srcAlphaBlendFactor), static_cast<VkBlendFactor>(dstAlphaBlendFactor), static_cast<VkBlendOp>(alphaBlendOp)};
    commandBuffer.vkCmdSetColorBlendEquationEXT(commandBuffer.commandBuffer, 0, 1, &blendEquation);
}
inline void SetColorWriteMask(const SVkCommandBuffer& commandBuffer, const uint32_t mask)
{
    const VkColorComponentFlags colorFlages{mask};
    commandBuffer.vkCmdSetColorWriteMaskEXT(commandBuffer.commandBuffer, 0, 1, &colorFlages);
}
inline void SetCullMode(const SVkCommandBuffer& commandBuffer, const uint8_t cullmode)
{
    commandBuffer.vkCmdSetCullModeEXT(commandBuffer.commandBuffer, cullmode);
}
inline void SetDepthBiasEnable(const SVkCommandBuffer& commandBuffer, const bool enable)
{
    commandBuffer.vkCmdSetDepthBiasEnableEXT(commandBuffer.commandBuffer, static_cast<VkBool32>(enable));
}
inline void SetDepthCompareOperator(const SVkCommandBuffer& commandBuffer, uint8_t compareOp)
{
    commandBuffer.vkCmdSetDepthCompareOpEXT(commandBuffer.commandBuffer, static_cast<VkCompareOp>(compareOp));
}
inline void SetDepthTestEnable(const SVkCommandBuffer& commandBuffer, const bool enable)
{
    commandBuffer.vkCmdSetDepthTestEnableEXT(commandBuffer.commandBuffer, static_cast<VkBool32>(enable));
}
inline void SetDepthWriteEnable(const SVkCommandBuffer& commandBuffer, const bool enable)
{
    commandBuffer.vkCmdSetDepthWriteEnableEXT(commandBuffer.commandBuffer, static_cast<VkBool32>(enable));
}
inline void SetFrontFace(const SVkCommandBuffer& commandBuffer, const bool isClockWice)
{
    commandBuffer.vkCmdSetFrontFaceEXT(commandBuffer.commandBuffer, static_cast<VkFrontFace>(isClockWice));
}
inline void SetLineWidth(const SVkCommandBuffer& commandBuffer, const float isClockWice)
{
    vkCmdSetLineWidth(commandBuffer.commandBuffer, isClockWice);
}
inline void SetPolygonMode(const SVkCommandBuffer& commandBuffer, uint32_t mode)
{
    commandBuffer.vkCmdSetPolygonModeEXT(commandBuffer.commandBuffer, static_cast<VkPolygonMode>(mode));
}
inline void SetPrimitiveRestartEnable(const SVkCommandBuffer& commandBuffer, const bool enable)
{
    commandBuffer.vkCmdSetPrimitiveRestartEnableEXT(commandBuffer.commandBuffer, static_cast<VkBool32>(enable));
}
inline void SetPrimitiveTopology(const SVkCommandBuffer& commandBuffer, uint8_t topology)
{
    commandBuffer.vkCmdSetPrimitiveTopologyEXT(commandBuffer.commandBuffer, static_cast<VkPrimitiveTopology>(topology));
}
inline void SetRasterizationSamples(const SVkCommandBuffer& commandBuffer, uint8_t sampleSize, const uint32_t mask)
{
    const VkSampleMask sampleMask{mask};
    commandBuffer.vkCmdSetRasterizationSamplesEXT(commandBuffer.commandBuffer, static_cast<VkSampleCountFlagBits>(sampleSize));
    commandBuffer.vkCmdSetSampleMaskEXT(commandBuffer.commandBuffer, static_cast<VkSampleCountFlagBits>(sampleSize), &sampleMask);
}
inline void SetRasterizerDiscardEnable(const SVkCommandBuffer& commandBuffer, const bool enable)
{
    commandBuffer.vkCmdSetRasterizerDiscardEnableEXT(commandBuffer.commandBuffer, enable ? VK_TRUE : VK_FALSE);
}
inline void SetStencilTestEnable(const SVkCommandBuffer& commandBuffer, const bool enable)
{
    commandBuffer.vkCmdSetStencilTestEnableEXT(commandBuffer.commandBuffer, static_cast<VkBool32>(enable));
}
inline void SetVertexInput(const SVkCommandBuffer& commandBuffer, const VkVertexInputBindingDescription2EXT& vertexBind, const std::vector<VkVertexInputAttributeDescription2EXT>& vertexAttribute)
{
    commandBuffer.vkCmdSetVertexInputEXT(commandBuffer.commandBuffer, 1, &vertexBind, static_cast<uint32_t>(vertexAttribute.size()), vertexAttribute.data());
}
inline void SetVertexInput(const SVkCommandBuffer& commandBuffer)
{
    constexpr VkVertexInputBindingDescription2EXT vertexInputBinding{
        .sType     = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
        .pNext     = VK_NULL_HANDLE,
        .binding   = 0,
        .stride    = 0,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        .divisor   = 1,
    };
    commandBuffer.vkCmdSetVertexInputEXT(commandBuffer.commandBuffer, 1, &vertexInputBinding, 0, VK_NULL_HANDLE);
}
inline void BindShader(const SVkCommandBuffer& commandBuffer, const SVKShader& shader)
{
    commandBuffer.vkCmdBindShadersEXT(commandBuffer.commandBuffer, static_cast<uint32_t>(shader.stages.size()), shader.stages.data(), shader.shaders.data());
}
// inline void BindDescriptorSet(const SVkCommandBuffer& commandBuffer)
// {
//     // commandBuffer.vkCmdBindDescriptorSets()
//     VkPipelineLayout pipelineLayout;
//     VkDescriptorSet descriptorSet;
//     vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, VK_NULL_HANDLE  );
// }
inline void MakeReadyToRender(const SVkCommandBuffer& commandBuffer, const SVkSwapChain& swapChain)
{
    const VkImageMemoryBarrier imageMemoryBarrier{GetImageRenderMemory(swapChain)};
    vkCmdPipelineBarrier(commandBuffer.commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier);
}
inline void MakeReadyToPresent(const SVkCommandBuffer& commandBuffer, const SVkSwapChain& swapChain)
{
    const VkImageMemoryBarrier imageMemoryBarrier{GetImagePresentMemory(swapChain)};
    vkCmdPipelineBarrier(commandBuffer.commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier);
}
inline void DrawFullscreen(const SVkCommandBuffer& commandBuffer)
{
    vkCmdDraw(commandBuffer.commandBuffer, 3, 1, 0, 0);
}
} // namespace DeerVulkan