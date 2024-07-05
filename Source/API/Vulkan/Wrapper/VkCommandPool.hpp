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
    constexpr explicit CVkCommandPool(const CVkDevice& pDevice, const CVkSwapChain& pSwapChain)
        : m_device{pDevice}
          , m_swapChain{pSwapChain}
    {
    }

    ~CVkCommandPool()
    {
        Cleanup();
        for (const auto& pBuffer : m_allocatedCommandBuffers)
        {
            delete pBuffer;
        }
        m_allocatedCommandBuffers.clear();
    }

    CVkCommandPool(const CVkCommandPool&) = delete;
    CVkCommandPool(CVkCommandPool&&) = delete;
    auto operator=(const CVkCommandPool&) -> CVkCommandPool& = delete;
    auto operator=(CVkCommandPool&&) -> CVkCommandPool& = delete;

    [[nodiscard]] constexpr auto Initialize(const uint32_t queueFamilyIndex) noexcept -> int32_t
    {
        const VkCommandPoolCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndex,
        };
        if (!CheckVkResult(vkCreateCommandPool(m_device.Device(), &createInfo, VK_NULL_HANDLE, &m_commandPool)))
        {
            return -1;
        }
        return 0;
    }

    constexpr void Cleanup() noexcept
    {
        for (const auto& pBuffer : m_allocatedCommandBuffers)
        {
            pBuffer->Cleanup();
        }
        vkDestroyCommandPool(m_device.Device(), m_commandPool, VK_NULL_HANDLE);
        m_commandPool = VK_NULL_HANDLE;
    }

    [[nodiscard]] constexpr auto CreateCommandBuffer(const uint32_t count = 1U) const noexcept -> const CVkCommandBuffer&&
    {
        const VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = m_commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count,
        };
        VkCommandBuffer pBuffer{VK_NULL_HANDLE};
        if (!CheckVkResult(vkAllocateCommandBuffers(m_device.Device(), &allocInfo, &pBuffer)))
        {
            return CVkCommandBuffer{m_swapChain, 0, &CVkCommandPool::EmptyCleanupCommandBuffer};
        }
        return CVkCommandBuffer{m_swapChain, pBuffer, count, &CVkCommandPool::CleanupCommandBuffer};
    }

    [[nodiscard]] constexpr auto AllocateCommandBuffer(const uint32_t count = 1) noexcept -> CVkCommandBuffer*
    {
        const VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = m_commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count,
        };
        VkCommandBuffer pBuffer{VK_NULL_HANDLE};
        if (!CheckVkResult(vkAllocateCommandBuffers(m_device.Device(), &allocInfo, &pBuffer)))
        {
            return nullptr;
        }
        m_allocatedCommandBuffers.push_back(new CVkCommandBuffer{m_swapChain, pBuffer, count, &CVkCommandPool::CleanupCommandBuffer});
        return m_allocatedCommandBuffers.back();
    }

private:
    const CVkDevice& m_device;
    const CVkSwapChain& m_swapChain;
    VkCommandPool m_commandPool{VK_NULL_HANDLE};

    std::vector<CVkCommandBuffer*> m_allocatedCommandBuffers;

    void CleanupCommandBuffer(const VkCommandBuffer& pCommandBuffer, const uint32_t bufferCount) const
    {
        vkFreeCommandBuffers(m_device.Device(), m_commandPool, bufferCount, &pCommandBuffer);
    }

    void EmptyCleanupCommandBuffer(const VkCommandBuffer& pCommandBuffer, const uint32_t bufferCount) const
    {
        (void)pCommandBuffer;
        (void)bufferCount;
    }
};

} // namespace DeerVulkan