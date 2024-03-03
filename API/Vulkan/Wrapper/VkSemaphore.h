//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "Base.h"
#include "VkDevice.h"

namespace DeerVulkan
{
class CVkSemaphore final : public CDeviceObject
{
public:
    explicit CVkSemaphore(const CVkDevice* pDevice)
        : CDeviceObject(pDevice)
    {
    }

    ~CVkSemaphore() override
    {
        vkDestroySemaphore(Device()->VkDevice(), m_semaphore, VK_NULL_HANDLE);
    }

    auto Initialize(const bool isTimeline) noexcept -> int32_t
    {
        constexpr VkSemaphoreTypeCreateInfo timelineCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = 0,
        };

        const VkSemaphoreCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = isTimeline ? &timelineCreateInfo : VK_NULL_HANDLE,
            .flags = 0,
        };
        if (!CheckVkResult(vkCreateSemaphore(Device()->VkDevice(), &createInfo, VK_NULL_HANDLE, &m_semaphore)))
        {
            return -1;
        }
        m_isTimeline = isTimeline;
        return 0;
    }

    auto Handle() const noexcept -> const VkSemaphore&
    {
        return m_semaphore;
    }

    auto Value(uint64_t& value) const noexcept -> int32_t
    {
        if (!CheckVkResult(vkGetSemaphoreCounterValue(Device()->VkDevice(), m_semaphore, &value)))
        {
            return -1;
        }
        return 0;
    }

    auto Wait(const uint64_t waitValue) -> int32_t
    {
        const VkSemaphoreWaitInfo waitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = 0,
            .semaphoreCount = 1,
            .pSemaphores = &m_semaphore,
            .pValues = &waitValue,

        };
        if (!CheckVkResult(vkWaitSemaphores(Device()->VkDevice(), &waitInfo, UINT64_MAX)))
        {
            return -1;
        }
        return 0;
    }

    auto IsTimeline() const -> bool
    {
        return m_isTimeline;
    }

    static auto SubmitInfo(const uint64_t& waitValue, const uint64_t& signalValue) -> VkTimelineSemaphoreSubmitInfo
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
    VkSemaphore m_semaphore{};
    bool m_isTimeline{};
};
} // namespace DeerVulkan