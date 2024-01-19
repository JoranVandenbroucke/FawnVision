//
// Created by joran on 03/01/2024.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include "Base.h"
#include "VkDevice.h"

namespace DeerVulkan
{
    class CVkFence;
    class CVkCommandHandler;
}// namespace DeerVulkan
namespace DeerVulkan
{
    class CVkSemaphore;
    class CVkSwapChain;
    struct SPhysicalDeviceInfo;

    class CVkQueue final : public CDeviceObject
    {
    public:
        explicit CVkQueue( CVkDevice* device )
            : CDeviceObject{device}
        {}

        ~CVkQueue() override
        {
            m_queue = VK_NULL_HANDLE;
        }

        int Initialize( uint32_t familyIndex ) noexcept;

        void WaitIdle() const noexcept
        {
            vkQueueWaitIdle( m_queue );
        }

        int Present( const CVkSwapChain* pSwapChain, const CVkSemaphore* pWaitSemaphore, uint32_t index ) const;

        int Execute( const CVkCommandHandler* pCommandHandler, const CVkSemaphore* pWaitSemaphore, const CVkSemaphore* pSignalSemaphore, const CVkFence* pFence, uint32_t imageIndex, uint32_t* pPilelineFlags ) const;

    private:
        enum class task_type : uint8_t
        {
            execute,
            present,
        };

        struct ExecuteCommandTask
        {
            VkCommandBuffer commandBuffer{VK_NULL_HANDLE};
            VkSemaphore waitSemaphore{VK_NULL_HANDLE};
            VkSemaphore signalSemaphore{VK_NULL_HANDLE};
            VkFence fence{VK_NULL_HANDLE};
            VkPipelineStageFlags* pilelineFlags{BALBINO_NULL};
            uint32_t nrWaisSemaphore{};
            uint32_t nrSignalSemaphore{};

            int Process( VkQueue queue );
        };

        struct PresentCommandTask
        {
            VkSwapchainKHR swapChain{VK_NULL_HANDLE};
            VkSemaphore waitableSemaphores{VK_NULL_HANDLE};
            uint32_t numWaitableSemaphores{};
            uint32_t index{};

            int Process( VkQueue queue );
        };

        struct SubmissionTask
        {
            union
            {
                ExecuteCommandTask execute;
                PresentCommandTask present;
            } data{};

            task_type type{};

            template<typename TaskType>
            int Process( VkQueue queue )
            {
                auto* pTask = reinterpret_cast<TaskType*>(&data);
                return pTask->Process( queue );
            }
        };

        VkQueue m_queue{VK_NULL_HANDLE};
        uint32_t m_queueFamilyId{};
        uint32_t m_queuePoolId{};

        template<typename TaskType>
        int AddTask( SubmissionTask& task ) const
        {
            return task.Process<TaskType>( m_queue );
        }
    };
}// namespace DeerVulkan
