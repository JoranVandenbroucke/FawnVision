//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "Base.h"
#include "VkDevice.h"

namespace DeerVulkan
{
class CVkFence final : public CDeviceObject
{
public:
    explicit CVkFence(const CVkDevice* pDevice)
        : CDeviceObject(pDevice)
    {
    }

    ~CVkFence() override
    {
        vkDestroyFence(Device()->VkDevice(), m_fence, VK_NULL_HANDLE);
    }

    auto Initialize() noexcept -> int32_t
    {
        constexpr VkFenceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        if (!CheckVkResult(vkCreateFence(Device()->VkDevice(), &createInfo, VK_NULL_HANDLE, &m_fence)))
        {
            return -1;
        }
        return 0;
    }

    auto Wait() const noexcept -> int
    {
        if (!CheckVkResult(vkWaitForFences(Device()->VkDevice(), 1, &m_fence, VK_TRUE, UINT64_MAX)))
        {
            return -1;
        }
        return 0;
    }

    auto Reset() const noexcept -> int
    {
        if (!CheckVkResult(vkResetFences(Device()->VkDevice(), 1, &m_fence)))
        {
            return -1;
        }
        return 0;
    }

    auto Handle() const noexcept -> VkFence
    {
        return m_fence;
    }

private:
    VkFence m_fence{VK_NULL_HANDLE};
};
} // namespace DeerVulkan