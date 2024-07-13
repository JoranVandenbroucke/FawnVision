//
// Copyright (c) 2024.
// Author: Joran.
//

#pragma once
#include "../DeerVulkan_Core.hpp"

#include "VkDevice.hpp"

namespace DeerVulkan
{
struct SVkFence
{
    VkFence fence;
};

inline auto Initialize(const SVkDevice& device, SVkFence& fence)
{

    if (constexpr VkFenceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        !CheckVkResult(vkCreateFence(device.device, &createInfo, VK_NULL_HANDLE, &fence.fence)))
    {
        return -1;
    }
    return 0;
}
inline void Cleanup(const SVkDevice& device, SVkFence& fence)
{
    vkDestroyFence(device.device, fence.fence, nullptr);
    fence.fence = nullptr;
}
inline auto wait(const SVkDevice& device, const SVkFence& fence)
{
    if (!CheckVkResult(vkWaitForFences(device.device, 1, &fence.fence, VK_TRUE, UINT64_MAX)))
    {
        return -1;
    }
    return 0;
}
inline auto Reset(const SVkDevice& device, const SVkFence& fence)
{
    if (!CheckVkResult(vkResetFences(device.device, 1, &fence.fence)))
    {
        return -1;
    }
    return 0;
}
} // namespace DeerVulkan