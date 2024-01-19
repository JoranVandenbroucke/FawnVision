//
// Created by Joran on 09/01/2024.
//

#include "VkSemaphore.h"

namespace DeerVulkan {
    int32_t CVkSemaphore::Initialize( const int createCount) noexcept
    {
        constexpr VkSemaphoreCreateInfo createInfor{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        m_semaphores.reserve(createCount);
        for ( int i = 0; i < createCount; ++i )
        {
            VkSemaphore semaphore{VK_NULL_HANDLE};
        if( !CheckVkResult( vkCreateSemaphore( GetDevice()->GetVkDevice(), &createInfor, VK_NULL_HANDLE, &semaphore )))
        {
            return -1;
        }
            m_semaphores.emplace_back(semaphore);
        }
        return 0;
    }
} // DeerVulkan