//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "VkCommandBuffer.hpp"
#include "VkDevice.hpp"
#include "VkSemaphore.hpp"
#include "VkSwapChain.hpp"

#include <vulkan/vulkan.hpp>

namespace DeerVulkan
{
class CVkQueue final
{
public:
    constexpr explicit CVkQueue(const CVkDevice& pDevice, const uint32_t familyIndex, const uint32_t poolIndex)
        : m_pDevice{pDevice}
          , m_queueFamilyId{familyIndex}
          , m_queuePoolId{poolIndex}
    {
    }

    constexpr ~CVkQueue()
    {
        m_queue = VK_NULL_HANDLE;
    }

    CVkQueue(const CVkQueue& other) = delete;
    CVkQueue(CVkQueue&& other) noexcept = delete;
    auto operator=(const CVkQueue& other) -> CVkQueue& = delete;
    auto operator=(CVkQueue&& other) -> CVkQueue& = delete;

    [[nodiscard]] constexpr auto Initialize() noexcept -> int32_t
    {
        if (m_queue != nullptr)
        {
            return -1;
        }
        const VkDeviceQueueInfo2 info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
            .pNext = VK_NULL_HANDLE,
            .flags = 0,
            .queueFamilyIndex = m_queueFamilyId,
            .queueIndex = m_queuePoolId,
        };
        vkGetDeviceQueue2(m_pDevice.Device(), &info, &m_queue);
        return 0;
    }

    void WaitIdle() const noexcept
    {
        vkQueueWaitIdle(m_queue);
    }

    [[nodiscard]] auto Present(const CVkSwapChain& pSwapChain, const CVkSemaphore& pWaitSemaphore, uint32_t imageIndex) const noexcept -> int32_t
    {
        const VkPresentInfoKHR info{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = VK_NULL_HANDLE,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &pWaitSemaphore.Handle(),
            .swapchainCount = 1U,
            .pSwapchains = &pSwapChain.Handle(),
            .pImageIndices = &imageIndex,
            .pResults = VK_NULL_HANDLE,
        };
        if (!CheckVkResult(vkQueuePresentKHR(m_queue, &info)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr auto Execute(const CVkSemaphore& pSemaphore, const CVkCommandBuffer& pCommandBuffer) const noexcept -> int32_t
    {
        uint64_t waitValue{};
        if (pSemaphore.Value(waitValue) != 0)
        {
            return -1;
        }
        const uint64_t signalValue{waitValue + 1U};
        const VkSemaphoreSubmitInfo waitSemaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = VK_NULL_HANDLE,
            .semaphore = pSemaphore.Handle(),
            .value = waitValue,
            .stageMask = 0,
            .deviceIndex = 0,
        };
        const VkSemaphoreSubmitInfo submitSemaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = VK_NULL_HANDLE,
            .semaphore = pSemaphore.Handle(),
            .value = signalValue,
            .stageMask = 0,
            .deviceIndex = 0,
        };
        const VkCommandBufferSubmitInfo commandBufferSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .pNext = VK_NULL_HANDLE,
            .commandBuffer = pCommandBuffer.Handle(),
            .deviceMask = 0,
        };
        const VkSubmitInfo2 info{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .pNext = VK_NULL_HANDLE,
            .flags = 0,
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &waitSemaphoreInfo,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &commandBufferSubmitInfo,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &submitSemaphoreInfo,
        };
        if (!CheckVkResult(vkQueueSubmit2(m_queue, 1, &info, VK_NULL_HANDLE)))
        {
            return -1;
        }
        return 0;
    }

private:
    const CVkDevice& m_pDevice;
    VkQueue m_queue{VK_NULL_HANDLE};
    const uint32_t m_queueFamilyId;
    const uint32_t m_queuePoolId;
};
} // namespace DeerVulkan