//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "API/Vulkan/DeerVulkan.hpp"

#include <functional>
#include <list>

export module FawnVision.RenderGraph;
import FawnVision.SharedObjectPool;
import FawnVision.RendererTypes;
import FawnVision.RenderPass;
import FawnVision.Shader;

namespace FawnVision
{
export enum class compare_operator : uint8_t {
    never            = 0,
    less             = 1,
    equal            = 2,
    less_or_equal    = 3,
    greater          = 4,
    not_equal        = 5,
    greater_or_equal = 6,
    always           = 7,
};
export enum class cull_mode : uint8_t {
    none           = 0,
    front          = 1,
    back           = 2,
    front_and_back = 3,
};
export enum class primitive_topology : uint8_t {
    point_list                    = 0,
    line_list                     = 1,
    line_strip                    = 2,
    triangle_list                 = 3,
    triangle_strip                = 4,
    triangle_fan                  = 5,
    line_list_with_adjacency      = 6,
    line_strip_with_adjacency     = 7,
    triangle_list_with_adjacency  = 8,
    triangle_strip_with_adjacency = 9,
    patch_list                    = 10,
};
export enum class polygon_mode : uint8_t {
    fill  = 0,
    line  = 1,
    point = 2,
};
export enum class color_component : uint8_t {
    r_bit = 0x01,
    g_bit = 0x02,
    b_bit = 0x04,
    a_bit = 0x08,
};

export enum class blend_factor : uint32_t {
    zero                     = 0,
    one                      = 1,
    src_color                = 2,
    one_minus_src_color      = 3,
    dst_color                = 4,
    one_minus_dst_color      = 5,
    src_alpha                = 6,
    one_minus_src_alpha      = 7,
    dst_alpha                = 8,
    one_minus_dst_alpha      = 9,
    constant_color           = 10,
    one_minus_constant_color = 11,
    constant_alpha           = 12,
    one_minus_constant_alpha = 13,
    src_alpha_saturate       = 14,
    src1_color               = 15,
    one_minus_src1_color     = 16,
    src1_alpha               = 17,
    one_minus_src1_alpha     = 18,
    max_enum                 = 0x7FFFFFFF
};

export enum class blend_operator : uint32_t {
    add                    = 0,
    subtract               = 1,
    reverse_subtract       = 2,
    min                    = 3,
    max                    = 4,
    zero_ext               = 1000148000,
    src_ext                = 1000148001,
    dst_ext                = 1000148002,
    src_over_ext           = 1000148003,
    dst_over_ext           = 1000148004,
    src_in_ext             = 1000148005,
    dst_in_ext             = 1000148006,
    src_out_ext            = 1000148007,
    dst_out_ext            = 1000148008,
    src_atop_ext           = 1000148009,
    dst_atop_ext           = 1000148010,
    xor_ext                = 1000148011,
    multiply_ext           = 1000148012,
    screen_ext             = 1000148013,
    overlay_ext            = 1000148014,
    darken_ext             = 1000148015,
    lighten_ext            = 1000148016,
    colordodge_ext         = 1000148017,
    colorburn_ext          = 1000148018,
    hardlight_ext          = 1000148019,
    softlight_ext          = 1000148020,
    difference_ext         = 1000148021,
    exclusion_ext          = 1000148022,
    invert_ext             = 1000148023,
    invert_rgb_ext         = 1000148024,
    lineardodge_ext        = 1000148025,
    linearburn_ext         = 1000148026,
    vividlight_ext         = 1000148027,
    linearlight_ext        = 1000148028,
    pinlight_ext           = 1000148029,
    hardmix_ext            = 1000148030,
    hsl_hue_ext            = 1000148031,
    hsl_saturation_ext     = 1000148032,
    hsl_color_ext          = 1000148033,
    hsl_luminosity_ext     = 1000148034,
    plus_ext               = 1000148035,
    plus_clamped_ext       = 1000148036,
    plus_clamped_alpha_ext = 1000148037,
    plus_darker_ext        = 1000148038,
    minus_ext              = 1000148039,
    minus_clamped_ext      = 1000148040,
    contrast_ext           = 1000148041,
    invert_ovg_ext         = 1000148042,
    red_ext                = 1000148043,
    green_ext              = 1000148044,
    blue_ext               = 1000148045,
};

export struct IRenderGraphBuilder
{
    const DeerVulkan::SVkCommandBuffer* commandBuffer{nullptr};
    const SRenderPassCore* currentRenderPass{nullptr};
};
export struct SRenderGraph
{
    RenderGraphObjectPool renderGraphObjectPool;
    std::list<SRenderPassCore*> renderPasses;
};

export void FromInternalContext(IRenderGraphBuilder& renderGraphBuilder, const DeerVulkan::SVkCommandBuffer& commandBuffer, const SRenderPassCore& currentRenderPass)
{
    renderGraphBuilder.commandBuffer     = &commandBuffer;
    renderGraphBuilder.currentRenderPass = &currentRenderPass;
}

export inline void SetViewport(const SRenderContext& renderGraphBuilder, const float xPos, const float yPos, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f)
{
    DeerVulkan::SetViewport(renderGraphBuilder.commandBuffer, xPos, yPos, width, height, minDepth, maxDepth);
}
export inline void SetScissor(const SRenderContext& renderGraphBuilder, const uint32_t width, const uint32_t height, const uint32_t xPos, const uint32_t yPos)
{
    DeerVulkan::SetScissor(renderGraphBuilder.commandBuffer, width, height, xPos, yPos);
}
export inline void SetAlphaToCoverageEnable(const SRenderContext& renderGraphBuilder, const bool enable)
{
    DeerVulkan::SetAlphaToCoverageEnable(renderGraphBuilder.commandBuffer, enable);
}
export inline void SetColorBlendEnable(const SRenderContext& renderGraphBuilder, const bool enable)
{
    DeerVulkan::SetColorBlendEnable(renderGraphBuilder.commandBuffer, enable);
}
export inline void SetColorBlendEquation(const SRenderContext& renderGraphBuilder, const blend_factor srcColorBlendFactor, blend_factor dstColorBlendFactor, blend_operator colorBlendOp, blend_factor srcAlphaBlendFactor,
                                         blend_factor dstAlphaBlendFactor, blend_operator alphaBlendOp)
{
    DeerVulkan::SetColorBlendEquation(renderGraphBuilder.commandBuffer, static_cast<uint32_t>(srcColorBlendFactor), static_cast<uint32_t>(dstColorBlendFactor), static_cast<uint32_t>(colorBlendOp), static_cast<uint32_t>(srcAlphaBlendFactor),
                                      static_cast<uint32_t>(dstAlphaBlendFactor), static_cast<uint32_t>(alphaBlendOp));
}
export inline void SetColorWriteMask(const SRenderContext& renderGraphBuilder, const color_component mask)
{
    DeerVulkan::SetColorWriteMask(renderGraphBuilder.commandBuffer, static_cast<uint32_t>(mask));
}
export inline void SetCullMode(const SRenderContext& renderGraphBuilder, const cull_mode cullMode)
{
    DeerVulkan::SetCullMode(renderGraphBuilder.commandBuffer, static_cast<uint8_t>(cullMode));
}
export inline void SetDepthBiasEnable(const SRenderContext& renderGraphBuilder, const bool enable)
{
    DeerVulkan::SetDepthBiasEnable(renderGraphBuilder.commandBuffer, enable);
}
export inline void SetDepthCompareOperator(const SRenderContext& renderGraphBuilder, const compare_operator depthOperator)
{
    DeerVulkan::SetDepthCompareOperator(renderGraphBuilder.commandBuffer, static_cast<uint8_t>(depthOperator));
}
export inline void SetDepthTestEnable(const SRenderContext& renderGraphBuilder, const bool enable)
{
    DeerVulkan::SetDepthTestEnable(renderGraphBuilder.commandBuffer, enable);
}
export inline void SetDepthWriteEnable(const SRenderContext& renderGraphBuilder, const bool enable)
{
    DeerVulkan::SetDepthWriteEnable(renderGraphBuilder.commandBuffer, enable);
}
export inline void SetFrontFace(const SRenderContext& renderGraphBuilder, const bool isClockWice)
{
    DeerVulkan::SetFrontFace(renderGraphBuilder.commandBuffer, isClockWice);
}
export inline void SetLineWidth(const SRenderContext& renderGraphBuilder, const float isClockWice)
{
    DeerVulkan::SetLineWidth(renderGraphBuilder.commandBuffer, isClockWice);
}
export inline void SetPolygonMode(const SRenderContext& renderGraphBuilder, const polygon_mode mode)
{
    DeerVulkan::SetPolygonMode(renderGraphBuilder.commandBuffer, static_cast<uint32_t>(mode));
}
export inline void SetPrimitiveRestartEnable(const SRenderContext& renderGraphBuilder, const bool enable)
{
    SetPrimitiveRestartEnable(renderGraphBuilder.commandBuffer, enable);
}
export inline void SetPrimitiveTopology(const SRenderContext& renderGraphBuilder, const primitive_topology topology)
{
    DeerVulkan::SetPrimitiveTopology(renderGraphBuilder.commandBuffer, static_cast<uint8_t>(topology));
}
export inline void SetRasterizationSamples(const SRenderContext& renderGraphBuilder, const uint8_t samples, const uint32_t mask = 0xFFFFFFFF)
{
    DeerVulkan::SetRasterizationSamples(renderGraphBuilder.commandBuffer, samples, mask);
}
export inline void SetRasterizerDiscardEnable(const SRenderContext& renderGraphBuilder, const bool enable)
{
    DeerVulkan::SetRasterizerDiscardEnable(renderGraphBuilder.commandBuffer, enable);
}
export inline void SetStencilTestEnable(const SRenderContext& renderGraphBuilder, const bool enable)
{
    DeerVulkan::SetStencilTestEnable(renderGraphBuilder.commandBuffer, enable);
}
export inline void SetVertexInput(const SRenderContext& renderGraphBuilder)
{
    DeerVulkan::SetVertexInput(renderGraphBuilder.commandBuffer);
}

export inline void BindShader(const SRenderContext& renderGraphBuilder, const SShader& shader)
{
    DeerVulkan::BindShader(renderGraphBuilder.commandBuffer, shader.shader);
}
export inline void DrawFullscreen(const SRenderContext& renderGraphBuilder)
{
    DeerVulkan::DrawFullscreen(renderGraphBuilder.commandBuffer);
}

export template <class PassData>
void SetRenderFunc(IRenderGraphBuilder& renderGraphBuilder, std::function<void(const PassData*, const SRenderContext&)> renderFunc)
{
    SRenderPassBase<PassData>* renderPass = std::bit_cast<SRenderPassBase<PassData>*>(renderGraphBuilder.currentRenderPass);
    renderPass->renderFunction            = renderFunc;
}

export template <class PassData>
IRenderGraphBuilder AddComputeRenderPass(SRenderGraph&renderGraph, PassData*& passData)
{
    // todo : remove the need for the seconde template argument
    auto* renderPass = Get<SComputePass<PassData>, SRenderPassCore>(renderGraph.renderGraphObjectPool);
    auto* renderData = Get<PassData>(renderGraph.renderGraphObjectPool);

    Initialize(renderPass, true, static_cast<int>(renderGraph.renderPasses.size()), renderData);
    passData = renderPass->data;

    renderGraph.renderPasses.emplace_back(renderPass);
    return IRenderGraphBuilder{nullptr, renderPass};
}

export template <class PassData>
IRenderGraphBuilder AddRasterRenderPass(SRenderGraph& renderGraph, PassData*& passData)
{
    // todo : remove the need for the seconde template argument
    auto* renderPass = Get<SRasterPass<PassData>, SRenderPassCore>(renderGraph.renderGraphObjectPool);
    auto* renderData = Get<PassData>(renderGraph.renderGraphObjectPool);

    Initialize(renderPass, true, static_cast<int>(renderGraph.renderPasses.size()), renderData);
    passData = renderPass->data;

    renderGraph.renderPasses.emplace_back(renderPass);
    return IRenderGraphBuilder{nullptr, renderPass};
}

export void ExecuteAll(SRenderer& renderer, const SRenderGraph& renderGraph)
{
    const SRenderContext renderContext{.commandBuffer = renderer.commandBuffer};
    for (SRenderPassCore* renderPass : renderGraph.renderPasses)
    {
        renderPass->Execute(renderContext);
    }
}
export void CleanupRenderGraph(SRenderGraph& renderGraph)
{
    for (SRenderPassCore* renderPass : renderGraph.renderPasses)
    {
        Release(renderGraph.renderGraphObjectPool, renderPass);
    }
    renderGraph.renderPasses.clear();
}
} // namespace FawnVision
