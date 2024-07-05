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
    constexpr explicit CVkSemaphore(const CVkDevice& pDevice, const bool isTimeline)
        : m_pDevice{pDevice}
          , m_isTimeline{isTimeline}
    {
    }

    ~CVkSemaphore()
    {
        vkDestroySemaphore(m_pDevice.Device(), m_semaphore, VK_NULL_HANDLE);
    }

    CVkSemaphore(const CVkSemaphore& other) = delete;
    CVkSemaphore(CVkSemaphore&& other) noexcept = delete;
    auto operator=(const CVkSemaphore& other) -> CVkSemaphore& = delete;
    auto operator=(CVkSemaphore&& other) -> CVkSemaphore& = delete;

    [[nodiscard]] constexpr auto Initialize() noexcept -> int32_t
    {
        constexpr VkSemaphoreTypeCreateInfo timelineCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = 0,
        };

        const VkSemaphoreCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = m_isTimeline ? &timelineCreateInfo : VK_NULL_HANDLE,
            .flags = 0,
        };
        if (!CheckVkResult(vkCreateSemaphore(m_pDevice.Device(), &createInfo, VK_NULL_HANDLE, &m_semaphore)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr auto Handle() const noexcept -> const VkSemaphore&
    {
        return m_semaphore;
    }

    [[nodiscard]] constexpr auto Value(uint64_t& value) const noexcept -> int32_t
    {
        if (!CheckVkResult(vkGetSemaphoreCounterValue(m_pDevice.Device(), m_semaphore, &value)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr auto Wait(const uint64_t waitValue) const noexcept -> int32_t
    {
        const VkSemaphoreWaitInfo waitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = 0,
            .semaphoreCount = 1,
            .pSemaphores = &m_semaphore,
            .pValues = &waitValue,

        };
        if (!CheckVkResult(vkWaitSemaphores(m_pDevice.Device(), &waitInfo, UINT64_MAX)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr auto IsTimeline() const noexcept -> bool
    {
        return m_isTimeline;
    }

    [[nodiscard]] static constexpr auto SubmitInfo(const uint64_t& waitValue, const uint64_t& signalValue) noexcept -> VkTimelineSemaphoreSubmitInfo
    {
        return {
            .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
            .pNext = VK_NULL_HANDLE,
            .waitSemaphoreValueCount = 1,
            .pWaitSemaphoreValues = &waitValue,
            .signalSemaphoreValueCount = 1,
            .pSignalSemaphoreValues = &signalValue,
        };
    }

private:
    const CVkDevice& m_pDevice;
    VkSemaphore m_semaphore{VK_NULL_HANDLE};
    const bool m_isTimeline{};
};

} // namespace DeerVulkan