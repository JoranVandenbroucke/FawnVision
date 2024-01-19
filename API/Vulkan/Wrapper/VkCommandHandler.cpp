//
// Created by Joran on 09/01/2024.
//

#include "VkCommandHandler.h"

#include "VkDevice.h"
#include "VkSwapChain.h"

namespace DeerVulkan
{
    CVkCommandHandler::~CVkCommandHandler()
    {
        vkFreeCommandBuffers( GetDevice()->GetVkDevice(), m_commandPool, m_commandBufferCount, &m_commandBuffer );
        vkDestroyCommandPool( GetDevice()->GetVkDevice(), m_commandPool, VK_NULL_HANDLE );
        m_swapChain->Release();
    }
    int32_t CVkCommandHandler::Initialize( CVkSwapChain* pSwapChain, const uint32_t queueFamilyIndex ) noexcept
    {
        m_swapChain = pSwapChain;
        m_swapChain->AddRef();
        const VkCommandPoolCreateInfo createInfo {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext            = VK_NULL_HANDLE,
                .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = queueFamilyIndex,
        };
        if ( !CheckVkResult( vkCreateCommandPool( GetDevice()->GetVkDevice(), &createInfo, VK_NULL_HANDLE, &m_commandPool ) ) )
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
        if ( !CheckVkResult( vkAllocateCommandBuffers( GetDevice()->GetVkDevice(), &allocInfo, &m_commandBuffer ) ) )
        {
            return -1;
        }
        vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>( vkGetDeviceProcAddr( GetDevice()->GetVkDevice(), "vkCmdBeginRenderingKHR" ) );
        vkCmdEndRenderingKHR   = reinterpret_cast<PFN_vkCmdEndRenderingKHR>( vkGetDeviceProcAddr( GetDevice()->GetVkDevice(), "vkCmdEndRenderingKHR" ) );
        return 0;
    }
    int32_t CVkCommandHandler::BeginCommand() const noexcept
    {
        constexpr VkCommandBufferBeginInfo commandBufferBeginInfo {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags            = 0,// Optional
                .pInheritanceInfo = nullptr,// Optional
        };
        if ( !CheckVkResult( vkBeginCommandBuffer( m_commandBuffer, &commandBufferBeginInfo ) ) )
        {
            return -1;
        }
        return 0;
    }
    int32_t CVkCommandHandler::EndCommand() const noexcept
    {
        if ( !CheckVkResult( vkEndCommandBuffer( m_commandBuffer ) ) )
        {
            return -1;
        }
        return 0;
    }
    void CVkCommandHandler::BeginRender() const noexcept
    {
        vkCmdBeginRenderingKHR( m_commandBuffer, &m_swapChain->GetRenderInfo() );
    }
    void CVkCommandHandler::EndRender() const noexcept
    {
        vkCmdEndRenderingKHR( m_commandBuffer );
    }

}// namespace DeerVulkan
