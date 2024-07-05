//
// Copyright (c) 2024.
// Author: Joran
//

#include "Presenter.hpp"

namespace DeerVulkan
{
auto CPresenter::Initialize(const int32_t width, const int32_t height, const SQueueFamily& familyQueue) noexcept -> int32_t
{
    if (m_queue[g_graphicsQueueId].Initialize() != 0)
    {
        return -1;
    }
    if (m_queue[g_computeQueueId].Initialize() != 0)
    {
        return -1;
    }
    if (m_queue[g_presentQueueId].Initialize() != 0)
    {
        return -1;
    }
    if (m_swapChain.Initialize(width, height) != 0)
    {
        return -1;
    }
    if (m_timelineSemaphore.Initialize() != 0)
    {
        return -1;
    }
    if (m_binarySemaphore.Initialize() != 0)
    {
        return -1;
    }
    if (m_commandPool.Initialize(familyQueue.graphicsFamily) != 0)
    {
        return -1;
    }
    m_commandBuffer = m_commandPool.AllocateCommandBuffer();
    return 0;
}

void CPresenter::Cleanup() noexcept
{

}

auto CPresenter::BeginRender() noexcept -> int
{
    uint64_t currentSemaphoreValue{};
    if (m_timelineSemaphore.Value(currentSemaphoreValue) != 0)
    {
        return -1;
    }
    if (m_timelineSemaphore.Wait(currentSemaphoreValue) != 0)
    {
        return -1;
    }
    if (m_swapChain.NextImage(m_binarySemaphore) != 0)
    {
        return -1;
    }
    if (m_commandBuffer->BeginCommand() != 0)
    {
        return -1;
    }
    m_commandBuffer->MakeReadyToRender(m_swapChain);
    m_commandBuffer->BeginRender();
    return 0;
}

auto CPresenter::EndRender() noexcept -> int
{
    m_commandBuffer->EndRender();
    m_commandBuffer->MakeReadyToPresent(m_swapChain);
    if (m_commandBuffer->EndCommand() != 0)
    {
        return -1;
    }
    if (m_queue[g_presentQueueId].Execute(m_timelineSemaphore, *m_commandBuffer) != 0)
    {
        return -1;
    }
    if (m_queue[g_presentQueueId].Present(m_swapChain, m_binarySemaphore, m_swapChain.GetImageIndex()) != 0)
    {
        return -1;
    }
    m_swapChain.NextFrame();
    m_queue[g_presentQueueId].WaitIdle();
    return 0;
}
} // namespace DeerVulkan