//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"

#include "VkCommandBuffer.hpp"
#include "VkDevice.hpp"
#include "VkSwapChain.hpp"

#include <cstdint>

namespace DeerVulkan
{
class CVkCommandPool final
{
  public:
    BALBINO_CONSTEXPR_SINCE_CXX20 BALBINO_EXPLICIT_SINCE_CXX11 CVkCommandPool(const CVkDevice* pDevice, const CVkSwapChain* pSwapChain)
        : m_pDevice{pDevice}
        , m_swapChain{pSwapChain}
    {
    }
    BALBINO_CONSTEXPR_SINCE_CXX20 ~CVkCommandPool()
    {
        vkDestroyCommandPool(m_pDevice->Device(), m_commandPool, VK_NULL_HANDLE);
    }

    CVkCommandPool(const CVkCommandPool&)                    = delete;
    CVkCommandPool(CVkCommandPool&&)                         = delete;
    auto operator=(const CVkCommandPool&) -> CVkCommandPool& = delete;
    auto operator=(CVkCommandPool&&) -> CVkCommandPool&      = delete;

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Initialize(const uint32_t queueFamilyIndex) BALBINO_NOEXCEPT_SINCE_CXX11->int32_t
    {
        BALBINO_CONSTEXPR VkCommandPoolCreateInfo createInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext            = VK_NULL_HANDLE,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndex,
        };
        if (!CheckVkResult(vkCreateCommandPool(m_pDevice->Device(), &createInfo, VK_NULL_HANDLE, &m_commandPool)))
        {
            return -1;
        }
        return 0;
    }

    BALBINO_CONSTEXPR_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto CreateCommandBuffer(const uint32_t count) BALBINO_NOEXCEPT_SINCE_CXX11->CVkCommandBuffer
    {

        BALBINO_CONSTEXPR VkCommandBufferAllocateInfo allocInfo{
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = nullptr,
            .commandPool        = m_commandPool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count,
        };
        VkCommandBuffer pBuffer{VK_NULL_HANDLE};
        if (!CheckVkResult(vkAllocateCommandBuffers(m_pDevice->Device(), &allocInfo, &pBuffer)))
        {
            return CVkCommandBuffer{BALBINO_NULL, VK_NULL_HANDLE, 0, [this](const VkCommandBuffer& PH1, const uint32_t& PH2)
                                    {
                                        CleanupBuffer(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
                                    }};
        }
        return CVkCommandBuffer{m_swapChain, pBuffer, count, [this](const VkCommandBuffer& PH1, const uint32_t& PH2)
                                {
                                    CleanupBuffer(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
                                }};
    }

  private:
    const CVkDevice* m_pDevice{BALBINO_NULL};
    const CVkSwapChain* m_swapChain{BALBINO_NULL};
    VkCommandPool m_commandPool{VK_NULL_HANDLE};

    BALBINO_CONSTEXPR_SINCE_CXX23 void CleanupBuffer(const VkCommandBuffer& buffer, const uint32_t count) const BALBINO_NOEXCEPT_SINCE_CXX11
    {
        vkFreeCommandBuffers(m_pDevice->Device(), m_commandPool, count, &buffer);
    }
};

} // namespace DeerVulkan