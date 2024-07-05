//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once

#include "../DeerVulkan_Core.hpp"

#include "VkDevice.hpp"
#include "VkShader.hpp"
#include "VkSwapChain.hpp"

#include <cstdint>

namespace DeerVulkan
{
    class CVkCommandPool;
    class CVkCommandBuffer final
    {
    public:
        constexpr explicit CVkCommandBuffer(const CVkSwapChain& pSwapChain, const uint32_t count,
                                            void (CVkCommandPool::*pCleanup)(const VkCommandBuffer&, uint32_t)const)
            : m_swapChain{pSwapChain}
              , m_cleanup{pCleanup}
              , m_count{count}
        {
        }

        constexpr explicit CVkCommandBuffer(const CVkSwapChain& pSwapChain, const VkCommandBuffer& commandBuffer,
                                            const uint32_t count,
                                            void (CVkCommandPool::*pCleanup)(const VkCommandBuffer&, uint32_t)const)
            : m_swapChain{pSwapChain}
              , m_cleanup{pCleanup}
              , m_commandBuffer{commandBuffer}
              , m_count{count}
        {
        }

        ~CVkCommandBuffer()
        {
            Cleanup();
        }

        CVkCommandBuffer(const CVkCommandBuffer&) = delete;
        CVkCommandBuffer(CVkCommandBuffer&&) = delete;
        auto operator=(const CVkCommandBuffer&) -> CVkCommandBuffer& = delete;
        auto operator=(CVkCommandBuffer&&) -> CVkCommandBuffer& = delete;

        [[nodiscard]] constexpr auto Handle() const noexcept -> const VkCommandBuffer&
        {
            return m_commandBuffer;
        }

        constexpr void Initialize(const VkCommandBuffer& commandBuffer) noexcept
        {
            if (m_commandBuffer != VK_NULL_HANDLE)
            {
                return;
            }
            m_commandBuffer = commandBuffer;
        }

        constexpr void Cleanup() noexcept
        {
            if (m_commandBuffer == VK_NULL_HANDLE)
            {
                return;
            }
            m_cleanup(m_commandBuffer, m_count);
            m_commandBuffer = VK_NULL_HANDLE;
        }

        [[nodiscard]] constexpr auto BeginCommand() const noexcept -> int32_t
        {
            if (m_commandBuffer == VK_NULL_HANDLE)
            {
                return -1;
            }
            constexpr VkCommandBufferBeginInfo commandBufferBeginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = VK_NULL_HANDLE,
                .flags = 0, // Optional
                .pInheritanceInfo = nullptr, // Optional
            };
            if (!CheckVkResult(vkBeginCommandBuffer(m_commandBuffer, &commandBufferBeginInfo)))
            {
                return -1;
            }
            return 0;
        }

        [[nodiscard]] constexpr auto EndCommand() const noexcept -> int32_t
        {
            if (m_commandBuffer == VK_NULL_HANDLE)
            {
                return -1;
            }
            if (!CheckVkResult(vkEndCommandBuffer(m_commandBuffer)))
            {
                return -1;
            }
            return 0;
        }

        void BeginRender() const noexcept
        {
            vkCmdBeginRenderingKHR(m_commandBuffer, &m_swapChain.GetRenderInfo());
        }

        void EndRender() const noexcept
        {
            vkCmdEndRenderingKHR(m_commandBuffer);
        }

        void SetViewport(const float xPos, const float yPos, const float width, const float height,
                         const float minDepth, const float maxDepth) const noexcept
        {
            const VkViewport viewport{
                .x = xPos, .y = yPos, .width = width, .height = height, .minDepth = minDepth, .maxDepth = maxDepth
            };
            vkCmdSetViewportWithCountEXT(m_commandBuffer, 1, &viewport);
        }

        void SetSissor(const uint32_t scissorWidth, const uint32_t scissorHeight, const int32_t scissorX,
                       const int32_t scissorY) const noexcept
        {
            const VkRect2D scissor{.offset = {scissorX, scissorY}, .extent = {scissorWidth, scissorHeight}};
            vkCmdSetScissorWithCountEXT(m_commandBuffer, 1, &scissor);
        }

        void SetCullMode(const uint8_t cullmode) const noexcept
        {
            vkCmdSetCullModeEXT(m_commandBuffer, cullmode);
        }

        void SetFrontFace(const bool isClockWice) const noexcept
        {
            vkCmdSetFrontFaceEXT(m_commandBuffer, static_cast<VkFrontFace>(isClockWice));
        }

        void SetDepthTestEnable(const bool enable) const noexcept
        {
            vkCmdSetDepthTestEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
        }

