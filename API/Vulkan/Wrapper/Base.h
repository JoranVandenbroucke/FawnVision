//
// Created by joran on 02/01/2024.
//

#pragma once
#include <atomic>

#include "../DeerVulkan_Core.h"

namespace DeerVulkan
{
    class CVkDevice;

    class CRefCounted
    {
    public:
        CRefCounted() = default;

        CRefCounted( CRefCounted&& ) noexcept
        {
            /* refcount not copied on purpose*/
        }

        CRefCounted& operator=( CRefCounted&& ) noexcept
        {
            /* refcount not copied on purpose*/
            return *this;
        }

        virtual ~CRefCounted() = default;

        uint32_t AddRef()
        {
            return m_refCount.fetch_add( 1 );
        }

        uint32_t Release()
        {
            if ( m_refCount.fetch_sub( 1 ) == 1 )
            {
                Destroy();
                return 0;
            }
            return m_refCount;
        }

        bool IsUniquelyOwned() const
        {
            return m_refCount == 1;
        }

    protected:
        virtual void Destroy()
        {
            delete this;
        }

    private:
        std::atomic<uint32_t> m_refCount{1};
    };

    class CDeviceObject : public CRefCounted
    {
    public:
        explicit CDeviceObject( CVkDevice* pDevice )
            : m_device{pDevice}
        {}

        ~CDeviceObject() override = default;

        const CVkDevice* GetDevice() const noexcept        {
            return m_device;
        }

    private:
        CVkDevice* m_device{BALBINO_NULL};
    };
}// namespace DeerVulkan
