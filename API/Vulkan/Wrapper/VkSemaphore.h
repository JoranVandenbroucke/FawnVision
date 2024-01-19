//
// Created by Joran on 09/01/2024.
//

#pragma once
#include <vector>
#include "Base.h"
#include "VkDevice.h"

namespace DeerVulkan
{
    class CVkSemaphore final : public CDeviceObject
    {
    public:
        explicit CVkSemaphore( CVkDevice* pDevice )
            : CDeviceObject( pDevice )
        {}
        ~CVkSemaphore() override
        {
            std::ranges::for_each( m_semaphores,
                                   [ &device = GetDevice()->GetVkDevice() ]( VkSemaphore_T* semaphore )
                                   {
                                       vkDestroySemaphore( device, semaphore, VK_NULL_HANDLE );
                                   } );
            m_semaphores.clear();
        }
        int32_t Initialize( int createCount ) noexcept;
        VkSemaphore GetHandle( const uint32_t idx = 0 ) const noexcept
        {
            return m_semaphores[ idx ];
        }
        int GetCount() const noexcept
        {
            return m_semaphores.size();
        }

    private:
        std::vector<VkSemaphore> m_semaphores{};
    };
}// namespace DeerVulkan
