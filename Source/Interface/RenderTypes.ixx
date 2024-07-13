//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include <array>
#include "API/Vulkan/DeerVulkan.hpp"

export module FawnVision.RendererTypes;

namespace FawnVision
{
export constexpr uint8_t g_graphicsQueueId{0};
export constexpr uint8_t g_computeQueueId{1};
export constexpr uint8_t g_presentQueueId{2};
export constexpr uint8_t g_queueCount{3};

export struct SRenderer
{
    DeerVulkan::SVkInstance instance;
    DeerVulkan::SVkSurface surface;
    DeerVulkan::SVkDevice device;

    std::array<DeerVulkan::SVkQueue, g_queueCount> queue;
    DeerVulkan::SVkSwapChain swapChain;
    DeerVulkan::SVkSemaphore timelineSemaphore;
    DeerVulkan::SVkSemaphore binarySemaphore;
    DeerVulkan::SVkFence fence;
    DeerVulkan::SVkCommandPool commandPool;
    DeerVulkan::SVkCommandBuffer commandBuffer;
};
} // namespace FawnVision
