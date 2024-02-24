//
// Created by joran on 02/01/2024.
//

#pragma once
#include "../DeerVulkan_Core.h"

#include <atomic>

namespace DeerVulkan
{
    class CVkDevice;

    class CRefCounted
    {
    public:
        CRefCounted() = default;
        BALBINO_CONSTEXPR_SINCE_CXX20 CRefCounted( CRefCounted&& ) BALBINO_NOEXCEPT_SINCE_CXX11
        {
            /* refcount not copied on purpose*/
        }
        BALBINO_CONSTEXPR_SINCE_CXX20 CRefCounted& operator=( CRefCounted&& ) BALBINO_NOEXCEPT_SINCE_CXX11
        {
            /* refcount not copied on purpose*/
            return *this;
        }
        virtual ~CRefCounted() = default;
        uint32_t AddRef() BALBINO_NOEXCEPT_SINCE_CXX11
        {
            return m_refCount.fetch_add( 1 );
        }
        uint32_t Release() BALBINO_NOEXCEPT_SINCE_CXX11
        {
            if ( m_refCount.fetch_sub( 1 ) == 1 )
            {
                Destroy();
                return 0;
            }
            return m_refCount;
        }
        bool IsUniquelyOwned() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            return m_refCount == 1;
        }

    private:
        std::atomic<uint32_t> m_refCount { 1 };

        BALBINO_CONSTEXPR_SINCE_CXX20 void Destroy() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            delete this;
        }
    };

    class CDeviceObject : public CRefCounted
    {
    public:
        BALBINO_EXPLICIT_SINCE_CXX11 CDeviceObject( const CVkDevice* pDevice )
            : m_device { pDevice }
        {}

        ~CDeviceObject() override = default;

        BALBINO_CONSTEXPR_SINCE_CXX20 const CVkDevice* Device() const BALBINO_NOEXCEPT_SINCE_CXX11
        {
            return m_device;
        }

    private:
        const CVkDevice* m_device { BALBINO_NULL };
    };
}// namespace DeerVulkan
