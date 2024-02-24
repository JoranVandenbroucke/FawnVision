//
// Created by Joran on 09/01/2024.
//

#pragma once
#include "../DeerVulkan_Core.h"
#include "Base.h"

#include "VkDevice.h"
#include "VkShader.h"
#include "VkSwapChain.h"

namespace DeerVulkan
{
    class CVkCommandHandler final : public CDeviceObject
    {
    public:
        BALBINO_EXPLICIT_SINCE_CXX11 CVkCommandHandler( const CVkDevice* pDevice )
            : CDeviceObject( pDevice )
        {}
        ~CVkCommandHandler() override
        {
            vkFreeCommandBuffers( Device()->VkDevice(), m_commandPool, m_commandBufferCount, &m_commandBuffer );
            vkDestroyCommandPool( Device()->VkDevice(), m_commandPool, VK_NULL_HANDLE );
            m_swapChain->Release();
        }
        BALBINO_NODISCARD_SINCE_CXX17 int32_t Initialize( CVkSwapChain* pSwapChain, const uint32_t queueFamilyIndex ) BALBINO_NOEXCEPT_SINCE_CXX11
        {
            m_swapChain = pSwapChain;
            m_swapChain->AddRef();
            const VkCommandPoolCreateInfo createInfo {
                    .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                    .pNext            = VK_NULL_HANDLE,
                    .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                    .queueFamilyIndex = queueFamilyIndex,
            };
            if ( !CheckVkResult( vkCreateCommandPool( Device()->VkDevice(), &createInfo, VK_NULL_HANDLE, &m_commandPool ) ) )
            {
                return -1;
            }
            const VkCommandBufferAllocateInfo allocInfo {
                    .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                    .pNext              = nullptr,
                    .commandPool        = m_commandPool,
                    .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                    .commandBufferCount = m_commandBufferCount,
            };
            if ( !CheckVkResult( vkAllocateCommandBuffers( Device()->VkDevice(), &allocInfo, &m_commandBuffer ) ) )
            {
                return -1;
            }
            vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdBeginRenderingKHR" ) );
            vkCmdEndRenderingKHR   = reinterpret_cast<PFN_vkCmdEndRenderingKHR>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdEndRenderingKHR" ) );

            vkCmdSetColorBlendEnableEXT        = reinterpret_cast<PFN_vkCmdSetColorBlendEnableEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetColorBlendEnableEXT" ) );
            vkCmdSetColorWriteMaskEXT          = reinterpret_cast<PFN_vkCmdSetColorWriteMaskEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetColorWriteMaskEXT" ) );
            vkCmdSetCullModeEXT                = reinterpret_cast<PFN_vkCmdSetCullModeEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetCullModeEXT" ) );
            vkCmdSetDepthBiasEnableEXT         = reinterpret_cast<PFN_vkCmdSetDepthBiasEnableEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetDepthBiasEnableEXT" ) );
            vkCmdSetDepthCompareOpEXT          = reinterpret_cast<PFN_vkCmdSetDepthCompareOpEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetDepthCompareOpEXT" ) );
            vkCmdSetDepthTestEnableEXT         = reinterpret_cast<PFN_vkCmdSetDepthTestEnableEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetDepthTestEnableEXT" ) );
            vkCmdSetDepthWriteEnableEXT        = reinterpret_cast<PFN_vkCmdSetDepthWriteEnableEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetDepthWriteEnableEXT" ) );
            vkCmdSetFrontFaceEXT               = reinterpret_cast<PFN_vkCmdSetFrontFaceEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetFrontFaceEXT" ) );
            vkCmdSetPolygonModeEXT             = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetPolygonModeEXT" ) );
            vkCmdSetPrimitiveRestartEnableEXT  = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnableEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetPrimitiveRestartEnableEXT" ) );
            vkCmdSetPrimitiveTopologyEXT       = reinterpret_cast<PFN_vkCmdSetPrimitiveTopologyEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetPrimitiveTopologyEXT" ) );
            vkCmdSetRasterizationSamplesEXT    = reinterpret_cast<PFN_vkCmdSetRasterizationSamplesEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetRasterizationSamplesEXT" ) );
            vkCmdSetRasterizerDiscardEnableEXT = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnableEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetRasterizerDiscardEnableEXT" ) );
            vkCmdSetSampleMaskEXT              = reinterpret_cast<PFN_vkCmdSetSampleMaskEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetSampleMaskEXT" ) );
            vkCmdSetScissorWithCountEXT        = reinterpret_cast<PFN_vkCmdSetScissorWithCountEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetScissorWithCountEXT" ) );
            vkCmdSetStencilTestEnableEXT       = reinterpret_cast<PFN_vkCmdSetStencilTestEnableEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetStencilTestEnableEXT" ) );
            vkCmdSetViewportWithCountEXT       = reinterpret_cast<PFN_vkCmdSetViewportWithCountEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetViewportWithCountEXT" ) );

            vkCmdSetVertexInputEXT = reinterpret_cast<PFN_vkCmdSetVertexInputEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdSetVertexInputEXT" ) );
            vkCmdBindShadersEXT    = reinterpret_cast<PFN_vkCmdBindShadersEXT>( vkGetDeviceProcAddr( Device()->VkDevice(), "vkCmdBindShadersEXT" ) );
            return 0;
        }
        BALBINO_NODISCARD_SINCE_CXX17 int32_t BeginCommand() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            constexpr VkCommandBufferBeginInfo commandBufferBeginInfo {
                    .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = VK_NULL_HANDLE,
                    .flags            = 0,// Optional
                    .pInheritanceInfo = nullptr,// Optional
            };
            if ( !CheckVkResult( vkBeginCommandBuffer( m_commandBuffer, &commandBufferBeginInfo ) ) )
            {
                return -1;
            }
            return 0;
        }
        BALBINO_NODISCARD_SINCE_CXX17 int32_t EndCommand() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            if ( !CheckVkResult( vkEndCommandBuffer( m_commandBuffer ) ) )
            {
                return -1;
            }
            return 0;
        }
        void BeginRender() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdBeginRenderingKHR( m_commandBuffer, &m_swapChain->GetRenderInfo() );
        }
        void EndRender() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdEndRenderingKHR( m_commandBuffer );
        }
        void SetViewport( const float x, const float y, const float w, const float h, const float minDepth = 0.0f, const float maxDepth = 1.0f ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            const VkViewport viewport { .x = x, .y = y, .width = w, .height = h, .minDepth = minDepth, .maxDepth = maxDepth };
            vkCmdSetViewportWithCountEXT( m_commandBuffer, 1, &viewport );
        }
        void SetSissor( const uint32_t scissorWidth, const uint32_t scissorHeight, const int32_t scissorX, const int32_t scissorY ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            const VkRect2D scissor { .offset = { scissorX, scissorY }, .extent = { scissorWidth, scissorHeight } };
            vkCmdSetScissorWithCountEXT( m_commandBuffer, 1, &scissor );
        }
        void SetCullMode( const uint8_t cullmode ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetCullModeEXT( m_commandBuffer, cullmode );
        }
        void SetFrontFace( const bool isClockWice ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetFrontFaceEXT( m_commandBuffer, static_cast<VkFrontFace>( isClockWice ) );
        }
        void SetDepthTestEnable( const bool enable ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetDepthTestEnableEXT( m_commandBuffer, enable );
        }
        void SetDepthWriteEnable( const bool enable ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetDepthWriteEnableEXT( m_commandBuffer, enable );
        }
        void SetDepthCompareOp( uint8_t compareOp ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetDepthCompareOpEXT( m_commandBuffer, static_cast<VkCompareOp>( compareOp ) );
        }
        void SetPrimitiveTopology( uint8_t topology ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetPrimitiveTopologyEXT( m_commandBuffer, static_cast<VkPrimitiveTopology>( topology ) );
        }
        void SetRasterizerDiscardEnable( const bool enable ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetRasterizerDiscardEnableEXT( m_commandBuffer, enable ? VK_TRUE : VK_FALSE );
        }
        void SetPolygonMode( uint32_t mode ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetPolygonModeEXT( m_commandBuffer, static_cast<VkPolygonMode>( mode ) );
        }
        void SetRasterizationSamples( uint8_t sampleSize, const uint32_t mask ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            const VkSampleMask sampleMask { mask };
            vkCmdSetRasterizationSamplesEXT( m_commandBuffer, static_cast<VkSampleCountFlagBits>( sampleSize ) );
            vkCmdSetSampleMaskEXT( m_commandBuffer, static_cast<VkSampleCountFlagBits>( sampleSize ), &sampleMask );
        }
        void SetAlphaToCoverageEnable( const bool enable ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetAlphaToCoverageEnableEXT( m_commandBuffer, enable );
        }
        void SetDepthBiasEnable( const bool enable ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetDepthBiasEnableEXT( m_commandBuffer, enable );
        }
        void SetStencilTestEnable( const bool enable ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetStencilTestEnableEXT( m_commandBuffer, enable );
        }
        void SetPrimitiveRestartEnable( const bool enable ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetPrimitiveRestartEnableEXT( m_commandBuffer, enable );
        }
        void SetColorBlendEnable( const bool enable ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            const VkBool32 enableColorBlend { enable };
            vkCmdSetColorBlendEnableEXT( m_commandBuffer, 0, 1, &enableColorBlend );
        }
        void SetColorWriteMask( const uint32_t mask ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            const VkColorComponentFlags colorFlages { mask };
            vkCmdSetColorWriteMaskEXT( m_commandBuffer, 0, 1, &colorFlages );
        }
        void SetVertexInput( const VkVertexInputBindingDescription2EXT& vertexBind, const std::vector<VkVertexInputAttributeDescription2EXT>& vertexAttribute ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdSetVertexInputEXT( m_commandBuffer, 1, &vertexBind, static_cast<uint32_t>( vertexAttribute.size() ), vertexAttribute.data() );
        }
        // void BindDescriptorSets( uint8_t bindingPoint, const CVkDescriptorSet* pDescriptor ) const BALBINO_NOEXCEPT_SINCE_CXX11
        // {
        //     vkCmdBindDescriptorSets( m_commandBuffer, static_cast<VkPipelineBindPoint>( bindingPoint ), pDescriptor->GetPipelineHanle(), 0, 1, &pDescriptor->Handle(), 0, VK_NULL_HANDLE );
        // }
        void BindShader( const CVkShader* pShader ) const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            vkCmdBindShadersEXT( m_commandBuffer, pShader->GetShaderCount(), pShader->GetStages(), pShader->Handle() );
        }
        void MakeReadyToRender( const CVkSwapChain* pSwapChain ) const noexcept
        {
            const VkImageMemoryBarrier imageMemoryBarrier { pSwapChain->GetImageRenderMemory() };
            vkCmdPipelineBarrier( m_commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier );
        }
        void MakeReadyToPresent( const CVkSwapChain* pSwapChain ) const noexcept
        {
            const VkImageMemoryBarrier imageMemoryBarrier { pSwapChain->GetImagePresentMemory() };
            vkCmdPipelineBarrier( m_commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier );
        }
        const VkCommandBuffer& Handle() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            return m_commandBuffer;
        }
        void DrawFullscreen() const
        {
            vkCmdSetPrimitiveRestartEnable (m_commandBuffer, VK_TRUE);
            vkCmdDraw( m_commandBuffer, 3, 1, 0, 0 );
        }

    private:
        VkCommandPool m_commandPool { VK_NULL_HANDLE };
        VkCommandBuffer m_commandBuffer { VK_NULL_HANDLE };
        CVkSwapChain* m_swapChain { BALBINO_NULL };

        PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR { VK_NULL_HANDLE };
        PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT { VK_NULL_HANDLE };
        PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR { VK_NULL_HANDLE };
        PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT { VK_NULL_HANDLE };
        PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT { VK_NULL_HANDLE };

        uint32_t m_commandBufferCount { 1 };
    };
}// namespace DeerVulkan
