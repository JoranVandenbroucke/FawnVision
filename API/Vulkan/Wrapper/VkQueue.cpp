#include "VkQueue.h"

#include "VkCommandHandler.h"
#include "VkDevice.h"
#include "VkFence.h"
#include "VkInstance.h"
#include "VkSemaphore.h"
#include "VkSwapChain.h"

namespace DeerVulkan
{
    int CVkQueue::Initialize( const uint32_t familyIndex ) noexcept
    {
        if ( m_queue )
        {
            return -1;
        }
        m_queueFamilyId = familyIndex;
        m_queuePoolId = 0;
        vkGetDeviceQueue( GetDevice()->GetVkDevice(), m_queueFamilyId, m_queuePoolId, &m_queue );
        return 0;
    }

    int CVkQueue::Present( const CVkSwapChain* pSwapChain, const CVkSemaphore* pWaitSemaphore, const uint32_t index ) const
    {
        SubmissionTask task{
                .data = {
                        .present = {
                                .swapChain = pSwapChain->GetHandle(),
                                .waitableSemaphores = pWaitSemaphore ? pWaitSemaphore->GetHandle( index ) : BALBINO_NULL,
                                .numWaitableSemaphores = pWaitSemaphore ? 1u : 0u,
                                .index = index,
                        },
                },
                .type = task_type::present
        };

        return AddTask<PresentCommandTask>( task );
    }

    int CVkQueue::Execute( const CVkCommandHandler* pCommandHandler, const CVkSemaphore* pWaitSemaphore, const CVkSemaphore* pSignalSemaphore, const CVkFence* pFence, const uint32_t imageIndex, uint32_t* pPilelineFlags ) const
    {
        SubmissionTask task{
                .data = {
                        .execute = {
                                .commandBuffer = pCommandHandler->GetHandle(),
                                .waitSemaphore = pWaitSemaphore ? pWaitSemaphore->GetHandle( imageIndex ) : BALBINO_NULL,
                                .signalSemaphore = pSignalSemaphore ? pSignalSemaphore->GetHandle( imageIndex ) : BALBINO_NULL,
                                .fence = pFence ? pFence->GetHandle( imageIndex ) : BALBINO_NULL,
                                .pilelineFlags = pPilelineFlags,
                                .nrWaisSemaphore = pWaitSemaphore ? 1u : 0u,
                                .nrSignalSemaphore = pSignalSemaphore ? 1u : 0u,
                        },
                },
                .type = task_type::execute,
        };

        return AddTask<ExecuteCommandTask>( task );
    }

    int CVkQueue::ExecuteCommandTask::Process( const VkQueue queue )
    {
        const VkSubmitInfo info{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = VK_NULL_HANDLE,
                .waitSemaphoreCount = nrWaisSemaphore,
                .pWaitSemaphores = &waitSemaphore,
                .pWaitDstStageMask = pilelineFlags,
                .commandBufferCount = 1,
                .pCommandBuffers = &commandBuffer,
                .signalSemaphoreCount = nrSignalSemaphore,
                .pSignalSemaphores = &signalSemaphore,
        };
        if ( !CheckVkResult( vkQueueSubmit( queue, 1, &info, fence ) ) )
        {
            return -1;
        }
        return 0;
    }

    int CVkQueue::PresentCommandTask::Process( const VkQueue queue )
    {
        const VkPresentInfoKHR info
        {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = VK_NULL_HANDLE,
                .waitSemaphoreCount = numWaitableSemaphores,
                .pWaitSemaphores = &waitableSemaphores,
                .swapchainCount = 1u,
                .pSwapchains = &swapChain,
                .pImageIndices = &index,
        };
        if ( !CheckVkResult( vkQueuePresentKHR( queue, &info ) ) )
        {
            return -1;
        }
        return 0;
    }

}// namespace DeerVulkan
