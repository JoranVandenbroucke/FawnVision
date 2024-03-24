//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"

#include "VkDevice.hpp"
#include "VkShader.hpp"
#include "VkSwapChain.hpp"

#include <Windows.ApplicationModel.Background.h>
#include <cstdint>
#include <functional>

namespace DeerVulkan
{
class CVkCommandBuffer final
{
  public:
    BALBINO_CONSTEXPR_SINCE_CXX20 BALBINO_EXPLICIT_SINCE_CXX11 CVkCommandBuffer(const CVkSwapChain* pSwapChain, const VkCommandBuffer& commandBuffer, const uint32_t count,
                                                                                                  const std::function<void(const VkCommandBuffer&, uint32_t)>& cleanup)
        : m_swapChain{pSwapChain}
        , m_commandBuffer{commandBuffer}
        , m_cleanup{cleanup}
        , m_count{count}
    {
    }
    BALBINO_CONSTEXPR_SINCE_CXX20 ~CVkCommandBuffer()
    {
        m_cleanup(m_commandBuffer, m_count);
    }

    CVkCommandBuffer(const CVkCommandBuffer&)                    = delete;
    CVkCommandBuffer(CVkCommandBuffer&&)                         = delete;
    auto operator=(const CVkCommandBuffer&) -> CVkCommandBuffer& = delete;
    auto operator=(CVkCommandBuffer&&) -> CVkCommandBuffer&      = delete;

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Handle() const BALBINO_NOEXCEPT_SINCE_CXX11->const VkCommandBuffer&
    {
        return m_commandBuffer;
    }
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto BeginCommand() const BALBINO_NOEXCEPT_SINCE_CXX11->int32_t
    {
        constexpr VkCommandBufferBeginInfo commandBufferBeginInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = VK_NULL_HANDLE,
            .flags            = 0,       // Optional
            .pInheritanceInfo = nullptr, // Optional
        };
        if (!CheckVkResult(vkBeginCommandBuffer(m_commandBuffer, &commandBufferBeginInfo)))
        {
            return -1;
        }
        return 0;
    }
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto EndCommand() const BALBINO_NOEXCEPT_SINCE_CXX11->int32_t
    {
        if (!CheckVkResult(vkEndCommandBuffer(m_commandBuffer)))
        {
            return -1;
        }
        return 0;
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void BeginRender() const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdBeginRenderingKHR(m_commandBuffer, &m_swapChain->GetRenderInfo());
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void EndRender() const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdEndRenderingKHR(m_commandBuffer);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetViewport(const float xPos, const float yPos, const float width, const float height, const float minDepth, const float maxDepth) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        const VkViewport viewport{.x = xPos, .y = yPos, .width = width, .height = height, .minDepth = minDepth, .maxDepth = maxDepth};
        vkCmdSetViewportWithCountEXT(m_commandBuffer, 1, &viewport);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetSissor(const uint32_t scissorWidth, const uint32_t scissorHeight, const int32_t scissorX, const int32_t scissorY) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        const VkRect2D scissor{.offset = {scissorX, scissorY}, .extent = {scissorWidth, scissorHeight}};
        vkCmdSetScissorWithCountEXT(m_commandBuffer, 1, &scissor);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetCullMode(const uint8_t cullmode) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetCullModeEXT(m_commandBuffer, cullmode);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetFrontFace(const bool isClockWice) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetFrontFaceEXT(m_commandBuffer, static_cast<VkFrontFace>(isClockWice));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetDepthTestEnable(const bool enable) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetDepthTestEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetDepthWriteEnable(const bool enable) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetDepthWriteEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetDepthCompareOp(uint8_t compareOp) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetDepthCompareOpEXT(m_commandBuffer, static_cast<VkCompareOp>(compareOp));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetPrimitiveTopology(uint8_t topology) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetPrimitiveTopologyEXT(m_commandBuffer, static_cast<VkPrimitiveTopology>(topology));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetRasterizerDiscardEnable(const bool enable) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetRasterizerDiscardEnableEXT(m_commandBuffer, enable ? VK_TRUE : VK_FALSE);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetPolygonMode(uint32_t mode) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetPolygonModeEXT(m_commandBuffer, static_cast<VkPolygonMode>(mode));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetRasterizationSamples(uint8_t sampleSize, const uint32_t mask) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        const VkSampleMask sampleMask{mask};
        vkCmdSetRasterizationSamplesEXT(m_commandBuffer, static_cast<VkSampleCountFlagBits>(sampleSize));
        vkCmdSetSampleMaskEXT(m_commandBuffer, static_cast<VkSampleCountFlagBits>(sampleSize), &sampleMask);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetAlphaToCoverageEnable(const bool enable) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetAlphaToCoverageEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetDepthBiasEnable(const bool enable) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetDepthBiasEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetStencilTestEnable(const bool enable) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetStencilTestEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void     SetPrimitiveRestartEnable(const bool enable) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetPrimitiveRestartEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetColorBlendEnable(const bool enable) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        const VkBool32 enableColorBlend{static_cast<const VkBool32>(enable)};
        vkCmdSetColorBlendEnableEXT(m_commandBuffer, 0, 1, &enableColorBlend);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetColorWriteMask(const uint32_t mask) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        const VkColorComponentFlags colorFlages{mask};
        vkCmdSetColorWriteMaskEXT(m_commandBuffer, 0, 1, &colorFlages);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void SetVertexInput(const VkVertexInputBindingDescription2EXT& vertexBind, const std::vector<VkVertexInputAttributeDescription2EXT>& vertexAttribute) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetVertexInputEXT(m_commandBuffer, 1, &vertexBind, static_cast<uint32_t>(vertexAttribute.size()), vertexAttribute.data());
    }

    // void BindDescriptorSets( uint8_t bindingPoint, const CVkDescriptorSet* pDescriptor ) const BALBINO_NOEXCEPT_SINCE_CXX11
    // {
    //     vkCmdBindDescriptorSets( m_commandBuffer, static_cast<VkPipelineBindPoint>( bindingPoint ), pDescriptor->GetPipelineHanle(), 0, 1, &pDescriptor->Handle(), 0, VK_NULL_HANDLE );
    // }

    BALBINO_CONSTEXPR_SINCE_CXX23 void BindShader(const CVkShader* pShader) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdBindShadersEXT(m_commandBuffer, pShader->GetShaderCount(), pShader->GetStages(), pShader->Handle());
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void MakeReadyToRender(const CVkSwapChain* pSwapChain) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        const VkImageMemoryBarrier imageMemoryBarrier{pSwapChain->GetImageRenderMemory()};
        vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void MakeReadyToPresent(const CVkSwapChain* pSwapChain) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        const VkImageMemoryBarrier imageMemoryBarrier{pSwapChain->GetImagePresentMemory()};
        vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier);
    }
    BALBINO_CONSTEXPR_SINCE_CXX23 void DrawFullscreen() const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkCmdSetPrimitiveRestartEnable(m_commandBuffer, VK_TRUE);
        vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);
    }

  private:
    const CVkSwapChain* m_swapChain{BALBINO_NULL};
    const VkCommandBuffer& m_commandBuffer{VK_NULL_HANDLE};
    const std::function<void(const VkCommandBuffer&, uint32_t)>& m_cleanup{};
    uint32_t m_count{};

    // PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{VK_NULL_HANDLE};
    // PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT{VK_NULL_HANDLE};
    // PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR{VK_NULL_HANDLE};
    // PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT{VK_NULL_HANDLE};
    // PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT{VK_NULL_HANDLE};
};
} // namespace DeerVulkan