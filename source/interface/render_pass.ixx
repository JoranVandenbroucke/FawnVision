//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "api/vulkan/wrapper/image.hpp"
#include "api/vulkan/wrapper/image_view.hpp"

export module FawnVision:RenderPass;
import :RenderPassContext;
import FawnAlgebra;

import std;

namespace fawn_vision
{
export constexpr std::uint8_t SYNC_NONE        = 0U;
export constexpr std::uint8_t SYNC_WAIT        = 1U;
export constexpr std::uint8_t SYNC_SIGNAL      = 2U;
export constexpr std::uint8_t SYNC_WAIT_SIGNAL = SYNC_WAIT | SYNC_SIGNAL;

export struct RenderPassBase
{
    virtual ~RenderPassBase()                                       = default;
    virtual auto Execute(const RenderPassContext&) noexcept -> void = 0;

    // Non-owning — lifetime managed by the texture/swapchain that owns them.
    deer_vulkan::Image* colorImage{nullptr};
    deer_vulkan::Image* depthImage{nullptr};
    const deer_vulkan::ImageView* colorImageView{nullptr};
    const deer_vulkan::ImageView* depthImageView{nullptr};

    std::uint64_t waitValue{0ULL};
    std::uint64_t signalValue{0ULL};
    std::uint32_t index{~0U};
    std::uint8_t syncMode{SYNC_NONE};
    bool isCompute{false};
    bool isEnabled{true};
};

export template <class PassData>
struct RenderPass final : RenderPassBase
{
    std::function<void(const PassData*, const RenderPassContext&)> renderFunction{};
    PassData* data{nullptr};

    auto Execute(const RenderPassContext& ctx) noexcept -> void override
    {
        if (data != nullptr && renderFunction) [[likely]]
        {
            renderFunction(data, ctx);
        }
    }
};

// Initialize a render pass — not constexpr, std::function is not a literal type.
export template <class PassData>
auto Initialize(RenderPass<PassData>* renderPass, const bool isComputePass, const std::int32_t passIndex, PassData* passData) noexcept -> void
{
    if (renderPass == nullptr) [[unlikely]]
    {
        return;
    }
    renderPass->isCompute = isComputePass;
    renderPass->index     = static_cast<std::uint32_t>(passIndex);
    renderPass->data      = passData;
}

// Release resets a pass back to default state without deallocating.
// Caller is responsible for deallocation (CleanupRenderGraph does this).
export template <class PassData>
auto Release(RenderPass<PassData>* renderPass) noexcept -> void
{
    if (renderPass == nullptr) [[unlikely]]
    {
        return;
    }
    renderPass->renderFunction = nullptr;
    renderPass->data           = nullptr;
    renderPass->colorImage     = nullptr;
    renderPass->depthImage     = nullptr;
    renderPass->colorImageView = nullptr;
    renderPass->depthImageView = nullptr;
    renderPass->waitValue      = 0ULL;
    renderPass->signalValue    = 0ULL;
    renderPass->index          = ~0U;
    renderPass->syncMode       = SYNC_NONE;
    renderPass->isCompute      = false;
    renderPass->isEnabled      = false;
}

} // namespace fawn_vision
