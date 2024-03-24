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
    BALBINO_CONSTEXPR_SINCE_CXX20 BALBINO_EXPLICIT_SINCE_CXX11 CVkFence::CVkFence(const CVkDevice* pDevice)
        : m_pDevice{pDevice}
    {
    }
    BALBINO_CONSTEXPR_SINCE_CXX20 CVkFence::~CVkFence()
    {
        vkDestroyFence(m_pDevice->Device(), m_fence, VK_NULL_HANDLE);
    };

    CVkFence(const CVkFence& other)                    = delete;
    CVkFence(CVkFence&& other) noexcept                = delete;
    auto operator=(const CVkFence& other) -> CVkFence& = delete;
    auto operator=(CVkFence&& other) -> CVkFence&      = delete;

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Initialize() noexcept -> int32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Wait() const noexcept -> int;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Reset() const noexcept -> int;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Handle() const noexcept -> VkFence;

  private:
    const CVkDevice* m_pDevice{BALBINO_NULL};
    VkFence m_fence{VK_NULL_HANDLE};
};

BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkFence::Initialize() noexcept -> int32_t
{
    BALBINO_CONSTEXPR VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    if (!CheckVkResult(vkCreateFence(m_pDevice->Device(), &createInfo, VK_NULL_HANDLE, &m_fence)))
    {
        return -1;
    }
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkFence::Wait() const noexcept -> int
{
    if (!CheckVkResult(vkWaitForFences(m_pDevice->Device(), 1, &m_fence, VK_TRUE, UINT64_MAX)))
    {
        return -1;
    }
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkFence::Reset() const noexcept -> int
{
    if (!CheckVkResult(vkResetFences(m_pDevice->Device(), 1, &m_fence)))
    {
        return -1;
    }
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkFence::Handle() const noexcept -> VkFence
{
    return m_fence;
}
} // namespace DeerVulkan