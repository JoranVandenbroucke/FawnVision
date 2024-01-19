//
// Created by Joran on 09/01/2024.
//

#pragma once
#include "Base.h"
#include "VkDevice.h"

namespace DeerVulkan
{
    class CVkFence final : public CDeviceObject
    {
    public:
        explicit CVkFence( CVkDevice* pDevice )
            : CDeviceObject( pDevice )
        {}

        ~CVkFence() override
        {
            std::ranges::for_each( m_fences,
                                   [ &device = GetDevice()->GetVkDevice() ]( VkFence_T* semaphore )
                                   {
                                       vkDestroyFence( device, semaphore, VK_NULL_HANDLE );
                                   } );
            m_fences.clear();
        }

        int32_t Initialize( int createCount ) noexcept;

        int Wait( const uint32_t idx = 0 ) const noexcept
        {
            if ( !CheckVkResult( vkWaitForFences( GetDevice()->GetVkDevice(), 1, &m_fences[ idx ], VK_TRUE, UINT64_MAX ) ) )
            {
                return -1;
            }
            return 0;
        }

        int Reset( const uint32_t idx = 0 ) const noexcept
        {
            if ( !CheckVkResult( vkResetFences( GetDevice()->GetVkDevice(), 1, &m_fences[ idx ] ) ) )
            {
                return -1;
            }
            return 0;
        }

        VkFence GetHandle( const uint32_t idx = 0 ) const noexcept
        {
            return m_fences[ idx ];
        }

    private:
        std::vector<VkFence> m_fences;
    };
}// namespace DeerVulkan
