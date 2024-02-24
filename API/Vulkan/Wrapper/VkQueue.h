//
// Created by joran on 03/01/2024.
//

#pragma once
#include "Base.h"
#include "VkDevice.h"
#include "VkSemaphore.h"
#include "VkSwapChain.h"
#include <vulkan/vulkan.hpp>

namespace DeerVulkan
{
    class CVkQueue final : public CDeviceObject
    {
      public:
        explicit CVkQueue( const CVkDevice* device )
            : CDeviceObject { device }
        {
        }

        ~CVkQueue() override
        {
            m_queue = VK_NULL_HANDLE;
        }

        int32_t Initialize( const uint32_t familyIndex ) noexcept
        {
            if ( m_queue )
            {
                return -1;
            }
            m_queueFamilyId = familyIndex;
            m_queuePoolId   = 0;
            const VkDeviceQueueInfo2 info {
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
                .pNext            = VK_NULL_HANDLE,
                .flags            = 0,
                .queueFamilyIndex = m_queueFamilyId,
                .queueIndex       = m_queuePoolId,
            };
            vkGetDeviceQueue2( Device()->VkDevice(), &info, &m_queue );
            return 0;
        }

        void WaitIdle() const noexcept
        {
            vkQueueWaitIdle( m_queue );
        }

        int32_t Present( const CVkSwapChain* pSwapChain, const CVkSemaphore* pWaitSemaphore, uint32_t imageIndex ) const
        {
            const VkPresentInfoKHR info {
                .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext              = VK_NULL_HANDLE,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores    = &pWaitSemaphore->Handle(),
                .swapchainCount     = 1U,
                .pSwapchains        = &pSwapChain->Handle(),
                .pImageIndices      = &imageIndex,
                .pResults           = VK_NULL_HANDLE,
            };
            if ( !CheckVkResult( vkQueuePresentKHR( m_queue, &info ) ) )
            {
                return -1;
            }
            return 0;
        }

        int32_t Execute( const CVkSemaphore* pSemaphore, const CVkCommandHandler* pCommandHandle ) const
        {
            uint64_t waitValue;
            if ( pSemaphore->Value( waitValue ) )
            {
                return -1;
            }
            const uint64_t signalValue { waitValue + 1U };
            const VkSemaphoreSubmitInfo waitSemaphoreInfo {
                .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext       = VK_NULL_HANDLE,
                .semaphore   = pSemaphore->Handle(),
                .value       = waitValue,
                .stageMask   = 0,
                .deviceIndex = 0,
            };
            const VkSemaphoreSubmitInfo submitSemaphoreInfo {
                .sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext     = VK_NULL_HANDLE,
                .semaphore = pSemaphore->Handle(),
                .value     = signalValue,
                .stageMask   = 0,
                .deviceIndex = 0,
            };
            const VkCommandBufferSubmitInfo commandBufferSubmitInfo {
                .sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .pNext         = VK_NULL_HANDLE,
                .commandBuffer = pCommandHandle->Handle(),
                .deviceMask = 0,
            };
            const VkSubmitInfo2 info {
                .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                .pNext                    = VK_NULL_HANDLE,
                .flags                    = 0,
                .waitSemaphoreInfoCount   = 1,
                .pWaitSemaphoreInfos      = &waitSemaphoreInfo,
                .commandBufferInfoCount   = 1,
                .pCommandBufferInfos      = &commandBufferSubmitInfo,
                .signalSemaphoreInfoCount = 1,
                .pSignalSemaphoreInfos    = &submitSemaphoreInfo,
            };
            if ( !CheckVkResult( vkQueueSubmit2( m_queue, 1, &info, VK_NULL_HANDLE ) ) )
            {
                return -1;
            }
            return 0;
        }

      private:
        VkQueue m_queue { VK_NULL_HANDLE };
        uint32_t m_queueFamilyId {};
        uint32_t m_queuePoolId {};
    };
}// namespace DeerVulkan
