//
// Created by Joran on 09/01/2024.
//

#include "VkFence.h"

namespace DeerVulkan
{
    int32_t CVkFence::Initialize( const int createCount ) noexcept
    {
        constexpr VkFenceCreateInfo createInfo {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = VK_NULL_HANDLE,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        m_fences.reserve( createCount );
        for ( int i = 0; i < createCount; ++i )
        {
            VkFence fence { VK_NULL_HANDLE };
            if ( !CheckVkResult( vkCreateFence( GetDevice()->GetVkDevice(), &createInfo, VK_NULL_HANDLE, &fence ) ) )
            {
                return -1;
            }
            m_fences.emplace_back( fence );
        }
        return 0;
    }
}// namespace DeerVulkan
