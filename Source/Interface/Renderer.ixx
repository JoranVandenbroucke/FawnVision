//
// Copyright (c) 2024.
// Author: Joran.
//

module;

#include "API/Vulkan/DeerVulkan.hpp"

export module FawnVision.Renderer;

import FawnVision.WindowTypes;
import FawnVision.RendererTypes;
import FawnVision.RenderGraph;

namespace FawnVision
{
inline int InitializeComponents(const SWindow& window, SRenderer& renderer)
{
    if (Initialize(renderer.queue[g_graphicsQueueId], renderer.device, renderer.device.queueFamily.graphicsFamily, 0U) != 0)
    {
        return -1;
    }
    if (Initialize(renderer.queue[g_computeQueueId], renderer.device, renderer.device.queueFamily.graphicsFamily, 1U) != 0)
    {
        return -1;
    }
    if (Initialize(renderer.queue[g_presentQueueId], renderer.device, renderer.device.queueFamily.presentFamily, renderer.device.queueFamily.graphicsFamily == renderer.device.queueFamily.presentFamily ? 2U : 0U) != 0)
    {
        return -1;
    }
    if (Initialize(renderer.swapChain, renderer.device, renderer.surface, window.width, window.height) != 0)
    {
        return -1;
    }
    if (InitializeSemaphore(renderer.timelineSemaphore, renderer.device, true) != 0)
    {
        return -1;
    }
    if (InitializeSemaphore(renderer.binarySemaphore, renderer.device, false) != 0)
    {
        return -1;
    }
    if (Initialize(renderer.commandPool, renderer.device, renderer.device.queueFamily.graphicsFamily) != 0)
    {
        return -1;
    }
    if (CreateCommandBuffer(renderer.instance, renderer.device, renderer.commandPool, 1U, renderer.commandBuffer))
    {
        return -1;
    }
    return 0;
}

inline void CleanupComponents(SRenderer& renderer)
{
    CleanupBuffer(renderer.device, renderer.commandPool, renderer.commandBuffer);
    Cleanup(renderer.device, renderer.commandPool);
    Cleanup(renderer.device, renderer.binarySemaphore);
    Cleanup(renderer.device, renderer.timelineSemaphore);
    Cleanup(renderer.device, renderer.swapChain);
    Cleanup(renderer.queue[g_presentQueueId]);
    Cleanup(renderer.queue[g_computeQueueId]);
    Cleanup(renderer.queue[g_graphicsQueueId]);
}

auto BeginRender(SRenderer& renderer) noexcept -> int
{
    uint64_t currentSemaphoreValue{};
    if (Value(renderer.device, renderer.timelineSemaphore, currentSemaphoreValue) != 0)
    {
        return -1;
    }
    if (Wait(renderer.device, renderer.timelineSemaphore, currentSemaphoreValue) != 0)
    {
        return -1;
    }
    if (NextImage(renderer.device, renderer.swapChain, renderer.binarySemaphore) != 0)
    {
        return -1;
    }
    if (BeginCommand(renderer.commandBuffer) != 0)
    {
        return -1;
    }
    MakeReadyToRender(renderer.commandBuffer, renderer.swapChain);
    BeginRender(renderer.commandBuffer, renderer.swapChain);
    return 0;
}

auto EndRender(SRenderer& renderer) noexcept -> int
{
    EndRender(renderer.commandBuffer);
    MakeReadyToPresent(renderer.commandBuffer, renderer.swapChain);
    if (EndCommand(renderer.commandBuffer) != 0)
    {
        return -1;
    }
    if (Execute(renderer.device, renderer.queue[g_presentQueueId], renderer.timelineSemaphore, renderer.commandBuffer) != 0)
    {
        return -1;
    }
    if (Present(renderer.queue[g_presentQueueId], renderer.swapChain, renderer.binarySemaphore) != 0)
    {
        return -1;
    }
    NextFrame(renderer.swapChain);
    WaitIdle(renderer.queue[g_presentQueueId]);
    return 0;
}

export auto CreateRenderer(const SWindow& window, SRenderer& renderer) -> int32_t
{
    if (InitializeInstance(renderer.instance, "FixMe", VK_MAKE_VERSION(0, 1, 0), window.extensions, window.extensionCount) != 0)
    {
        return -1;
    }
    if (InitializeSurface(window.pWindow, renderer.instance, renderer.surface) != 0)
    {
        return -1;
    }
    if (InitializeDevice(renderer.device, renderer.instance, renderer.surface) != 0)
    {
        return -1;
    }
    if (InitializeComponents(window, renderer) != 0)
    {
        return -1;
    }
    return 0;
}

export inline void ReleaseRenderer(SRenderer& renderer)
{
    CleanupComponents(renderer);
    Cleanup(renderer.device);
    Cleanup(renderer.instance, renderer.surface);
    Cleanup(renderer.instance);
}

export inline auto RecreateRenderer(const SWindow& window, SRenderer& renderer)
{
    CleanupComponents(renderer);
    if (InitializeComponents(window, renderer) != 0)
    {
        return -1;
    }
    return 0;
}

export inline void RenderFrame(SRenderer& renderer, const SRenderGraph& renderGraph)
{
    BeginRender(renderer);
    ExecuteAll(renderer, renderGraph);
    EndRender(renderer);
}
} // namespace FawnVision
