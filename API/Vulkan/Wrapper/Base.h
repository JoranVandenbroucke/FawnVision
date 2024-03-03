//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include <atomic>

namespace DeerVulkan
{
class CVkDevice;

class CRefCounted
{
public:
    CRefCounted() = default;
    BALBINO_CONSTEXPR_SINCE_CXX20 CRefCounted(CRefCounted&& /*unused*/) BALBINO_NOEXCEPT_SINCE_CXX11
    {
        /* refcount not copied on purpose*/
    }
    BALBINO_CONSTEXPR_SINCE_CXX20 CRefCounted(CRefCounted& /*unused*/) BALBINO_NOEXCEPT_SINCE_CXX11
    {
        /* refcount not copied on purpose*/
    }

    BALBINO_CONSTEXPR_SINCE_CXX20 auto operator=(CRefCounted&& /*unused*/) BALBINO_NOEXCEPT_SINCE_CXX11 -> CRefCounted&
    {
        /* refcount not copied on purpose*/
        return *this;
    }
    BALBINO_CONSTEXPR_SINCE_CXX20 auto operator=(const CRefCounted&  /*unused*/) BALBINO_NOEXCEPT_SINCE_CXX11 -> CRefCounted&
    {
        /* refcount not copied on purpose*/
        return *this;
    }


    virtual ~CRefCounted() = default;

    auto AddRef() BALBINO_NOEXCEPT_SINCE_CXX11 -> uint32_t
    {
        return m_refCount.fetch_add(1);
    }

    auto Release() BALBINO_NOEXCEPT_SINCE_CXX11 -> uint32_t
    {
        if (m_refCount.fetch_sub(1) == 1)
        {
            Destroy();
            return 0;
        }
        return m_refCount;
    }

    auto IsUniquelyOwned() const BALBINO_NOEXCEPT_SINCE_CXX11 -> bool
    {
        return m_refCount == 1;
    }

private:
    std::atomic<uint32_t> m_refCount{1};

    BALBINO_CONSTEXPR_SINCE_CXX20 void Destroy() const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        delete this;
    }
};

class CDeviceObject : public CRefCounted
{
public:
    BALBINO_EXPLICIT_SINCE_CXX11 CDeviceObject(const CVkDevice* pDevice)
        : m_device{pDevice}
    {
    }

    ~CDeviceObject() override = default;
    CDeviceObject(const CDeviceObject& other) = delete;
    CDeviceObject(CDeviceObject&& other) noexcept = delete;
    auto operator=(const CDeviceObject& other) -> CDeviceObject& = delete;
    auto operator=(CDeviceObject&& other) noexcept -> CDeviceObject& = delete;

    BALBINO_CONSTEXPR_SINCE_CXX20 auto Device() const BALBINO_NOEXCEPT_SINCE_CXX11 -> const CVkDevice*
    {
        return m_device;
    }

private:
    const CVkDevice* m_device{BALBINO_NULL};
};
} // namespace DeerVulkan