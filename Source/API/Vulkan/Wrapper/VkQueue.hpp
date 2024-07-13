//
// Copyright (c) 2024.
// Author: Joran.
//

#pragma once
#include "../DeerVulkan_Core.hpp"
#include "VkDevice.hpp"
#include "VkSemaphore.hpp"
#include "VkSwapChain.hpp"

namespace DeerVulkan
{
struct SVkQueue
{
    VkQueue queue;
};

inline auto Initialize(SVkQueue& queue, const SVkDevice& device, const uint32_t family, const uint32_t index)
{
    const VkDeviceQueueInfo2 info{
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
        .pNext            = VK_NULL_HANDLE,
        .flags            = 0,
        .queueFamilyIndex = family,
        .queueIndex       = index,
    };
    vkGetDeviceQueue2(device.device, &info, &queue.queue);
    return 0;
}
inline void Cleanup(SVkQueue& queue)
{
    queue.queue = nullptr;
}

inline void WaitIdle(const SVkQueue& queue)
{
    vkQueueWaitIdle(queue.queue);
}
inline auto Present(const SVkQueue& queue, const SVkSwapChain& swapChain, const SVkSemaphore& waitSemaphore)
{

    if (const VkPresentInfoKHR info{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext              = VK_NULL_HANDLE,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &waitSemaphore.semaphore,
            .swapchainCount     = 1U,
            .pSwapchains        = &swapChain.swapchain,
            .pImageIndices      = &swapChain.currentFrameIdx,
            .pResults           = VK_NULL_HANDLE,
        };
        !CheckVkResult(vkQueuePresentKHR(queue.queue, &info)))
    {
        return -1;
    }
    return 0;
}
inline auto Execute(const SVkDevice& device, const SVkQueue& queue, const SVkSemaphore& semaphore, const SVkCommandBuffer& commandBuffer)
{
    uint64_t waitValue{};
    if (Value(device, semaphore, waitValue) != 0)
    {
        return -1;
    }
    const uint64_t signalValue{waitValue + 1U};
    const VkSemaphoreSubmitInfo waitSemaphoreInfo{
        .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext       = VK_NULL_HANDLE,
        .semaphore   = semaphore.semaphore,
        .value       = waitValue,
        .stageMask   = 0,
        .deviceIndex = 0,
    };
    const VkSemaphoreSubmitInfo submitSemaphoreInfo{
        .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext       = VK_NULL_HANDLE,
        .semaphore   = semaphore.semaphore,
        .value       = signalValue,
        .stageMask   = 0,
        .deviceIndex = 0,
    };
    const VkCommandBufferSubmitInfo commandBufferSubmitInfo{
        .sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext         = VK_NULL_HANDLE,
        .commandBuffer = commandBuffer.commandBuffer,
        .deviceMask    = 0,
    };
    const VkSubmitInfo2 info{
        .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext                    = VK_NULL_HANDLE,
        .flags                    = 0,
        .waitSemaphoreInfoCount   = 1,
        .pWaitSemaphoreInfos      = &waitSemaphoreInfo,
        .commandBufferInfoCount   = 1,
        .pCommandBufferInfos      = &commandBufferSubmitInfo,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos    = &submitSemaphoreInfo,
    };
    if (!CheckVkResult(vkQueueSubmit2(queue.queue, 1, &info, VK_NULL_HANDLE)))
    {
        return -1;
    }
    return 0;
}
} // namespace DeerVulkan