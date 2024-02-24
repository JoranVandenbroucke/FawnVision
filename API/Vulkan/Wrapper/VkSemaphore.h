//
// Created by Joran on 09/01/2024.
//

#pragma once
#include "Base.h"
#include "VkDevice.h"

namespace DeerVulkan
{
    class CVkSemaphore final : public CDeviceObject
    {
    public:
        explicit CVkSemaphore( const CVkDevice* pDevice )
            : CDeviceObject( pDevice )
        {}
        ~CVkSemaphore() override
        {
            vkDestroySemaphore( Device()->VkDevice(), m_semaphore, VK_NULL_HANDLE );
        }
        int32_t Initialize( const bool isTimeline ) noexcept
        {
            constexpr VkSemaphoreTypeCreateInfo timelineCreateInfo {
                    .sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                    .pNext         = VK_NULL_HANDLE,
                    .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                    .initialValue  = 0,
            };

            const VkSemaphoreCreateInfo createInfo {
                    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                    .pNext = isTimeline ? &timelineCreateInfo : VK_NULL_HANDLE,
                    .flags = 0,
            };
            if ( !CheckVkResult( vkCreateSemaphore( Device()->VkDevice(), &createInfo, VK_NULL_HANDLE, &m_semaphore ) ) )
            {
                return -1;
            }
            m_isTimeline = isTimeline;
            return 0;
        }
        const VkSemaphore& Handle() const noexcept
        {
            return m_semaphore;
        }
        int32_t Value( uint64_t& value ) const noexcept
        {
            if ( !CheckVkResult( vkGetSemaphoreCounterValue( Device()->VkDevice(), m_semaphore, &value ) ) )
            {
                return -1;
            }
            return 0;
        }
        int32_t Wait( const uint64_t waitValue )
        {
            const VkSemaphoreWaitInfo waitInfo {
                    .sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
                    .pNext          = VK_NULL_HANDLE,
                    .flags          = 0,
                    .semaphoreCount = 1,
                    .pSemaphores    = &m_semaphore,
                    .pValues        = &waitValue,

            };
            if ( !CheckVkResult( vkWaitSemaphores( Device()->VkDevice(), &waitInfo, UINT64_MAX ) ) )
            {
                return -1;
            }
            return 0;
        }
        bool IsTimeline() const
        {
            return m_isTimeline;
        }
        static VkTimelineSemaphoreSubmitInfo SubmitInfo( const uint64_t& waitValue, const uint64_t& signalValue )
        {
            return {
                    .sType                     = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
                    .pNext                     = VK_NULL_HANDLE,
                    .waitSemaphoreValueCount   = 1,
                    .pWaitSemaphoreValues      = &waitValue,
                    .signalSemaphoreValueCount = 1,
                    .pSignalSemaphoreValues    = &signalValue,
            };
        }

    private:
        VkSemaphore m_semaphore {};
        bool m_isTimeline {};
    };
}// namespace DeerVulkan