        void SetDepthWriteEnable(const bool enable) const noexcept
        {
            vkCmdSetDepthWriteEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
        }

        void SetDepthCompareOp(uint8_t compareOp) const noexcept
        {
            vkCmdSetDepthCompareOpEXT(m_commandBuffer, static_cast<VkCompareOp>(compareOp));
        }

        void SetPrimitiveTopology(uint8_t topology) const noexcept
        {
            vkCmdSetPrimitiveTopologyEXT(m_commandBuffer, static_cast<VkPrimitiveTopology>(topology));
        }

        void SetRasterizerDiscardEnable(const bool enable) const noexcept
        {
            vkCmdSetRasterizerDiscardEnableEXT(m_commandBuffer, enable ? VK_TRUE : VK_FALSE);
        }

        void SetPolygonMode(uint32_t mode) const noexcept
        {
            vkCmdSetPolygonModeEXT(m_commandBuffer, static_cast<VkPolygonMode>(mode));
        }

        void SetRasterizationSamples(uint8_t sampleSize, const uint32_t mask) const noexcept
        {
            const VkSampleMask sampleMask{mask};
            vkCmdSetRasterizationSamplesEXT(m_commandBuffer, static_cast<VkSampleCountFlagBits>(sampleSize));
            vkCmdSetSampleMaskEXT(m_commandBuffer, static_cast<VkSampleCountFlagBits>(sampleSize), &sampleMask);
        }

        void SetAlphaToCoverageEnable(const bool enable) const noexcept
        {
            vkCmdSetAlphaToCoverageEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
        }

        void SetDepthBiasEnable(const bool enable) const noexcept
        {
            vkCmdSetDepthBiasEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
        }

        void SetStencilTestEnable(const bool enable) const noexcept
        {
            vkCmdSetStencilTestEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
        }

        void SetPrimitiveRestartEnable(const bool enable) const noexcept
        {
            vkCmdSetPrimitiveRestartEnableEXT(m_commandBuffer, static_cast<VkBool32>(enable));
        }

        void SetColorBlendEnable(const bool enable) const noexcept
        {
            const VkBool32 enableColorBlend{static_cast<const VkBool32>(enable)};
            vkCmdSetColorBlendEnableEXT(m_commandBuffer, 0, 1, &enableColorBlend);
        }

        void SetColorWriteMask(const uint32_t mask) const noexcept
        {
            const VkColorComponentFlags colorFlages{mask};
            vkCmdSetColorWriteMaskEXT(m_commandBuffer, 0, 1, &colorFlages);
        }

        void SetVertexInput(const VkVertexInputBindingDescription2EXT& vertexBind,
                            const std::vector<VkVertexInputAttributeDescription2EXT>& vertexAttribute) const noexcept
        {
            vkCmdSetVertexInputEXT(m_commandBuffer, 1, &vertexBind, static_cast<uint32_t>(vertexAttribute.size()),
                                   vertexAttribute.data());
        }

        // void BindDescriptorSets( uint8_t bindingPoint, const CVkDescriptorSet* pDescriptor ) const noexcept
        // {
        //     vkCmdBindDescriptorSets( m_commandBuffer, static_cast<VkPipelineBindPoint>( bindingPoint ), pDescriptor->GetPipelineHanle(), 0, 1, &pDescriptor->Handle(), 0, VK_NULL_HANDLE );
        // }

        void BindShader(const CVkShader* pShader) const noexcept
        {
            vkCmdBindShadersEXT(m_commandBuffer, pShader->GetShaderCount(), pShader->GetStages(), pShader->Handle());
        }

        void MakeReadyToRender(const CVkSwapChain& swapChain) const noexcept
        {
            const VkImageMemoryBarrier imageMemoryBarrier{swapChain.GetImageRenderMemory()};
            vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE,
                                 1, &imageMemoryBarrier);
        }

        void MakeReadyToPresent(const CVkSwapChain& pSwapChain) const noexcept
        {
            const VkImageMemoryBarrier imageMemoryBarrier{pSwapChain.GetImagePresentMemory()};
            vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1,
                                 &imageMemoryBarrier);
        }

        void DrawFullscreen() const noexcept
        {
            vkCmdSetPrimitiveRestartEnable(m_commandBuffer, VK_TRUE);
            vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);
        }

    private:
        const CVkSwapChain& m_swapChain;
        void (CVkCommandPool::*m_cleanup)(const VkCommandBuffer&, uint32_t)const;
        VkCommandBuffer m_commandBuffer{VK_NULL_HANDLE};
        const uint32_t m_count;

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
