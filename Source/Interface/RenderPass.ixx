//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "API/Vulkan/Wrapper/VkCommand.hpp"

#include <functional>

export module FawnVision.RenderPass;
import FawnVision.SharedObjectPool;

namespace FawnVision
{
export struct SRenderContext
{
    DeerVulkan::SVkCommandBuffer& commandBuffer;
};

export struct SRenderPassCore
{
    virtual ~SRenderPassCore()                  = default;
    virtual void Execute(const SRenderContext&) = 0;
    bool allowGlobalState{false};
    bool allowPassCulling{true};
    bool allowRendererListCulling{true};
    bool enableAsyncCompute{false};
    bool enableFoveatedRasterization{false};
    bool isCompute{false};
    int index{-1};
};

export template <class PassData>
struct SRenderPassBase : SRenderPassCore
{
    ~SRenderPassBase() override = default;
    std::function<void(const PassData*, const SRenderContext&)> renderFunction{};
    PassData* data{nullptr};
    void Execute(const SRenderContext& renderContext) override
    {
        if (data && renderFunction)
        {
            renderFunction(data, renderContext);
        }
    }
};

export template <class PassData>
struct SComputePass final : SRenderPassBase<PassData>
{
    ~SComputePass() override = default;
};

export template <class PassData>
struct SRasterPass final : SRenderPassBase<PassData>
{
    ~SRasterPass() override = default;
};

export template <class PassData>
struct SRenderPass final : SRenderPassBase<PassData>
{
    ~SRenderPass() override = default;
};

export template <class PassData>
constexpr void Initialize(SRenderPassBase<PassData>* renderPass, bool isComputePass, int passIndex, PassData* passData)
{
    renderPass->isCompute = isComputePass;
    renderPass->index     = passIndex;
    renderPass->data      = passData;
}

export template <class PassData>
void Execute(SRenderPassBase<PassData>* renderPass, const SRenderContext& renderContext)
{
    if (renderPass && renderPass->renderFunction)
    {
        renderPass->renderFunction(renderPass->data, renderContext);
    }
}

export template <class PassData>
void Release(SRenderPassBase<PassData>* renderPass)
{
    renderPass->allowGlobalState            = false;
    renderPass->allowPassCulling            = true;
    renderPass->allowRendererListCulling    = true;
    renderPass->enableAsyncCompute          = false;
    renderPass->enableFoveatedRasterization = false;
    renderPass->index                       = -1;
}
} // namespace FawnVision
