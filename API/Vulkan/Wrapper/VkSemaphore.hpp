//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "VkDevice.hpp"

namespace DeerVulkan
{
class CVkSemaphore final
{
  public:
    BALBINO_CONSTEXPR_SINCE_CXX20 BALBINO_EXPLICIT_SINCE_CXX11 CVkSemaphore(const CVkDevice* pDevice, const bool isTimeline);
    BALBINO_CONSTEXPR_SINCE_CXX20 ~CVkSemaphore();
    CVkSemaphore(const CVkSemaphore& other)                    = delete;
    CVkSemaphore(CVkSemaphore&& other) noexcept                = delete;
    auto operator=(const CVkSemaphore& other) -> CVkSemaphore& = delete;
    auto operator=(CVkSemaphore&& other) -> CVkSemaphore&      = delete;

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Initialize() BALBINO_NOEXCEPT_SINCE_CXX11->int32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Handle() const BALBINO_NOEXCEPT_SINCE_CXX11->const VkSemaphore&;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Value(uint64_t& value) const BALBINO_NOEXCEPT_SINCE_CXX11->int32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Wait(const uint64_t waitValue) BALBINO_NOEXCEPT_SINCE_CXX11->int32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto IsTimeline() const BALBINO_NOEXCEPT_SINCE_CXX11->bool;
    BALBINO_NODISCARD_SINCE_CXX17 static BALBINO_CONSTEXPR_SINCE_CXX11 auto SubmitInfo(const uint64_t& waitValue, const uint64_t& signalValue) BALBINO_NOEXCEPT_SINCE_CXX11->VkTimelineSemaphoreSubmitInfo;

  private:
    const CVkDevice* m_pDevice{BALBINO_NULL};
    VkSemaphore m_semaphore{VK_NULL_HANDLE};
    const bool m_isTimeline{};
};
BALBINO_CONSTEXPR_SINCE_CXX20 CVkSemaphore::CVkSemaphore(const CVkDevice* pDevice, const bool isTimeline)
    : m_pDevice{pDevice}
    , m_isTimeline{isTimeline}
{
}
BALBINO_CONSTEXPR_SINCE_CXX20 CVkSemaphore::~CVkSemaphore()
{
    vkDestroySemaphore(m_pDevice->Device(), m_semaphore, VK_NULL_HANDLE);
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkSemaphore::Initialize() BALBINO_NOEXCEPT_SINCE_CXX11->int32_t
{
    constexpr VkSemaphoreTypeCreateInfo timelineCreateInfo{
        .sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext         = VK_NULL_HANDLE,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue  = 0,
    };

    const VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = m_isTimeline ? &timelineCreateInfo : VK_NULL_HANDLE,
        .flags = 0,
    };
    if (!CheckVkResult(vkCreateSemaphore(m_pDevice->Device(), &createInfo, VK_NULL_HANDLE, &m_semaphore)))
    {
        return -1;
    }
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkSemaphore::Handle() const BALBINO_NOEXCEPT_SINCE_CXX11->const VkSemaphore&
{
    return m_semaphore;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkSemaphore::Value(uint64_t& value) const BALBINO_NOEXCEPT_SINCE_CXX11->int32_t
{
    if (!CheckVkResult(vkGetSemaphoreCounterValue(m_pDevice->Device(), m_semaphore, &value)))
    {
        return -1;
    }
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkSemaphore::Wait(const uint64_t waitValue) BALBINO_NOEXCEPT_SINCE_CXX11->int32_t
{
    const VkSemaphoreWaitInfo waitInfo{
        .sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pNext          = VK_NULL_HANDLE,
        .flags          = 0,
        .semaphoreCount = 1,
        .pSemaphores    = &m_semaphore,
        .pValues        = &waitValue,

    };
    if (!CheckVkResult(vkWaitSemaphores(m_pDevice->Device(), &waitInfo, UINT64_MAX)))
    {
        return -1;
    }
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkSemaphore::IsTimeline() const BALBINO_NOEXCEPT_SINCE_CXX11->bool
{
    return m_isTimeline;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkSemaphore::SubmitInfo(const uint64_t& waitValue, const uint64_t& signalValue) BALBINO_NOEXCEPT_SINCE_CXX11->VkTimelineSemaphoreSubmitInfo
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
} // namespace DeerVulkan