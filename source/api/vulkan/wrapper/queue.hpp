#pragma once
#include "../deer_vulkan_core.hpp"

#include "command.hpp"
#include "device.hpp"
#include "dispatch.hpp"
#include "semaphore.hpp"
#include "swap_chain.hpp"

namespace deer_vulkan
{

struct Queue
{
    vk::Queue queue{nullptr};
    std::uint32_t familyIndex{};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const std::uint32_t familyIdx, const std::uint32_t queueIdx, Queue& queue) noexcept
    -> vk_status
{
    const vk::DeviceQueueInfo2 info{
        .sType            = vk::StructureType::eDeviceQueueInfo2,
        .pNext            = nullptr,
        .flags            = {},
        .queueFamilyIndex = familyIdx,
        .queueIndex       = queueIdx,
    };
    queue.queue       = device.device.getQueue2(info, dispatch.dispatch);
    queue.familyIndex = familyIdx;

    return vk_status::ok;
}

inline auto Cleanup(Queue& queue) noexcept -> void
{
    queue.queue = nullptr;
}

inline auto WaitIdle(const Dispatch& dispatch, const Queue& queue) noexcept -> void
{
    queue.queue.waitIdle(dispatch.dispatch);
}

[[nodiscard]] inline auto Present(const Dispatch& dispatch, const Queue& queue, const SwapChain& swapchain, const Semaphore& waitSemaphore) noexcept -> vk_status
{
    const vk::PresentInfoKHR presentInfo{
        .sType              = vk::StructureType::ePresentInfoKHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &waitSemaphore.semaphore,
        .swapchainCount     = 1,
        .pSwapchains        = &swapchain.swapChain,
        .pImageIndices      = &swapchain.currentFrameIdx,
        .pResults           = nullptr,
    };

    return FromVkResult(queue.queue.presentKHR(presentInfo, dispatch.dispatch));
}

[[nodiscard]] inline auto QueueSubmit(const Dispatch& dispatch, const Queue& queue, const CommandBuffer& commandBuffer, const Semaphore* pSemaphore = nullptr,
                                      const std::optional<uint64_t> waitValue = std::nullopt, const std::optional<uint64_t> signalValue = std::nullopt) noexcept -> vk_status
{
    const vk::CommandBufferSubmitInfo commandBufferSubmitInfo{
        .sType         = vk::StructureType::eCommandBufferSubmitInfo,
        .pNext         = nullptr,
        .commandBuffer = commandBuffer.commandBuffer.front(),
        .deviceMask    = 0,
    };

    const vk::SemaphoreSubmitInfo waitSemaphoreInfo{
        .sType       = vk::StructureType::eSemaphoreSubmitInfo,
        .pNext       = nullptr,
        .semaphore   = pSemaphore ? pSemaphore->semaphore : nullptr,
        .value       = waitValue.value_or(0),
        .stageMask   = {},
        .deviceIndex = 0,
    };

    const vk::SemaphoreSubmitInfo signalSemaphoreInfo{
        .sType       = vk::StructureType::eSemaphoreSubmitInfo,
        .pNext       = nullptr,
        .semaphore   = pSemaphore ? pSemaphore->semaphore : nullptr,
        .value       = signalValue.value_or(0),
        .stageMask   = {},
        .deviceIndex = 0,
    };

    const bool hasWait   = pSemaphore != nullptr && waitValue.has_value();
    const bool hasSignal = pSemaphore != nullptr && signalValue.has_value();
    const vk::SubmitInfo2 submitInfo{
        .sType                    = vk::StructureType::eSubmitInfo2,
        .pNext                    = nullptr,
        .flags                    = {},
        .waitSemaphoreInfoCount   = hasWait ? 1U : 0U,
        .pWaitSemaphoreInfos      = hasWait ? &waitSemaphoreInfo : nullptr,
        .commandBufferInfoCount   = 1,
        .pCommandBufferInfos      = &commandBufferSubmitInfo,
        .signalSemaphoreInfoCount = hasSignal ? 1U : 0U,
        .pSignalSemaphoreInfos    = hasSignal ? &signalSemaphoreInfo : nullptr,
    };
    return FromVkResult(queue.queue.submit2(1U, &submitInfo, nullptr, dispatch.dispatch));
}
} // namespace deer_vulkan
