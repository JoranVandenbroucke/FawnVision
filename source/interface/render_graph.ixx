//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "api/vulkan/wrapper/command.hpp"
#include "api/vulkan/wrapper/queue.hpp"

export module FawnVision:RenderGraph;
import :Enum;
import :Texture;
import :Renderer;
import :RenderPass;
import :RenderPassContext;

import FawnAlgebra;

import std;

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

namespace fawn_vision
{

export struct RenderGraph;
export struct RenderPassHandle;

export struct RenderPassHandle
{
    std::uint64_t index{~0ULL};

    [[nodiscard]] constexpr auto IsValid() const noexcept -> bool
    {
        return index != ~0ULL;
    }
};

export struct RenderGraph
{
    std::vector<RenderPassBase*> passes{};
    std::vector<RenderPassBase*> compiled{};
    bool dirty{true};
};

// ---------------------------------------------------------------------------
// Pass registration
// ---------------------------------------------------------------------------

export template <class PassData>
auto SetRenderFunc(RenderGraph& renderGraph, RenderPassHandle& handle, std::function<void(const PassData*, const RenderPassContext&)>&& renderFunc) noexcept -> void
{
    if (RenderPassBase* base{renderGraph.passes[handle.index]}; base != nullptr) [[likely]]
    {
        static_cast<RenderPass<PassData>*>(base)->renderFunction = std::move(renderFunc);
    }
}

export template <class PassData>
[[nodiscard]] auto AddComputeRenderPass(RenderGraph& renderGraph, PassData*& passData) noexcept -> RenderPassHandle
{
    const std::size_t idx = renderGraph.passes.size();

    auto* rp   = new (std::nothrow) RenderPass<PassData>{};
    auto* data = new (std::nothrow) PassData{};
    if (!rp || !data) [[unlikely]]
    {
        delete rp;
        delete data;
        return {};
    }

    Initialize(rp, /*isCompute=*/true, static_cast<std::int32_t>(idx), data);
    passData = rp->data;
    renderGraph.passes.emplace_back(rp);
    renderGraph.dirty = true;
    return RenderPassHandle{idx};
}

export template <class PassData>
[[nodiscard]] auto AddRasterRenderPass(RenderGraph& renderGraph, PassData*& passData) noexcept -> RenderPassHandle
{
    const std::size_t idx = renderGraph.passes.size();

    auto* rp   = new (std::nothrow) RenderPass<PassData>{};
    auto* data = new (std::nothrow) PassData{};
    if (!rp || !data) [[unlikely]]
    {
        delete rp;
        delete data;
        return {};
    }

    Initialize(rp, /*isCompute=*/false, static_cast<std::int32_t>(idx), data);
    passData = rp->data;
    renderGraph.passes.emplace_back(rp);
    renderGraph.dirty = true;
    return RenderPassHandle{idx};
}

// ---------------------------------------------------------------------------
// Pass configuration
// ---------------------------------------------------------------------------

export inline auto EnableRenderPass(RenderGraph& renderGraph, const RenderPassHandle& handle) noexcept -> void
{
    if (RenderPassBase* pass{renderGraph.passes[handle.index]}; pass != nullptr) [[likely]]
    {
        pass->isEnabled   = true;
        renderGraph.dirty = true;
    }
}

export inline auto DisableRenderPass(RenderGraph& renderGraph, const RenderPassHandle& handle) noexcept -> void
{
    if (RenderPassBase* pass{renderGraph.passes[handle.index]}; pass != nullptr) [[likely]]
    {
        pass->isEnabled   = false;
        renderGraph.dirty = true;
    }
}

export inline auto SetRenderTarget(RenderGraph& renderGraph, const RenderPassHandle& handle, Texture& color, Texture& depth) noexcept -> void
{
    if (RenderPassBase* pass{renderGraph.passes[handle.index]}; pass != nullptr) [[likely]]
    {
        pass->colorImage     = &color.image;
        pass->colorImageView = &color.view;
        pass->depthImage     = &depth.image;
        pass->depthImageView = &depth.view;
        renderGraph.dirty    = true;
    }
}

export inline auto SetRenderColorTarget(RenderGraph& renderGraph, const RenderPassHandle& handle, Texture& color) noexcept -> void
{
    if (RenderPassBase* pass{renderGraph.passes[handle.index]}; pass != nullptr) [[likely]]
    {
        pass->colorImage     = &color.image;
        pass->colorImageView = &color.view;
        renderGraph.dirty    = true;
    }
}

export inline auto SetRenderDepthTarget(RenderGraph& renderGraph, const RenderPassHandle& handle, Texture& depth) noexcept -> void
{
    if (RenderPassBase* pass{renderGraph.passes[handle.index]}; pass != nullptr) [[likely]]
    {
        pass->depthImage     = &depth.image;
        pass->depthImageView = &depth.view;
        renderGraph.dirty    = true;
    }
}

// ---------------------------------------------------------------------------
// Execution
// ---------------------------------------------------------------------------

constexpr void PreRenderPass(const RenderPassContext& renderPassContext, const RenderPassBase* renderPass) noexcept
{
    deer_vulkan::BeginCommand(renderPassContext.dispatch, renderPassContext.commandBuffer);
    deer_vulkan::Image* color               = renderPass->colorImage;
    deer_vulkan::Image* depth               = renderPass->depthImage;
    const deer_vulkan::ImageView* colorView = renderPass->colorImageView;
    const deer_vulkan::ImageView* depthView = renderPass->depthImageView;
    if (!color && !depth)
    {
        deer_vulkan::vk_status _{NextImage(renderPassContext.dispatch, renderPassContext.device, renderPassContext.swapChain, renderPassContext.binarySemaphore)};
        color     = &CurrentImage(renderPassContext.swapChain);
        colorView = &CurrentImageView(renderPassContext.swapChain);
    }
    if (color)
    {
        TransitionImageLayout(renderPassContext.dispatch, renderPassContext.commandBuffer, *color, static_cast<std::uint32_t>(image_layout::color_attachment_optimal));
    }
    if (depth)
    {
        TransitionImageLayout(renderPassContext.dispatch, renderPassContext.commandBuffer, *depth, static_cast<std::uint32_t>(image_layout::depth_stencil_attachment_optimal));
    }

    deer_vulkan::BeginRender(renderPassContext.dispatch, renderPassContext.commandBuffer,
                             deer_vulkan::RenderParams{.colorImageView = colorView,
                                                       .depthImageView = depthView,
                                                       .clearColor     = {0, 0, 0},
                                                       .depthClear     = 0,
                                                       .xOffset        = 0,
                                                       .yOffset        = 0,
                                                       .width          = renderPassContext.swapChain.extent.width,
                                                       .height         = renderPassContext.swapChain.extent.height,
                                                       .stencilClear   = 0});
}
inline void PostRenderPass(const RenderPassContext& renderPassContext, const RenderPassBase* renderPass) noexcept
{
    deer_vulkan::EndRender(renderPassContext.dispatch, renderPassContext.commandBuffer);

    deer_vulkan::Image* color = renderPass->colorImage;
    deer_vulkan::Image* depth = renderPass->depthImage;

    const bool isSwapChain{!color && !depth};
    if (isSwapChain)
    {
        deer_vulkan::Image& swapChainImage{deer_vulkan::CurrentImage(renderPassContext.swapChain)};
        TransitionImageLayout(renderPassContext.dispatch, renderPassContext.commandBuffer, swapChainImage, static_cast<std::uint32_t>(image_layout::present_src_khr));
    }
    else
    {
        if (color)
        {
            TransitionImageLayout(renderPassContext.dispatch, renderPassContext.commandBuffer, *color, static_cast<std::uint32_t>(image_layout::shader_read_only_optimal));
        }
        if (depth)
        {
            TransitionImageLayout(renderPassContext.dispatch, renderPassContext.commandBuffer, *depth, static_cast<std::uint32_t>(image_layout::depth_stencil_attachment_optimal));
        }
    }

    // End the command buffer and handle possible failure
    deer_vulkan::EndCommand(renderPassContext.dispatch, renderPassContext.commandBuffer);

    // Determine the wait and signal values for synchronization
    const bool wait{(renderPass->syncMode & SYNC_WAIT) != 0};
    const bool signal{(renderPass->syncMode & SYNC_SIGNAL) != 0};

    if (wait)
    {
        if (signal)
        {
            if (deer_vulkan::QueueSubmit(renderPassContext.dispatch, *renderPassContext.queue, renderPassContext.commandBuffer, &renderPassContext.timelineSemaphore,
                                         renderPass->waitValue, renderPass->signalValue)
                != deer_vulkan::vk_status::ok)
            {
                return;
            }
        }
        else
        {
            if (deer_vulkan::QueueSubmit(renderPassContext.dispatch, *renderPassContext.queue, renderPassContext.commandBuffer, &renderPassContext.timelineSemaphore,
                                         renderPass->waitValue, true)
                != deer_vulkan::vk_status::ok)
            {
                return;
            }
        }
    }
    else if (signal)
    {
        if (deer_vulkan::QueueSubmit(renderPassContext.dispatch, *renderPassContext.queue, renderPassContext.commandBuffer, &renderPassContext.timelineSemaphore,
                                     renderPass->signalValue, false)
            != deer_vulkan::vk_status::ok)
        {
            return;
        }
    }
    else
    {
        if (deer_vulkan::QueueSubmit(renderPassContext.dispatch, *renderPassContext.queue, renderPassContext.commandBuffer) != deer_vulkan::vk_status::ok)
        {
            return;
        }
    }

    // Present to screen if no colour and depth image is provided
    if (isSwapChain)
    {
        if (deer_vulkan::Present(renderPassContext.dispatch, *renderPassContext.queue, renderPassContext.swapChain, renderPassContext.binarySemaphore)
            != deer_vulkan::vk_status::ok)
        {
            return;
        }

        deer_vulkan::NextFrame(renderPassContext.swapChain);
    }

    deer_vulkan::WaitIdle(renderPassContext.dispatch, *renderPassContext.queue);
}

export constexpr void ExecuteAll(Renderer& renderer, RenderGraph& renderGraph) noexcept
{
    if (renderGraph.dirty)
    {
        renderGraph.compiled.clear();
        for (RenderPassBase* renderPass : renderGraph.passes)
        {
            if (renderPass->isEnabled)
            {
                renderGraph.compiled.emplace_back(renderPass);
            }
        }
        std::ranges::sort(renderGraph.compiled,
                          [](const RenderPassBase* pA, const RenderPassBase* pB)
                          {
                              return pA->index < pB->index;
                          });
        renderGraph.dirty = false;
    }
    for (std::size_t i{}; i < renderGraph.compiled.size(); ++i)
    {
        RenderPassBase* renderPass{renderGraph.compiled[i]};
        const RenderPassContext renderContext{
            .dispatch          = renderer.dispatch,
            .device            = renderer.device,
            .commandBuffer     = renderer.commandBuffer,
            .swapChain         = renderer.swapChain,
            .timelineSemaphore = renderer.timelineSemaphore,
            .binarySemaphore   = renderer.binarySemaphore,
            .queue             = &renderer.queue[g_presentQueueId],
        };
        PreRenderPass(renderContext, renderPass);
        renderPass->Execute(renderContext);
        PostRenderPass(renderContext, renderPass);
    }
}

export constexpr void CleanupRenderGraph(RenderGraph& renderGraph) noexcept
{
    for (const RenderPassBase* renderPass : renderGraph.passes)
    {
        delete renderPass;
    }
    renderGraph.compiled.clear();
    renderGraph.passes.clear();
}
} // namespace fawn_vision
