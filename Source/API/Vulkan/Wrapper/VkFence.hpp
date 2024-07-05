//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"

#include "VkDevice.hpp"

namespace DeerVulkan
{
class CVkFence final
{
public:
    constexpr explicit CVkFence(const CVkDevice& pDevice)
        : m_pDevice{pDevice}
    {
    }

    ~CVkFence()
    {
        vkDestroyFence(m_pDevice.Device(), m_fence, VK_NULL_HANDLE);
    }

    CVkFence(const CVkFence& other) = delete;
    CVkFence(CVkFence&& other) noexcept = delete;
    auto operator=(const CVkFence& other) -> CVkFence& = delete;
    auto operator=(CVkFence&& other) -> CVkFence& = delete;

    [[nodiscard]] auto Initialize() noexcept -> int32_t
    {
        constexpr VkFenceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        if (!CheckVkResult(vkCreateFence(m_pDevice.Device(), &createInfo, VK_NULL_HANDLE, &m_fence)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] auto Wait() const noexcept -> int
    {
        if (!CheckVkResult(vkWaitForFences(m_pDevice.Device(), 1, &m_fence, VK_TRUE, UINT64_MAX)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] auto Reset() const noexcept -> int
    {
        if (!CheckVkResult(vkResetFences(m_pDevice.Device(), 1, &m_fence)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr auto Handle() const noexcept -> VkFence
    {
        return m_fence;
    }

private:
    const CVkDevice& m_pDevice;
    VkFence m_fence{VK_NULL_HANDLE};
};

} // namespace DeerVulkan