//
// Copyright (c) 2026.
// Author: Joran.
//

module;
#include "api/vulkan/wrapper/command.hpp"
#include "api/vulkan/wrapper/device.hpp"
#include "api/vulkan/wrapper/fence.hpp"
#include "api/vulkan/wrapper/instance.hpp"
#include "api/vulkan/wrapper/physical_device.hpp"
#include "api/vulkan/wrapper/queue.hpp"
#include "api/vulkan/wrapper/semaphore.hpp"
#include "api/vulkan/wrapper/surface.hpp"
#include "api/vulkan/wrapper/swap_chain.hpp"
#include "deer_vulkan_core.hpp"

export module FawnVision:Renderer;
import :Enum;
import :Window;
import FawnAlgebra;

import std;

namespace fawn_vision
{
export constexpr std::uint8_t g_graphicsQueueId{0};
export constexpr std::uint8_t g_computeQueueId{1};
export constexpr std::uint8_t g_presentQueueId{2};
export constexpr std::uint8_t g_queueCount{3};

#define GFX_CHECK(expr, ...)                                                                                                                                                       \
    if (deer_vulkan::vk_status _s = expr; deer_vulkan::IsError(_s)) [[unlikely]]                                                                                                   \
    {                                                                                                                                                                              \
        auto _e = deer_vulkan::GetError(_s);                                                                                                                                       \
        std::println(std::cerr, "[GFX] {}:{} — {} | Hint: {} | Context: {}", __FILE__, __LINE__, _e.message, _e.hint, std::format(__VA_ARGS__));                                   \
        return ToGfxStatus(_s);                                                                                                                                                    \
    }

// For void-returning helpers that still need to propagate
#define GFX_CHECK_VOID(expr, ...)                                                                                                                                                  \
    if (deer_vulkan::vk_status _s = expr; deer_vulkan::IsError(_s)) [[unlikely]]                                                                                                   \
    {                                                                                                                                                                              \
        auto _e = deer_vulkan::GetError(_s);                                                                                                                                       \
        std::println(std::cerr, "[GFX] {}:{} — {} | Hint: {} | Context: {}", __FILE__, __LINE__, _e.message, _e.hint, std::format(__VA_ARGS__));                                   \
    }

export struct Renderer
{
    deer_vulkan::Dispatch dispatch{};
    deer_vulkan::Instance instance{};
    deer_vulkan::PhysicalDevice physical{};
    deer_vulkan::Surface surface{};
    deer_vulkan::Device device{};
    std::array<deer_vulkan::Queue, g_queueCount> queue{};
    deer_vulkan::SwapChain swapChain{};
    deer_vulkan::Semaphore timelineSemaphore{};
    deer_vulkan::Semaphore binarySemaphore{};
    deer_vulkan::Fence fence{};
    deer_vulkan::CommandPool commandPool{};
    deer_vulkan::CommandBuffer commandBuffer{};
};

// ---------------------------------------------------------------------------
// Internal helpers — now return vk_status so callers can forward the code
// ---------------------------------------------------------------------------

[[nodiscard]] inline auto InitializeQueues(Renderer& renderer) noexcept -> gfx_status
{
    using namespace deer_vulkan;

    GFX_CHECK(Initialize(renderer.dispatch, renderer.device, renderer.physical.graphicsQueueFamily, renderer.physical.graphicsQueueIdx, renderer.queue[g_graphicsQueueId]),
              "graphics queue");
    GFX_CHECK(Initialize(renderer.dispatch, renderer.device, renderer.physical.computeQueueFamily, renderer.physical.computeQueueIdx, renderer.queue[g_computeQueueId]),
              "compute queue");
    GFX_CHECK(Initialize(renderer.dispatch, renderer.device, renderer.physical.presentQueueFamily, renderer.physical.presentQueueIdx, renderer.queue[g_presentQueueId]),
              "present queue");

    return gfx_status::ok;
}

[[nodiscard]] inline auto InitializeComponents(const Window& window, Renderer& renderer) noexcept -> gfx_status
{
    using namespace deer_vulkan;

    if (const auto status{InitializeQueues(renderer)}; status != gfx_status::ok) [[unlikely]]
    {
        return gfx_status::not_ok;
    }

    GFX_CHECK(Initialize(renderer.dispatch, renderer.device, renderer.surface, window.width, window.height, renderer.swapChain), "swapchain ({}x{})", window.width, window.height);
    GFX_CHECK(Initialize(renderer.dispatch, renderer.device, /*isTimeline=*/true, renderer.timelineSemaphore), "timeline semaphore");
    GFX_CHECK(Initialize(renderer.dispatch, renderer.device, /*isTimeline=*/false, renderer.binarySemaphore), "binary semaphore");
    GFX_CHECK(Initialize(renderer.dispatch, renderer.device, renderer.physical.graphicsQueueFamily, renderer.commandPool), "command pool (family={})",
              renderer.physical.graphicsQueueFamily);
    GFX_CHECK(CreateCommandBuffer(renderer.dispatch, renderer.device, renderer.commandPool, 1U, renderer.commandBuffer), "primary command buffer");

    return gfx_status::ok;
}

inline auto CleanupComponents(Renderer& renderer) noexcept -> void
{
    Cleanup(renderer.dispatch, renderer.device, renderer.commandPool, renderer.commandBuffer);
    Cleanup(renderer.dispatch, renderer.device, renderer.commandPool);
    Cleanup(renderer.dispatch, renderer.device, renderer.binarySemaphore);
    Cleanup(renderer.dispatch, renderer.device, renderer.timelineSemaphore);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

export [[nodiscard]] inline auto CreateRenderer(const Window& window, Renderer& renderer) noexcept -> gfx_status
{
    using namespace deer_vulkan;
    Initialize(renderer.dispatch);
    GFX_CHECK(Initialize(renderer.dispatch, renderer.instance), "instance creation");
    GFX_CHECK(Initialize(window.pWindow, renderer.instance, renderer.surface), "surface creation for window '{}'", std::bit_cast<size_t>(window.pWindow));
    GFX_CHECK(SelectPhysicalDevice(renderer.dispatch, renderer.instance, renderer.physical), "physical device selection");
    GetSurfaceCapabilities(renderer.dispatch, renderer.physical, renderer.surface);
    GFX_CHECK(Initialize(renderer.dispatch, renderer.physical, renderer.device), "logical device selection");

    return InitializeComponents(window, renderer);
}

export inline auto ReleaseRenderer(Renderer& renderer) noexcept -> void
{
    CleanupComponents(renderer);
    Cleanup(renderer.dispatch, renderer.device, renderer.swapChain);
    for (auto& q : renderer.queue)
    {
        Cleanup(q);
    }

    Cleanup(renderer.dispatch, renderer.device);
    Cleanup(renderer.instance, renderer.surface);
    Cleanup(renderer.dispatch, renderer.instance);
}

export [[nodiscard]] inline auto RecreateRenderer(const Window& window, Renderer& renderer) noexcept -> gfx_status
{
    WaitIdle(renderer.dispatch, renderer.queue[g_presentQueueId]);

    CleanupComponents(renderer);
    GetSurfaceCapabilities(renderer.dispatch, renderer.physical, renderer.surface);
    GFX_CHECK(Recreate(renderer.dispatch, renderer.device, renderer.surface, renderer.swapChain, window.width, window.height), "recreating swap chain");

    return InitializeComponents(window, renderer);
}
} // namespace fawn_vision
