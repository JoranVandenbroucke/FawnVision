//
// Copyright (c) 2024.
// Author: Joran.
//

export module FawnVision:DepthOnlyPass;
import :Enum;
import :RenderGraph;
import :RenderPassContext;
import :Shader;
import :Texture;

import FawnAlgebra;
using namespace fawn_algebra;

namespace fawn_vision
{
export struct DepthOnlyPass;
export inline void Initialize(RenderGraph& renderGraph, DepthOnlyPass& depthOnlyPass);

struct DepthPassData
{
    Texture depthTexture{};
    Shader depthShader{};
};
struct DepthOnlyPass
{
    DepthPassData* data{nullptr};
    RenderPassHandle renderPassHandle{};
};
inline void Initialize(RenderGraph& renderGraph, DepthOnlyPass& depthOnlyPass)
{
    if (depthOnlyPass.data != nullptr)
    {
        return;
    }

    depthOnlyPass.renderPassHandle = AddRasterRenderPass<DepthPassData>(renderGraph, depthOnlyPass.data);
    SetRenderFunc<DepthPassData>(renderGraph, depthOnlyPass.renderPassHandle,
                                 [](const DepthPassData* data, const RenderPassContext& ctx) noexcept
                                 {
                                     SetViewport(ctx, 0, 0, 1920, 1080);
                                     SetScissor(ctx, 1920, 1080, 0, 0);
                                     SetDepthTestEnable(ctx, true);
                                     SetDepthWriteEnable(ctx, true);
                                     SetColorWriteMask(ctx, static_cast<color_component>(0)); // Disable color writing

                                     BindShader(ctx, data->depthShader);

                                     // todo : get renderlist of meshes.

                                     // Draw(ctx, indexCount, 0, instanceCount, 0);
                                 });
}
} // namespace fawn_vision
