//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "api/vulkan/wrapper/command.hpp"
#include "api/vulkan/wrapper/device.hpp"
#include "api/vulkan/wrapper/queue.hpp"
#include "api/vulkan/wrapper/semaphore.hpp"
#include "api/vulkan/wrapper/swap_chain.hpp"

export module FawnVision:RenderPassContext;
import :Buffer;
import :Enum;
import :Shader;
import :Descriptor;
import :Mesh;
import FawnAlgebra;

import std;

namespace fawn_vision
{
export enum class compare_operator : std::uint8_t {
    never            = 0,
    less             = 1,
    equal            = 2,
    less_or_equal    = 3,
    greater          = 4,
    not_equal        = 5,
    greater_or_equal = 6,
    always           = 7,
};

export enum class cull_mode : std::uint8_t {
    none           = 0,
    front          = 1,
    back           = 2,
    front_and_back = 3,
};

export enum class primitive_topology : std::uint8_t {
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

export enum class polygon_mode : std::uint8_t {
    fill  = 0,
    line  = 1,
    point = 2,
};

export enum class color_component : std::uint8_t {
    r = 0x01,
    g = 0x02,
    b = 0x04,
    a = 0x08,
};

export enum class blend_factor : std::uint32_t {
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
};

export enum class blend_operator : std::uint32_t {
    add                = 0,
    subtract           = 1,
    reverse_subtract   = 2,
    min                = 3,
    max                = 4,
    zero               = 1000148000,
    src                = 1000148001,
    dst                = 1000148002,
    src_over           = 1000148003,
    dst_over           = 1000148004,
    src_in             = 1000148005,
    dst_in             = 1000148006,
    src_out            = 1000148007,
    dst_out            = 1000148008,
    src_atop           = 1000148009,
    dst_atop           = 1000148010,
    xor_op             = 1000148011, // 'xor' is a keyword in C++
    multiply           = 1000148012,
    screen             = 1000148013,
    overlay            = 1000148014,
    darken             = 1000148015,
    lighten            = 1000148016,
    colordodge         = 1000148017,
    colorburn          = 1000148018,
    hardlight          = 1000148019,
    softlight          = 1000148020,
    difference         = 1000148021,
    exclusion          = 1000148022,
    invert             = 1000148023,
    invert_rgb         = 1000148024,
    lineardodge        = 1000148025,
    linearburn         = 1000148026,
    vividlight         = 1000148027,
    linearlight        = 1000148028,
    pinlight           = 1000148029,
    hardmix            = 1000148030,
    hsl_hue            = 1000148031,
    hsl_saturation     = 1000148032,
    hsl_color          = 1000148033,
    hsl_luminosity     = 1000148034,
    plus               = 1000148035,
    plus_clamped       = 1000148036,
    plus_clamped_alpha = 1000148037,
    plus_darker        = 1000148038,
    minus              = 1000148039,
    minus_clamped      = 1000148040,
    contrast           = 1000148041,
    invert_ovg         = 1000148042,
    red                = 1000148043,
    green              = 1000148044,
    blue               = 1000148045,
};

export color_component operator|(const color_component& lhs, const color_component& rhs)
{
    using value_t = std::underlying_type_t<color_component>;
    return static_cast<color_component>(static_cast<value_t>(lhs) | static_cast<value_t>(rhs));
}

export struct VertexAttributes
{
    std::uint32_t location{};
    std::uint32_t binding{};
    format pixelFormat{};
    std::uint32_t offset{};
    bool isInstance{};
};

export struct RenderPassContext
{
    const deer_vulkan::Dispatch& dispatch;
    const deer_vulkan::Device& device;
    const deer_vulkan::CommandBuffer& commandBuffer;
    deer_vulkan::SwapChain& swapChain;
    const deer_vulkan::Semaphore& timelineSemaphore;
    const deer_vulkan::Semaphore& binarySemaphore;
    const deer_vulkan::Queue* queue;
};

// ---------------------------------------------------------------------------
// Dynamic state
// ---------------------------------------------------------------------------

export inline auto SetViewport(const RenderPassContext& ctx, const float xPos, const float yPos, const float width, const float height, const float minDepth = 0.0F,
                               const float maxDepth = 1.0F) noexcept -> void
{
    deer_vulkan::SetViewport(ctx.dispatch, ctx.commandBuffer, xPos, yPos, width, height, minDepth, maxDepth);
}

export inline auto SetScissor(const RenderPassContext& ctx, const std::uint32_t width, const std::uint32_t height, const std::int32_t xPos, const std::int32_t yPos) noexcept -> void
{
    deer_vulkan::SetScissor(ctx.dispatch, ctx.commandBuffer, width, height, xPos, yPos);
}

export inline auto SetAlphaToCoverageEnable(const RenderPassContext& ctx, const bool enable) noexcept -> void
{
    deer_vulkan::SetAlphaToCoverageEnable(ctx.dispatch, ctx.commandBuffer, enable);
}

export inline auto SetColorBlendEnable(const RenderPassContext& ctx, const bool enable) noexcept -> void
{
    deer_vulkan::SetColorBlendEnable(ctx.dispatch, ctx.commandBuffer, enable);
}

export inline auto SetColorBlendEquation(const RenderPassContext& ctx, const blend_factor srcColorBlendFactor, const blend_factor dstColorBlendFactor,
                                         const blend_operator colorBlendOp, const blend_factor srcAlphaBlendFactor, const blend_factor dstAlphaBlendFactor,
                                         const blend_operator alphaBlendOp) noexcept -> void
{
    deer_vulkan::SetColorBlendEquation(ctx.dispatch, ctx.commandBuffer, static_cast<std::uint32_t>(srcColorBlendFactor), static_cast<std::uint32_t>(dstColorBlendFactor),
                                       static_cast<std::uint32_t>(colorBlendOp), static_cast<std::uint32_t>(srcAlphaBlendFactor), static_cast<std::uint32_t>(dstAlphaBlendFactor),
                                       static_cast<std::uint32_t>(alphaBlendOp));
}

export inline auto SetColorWriteMask(const RenderPassContext& ctx, const color_component mask) noexcept -> void
{
    deer_vulkan::SetColorWriteMask(ctx.dispatch, ctx.commandBuffer, static_cast<std::uint32_t>(mask));
}

export inline auto SetCullMode(const RenderPassContext& ctx, const cull_mode cullMode) noexcept -> void
{
    deer_vulkan::SetCullMode(ctx.dispatch, ctx.commandBuffer, static_cast<std::uint8_t>(cullMode));
}

export inline auto SetDepthBiasEnable(const RenderPassContext& ctx, const bool enable) noexcept -> void
{
    deer_vulkan::SetDepthBiasEnable(ctx.dispatch, ctx.commandBuffer, enable);
}

export inline auto SetDepthCompareOperator(const RenderPassContext& ctx, const compare_operator op) noexcept -> void
{
    deer_vulkan::SetDepthCompareOperator(ctx.dispatch, ctx.commandBuffer, static_cast<std::uint8_t>(op));
}

export inline auto SetDepthTestEnable(const RenderPassContext& ctx, const bool enable) noexcept -> void
{
    deer_vulkan::SetDepthTestEnable(ctx.dispatch, ctx.commandBuffer, enable);
}

export inline auto SetDepthWriteEnable(const RenderPassContext& ctx, const bool enable) noexcept -> void
{
    deer_vulkan::SetDepthWriteEnable(ctx.dispatch, ctx.commandBuffer, enable);
}

export inline auto SetFrontFace(const RenderPassContext& ctx, const bool isClockwise) noexcept -> void
{
    deer_vulkan::SetFrontFace(ctx.dispatch, ctx.commandBuffer, isClockwise);
}

export inline auto SetLineWidth(const RenderPassContext& ctx, const float lineWidth) noexcept -> void
{
    deer_vulkan::SetLineWidth(ctx.dispatch, ctx.commandBuffer, lineWidth);
}

export inline auto SetPolygonMode(const RenderPassContext& ctx, const polygon_mode mode) noexcept -> void
{
    deer_vulkan::SetPolygonMode(ctx.dispatch, ctx.commandBuffer, static_cast<std::uint32_t>(mode));
}

export inline auto SetPrimitiveRestartEnable(const RenderPassContext& ctx, const bool enable) noexcept -> void
{
    deer_vulkan::SetPrimitiveRestartEnable(ctx.dispatch, ctx.commandBuffer, enable); // was calling itself
}

export inline auto SetPrimitiveTopology(const RenderPassContext& ctx, const primitive_topology topology) noexcept -> void
{
    deer_vulkan::SetPrimitiveTopology(ctx.dispatch, ctx.commandBuffer, static_cast<std::uint8_t>(topology));
}

export inline auto SetRasterizationSamples(const RenderPassContext& ctx, const std::uint8_t samples, const std::uint32_t mask = 0xFFFFFFFFU) noexcept -> void
{
    deer_vulkan::SetRasterizationSamples(ctx.dispatch, ctx.commandBuffer, samples, mask);
}

export inline auto SetRasterizerDiscardEnable(const RenderPassContext& ctx, const bool enable) noexcept -> void
{
    deer_vulkan::SetRasterizerDiscardEnable(ctx.dispatch, ctx.commandBuffer, enable);
}

export inline auto SetStencilTestEnable(const RenderPassContext& ctx, const bool enable) noexcept -> void
{
    deer_vulkan::SetStencilTestEnable(ctx.dispatch, ctx.commandBuffer, enable);
}

// ---------------------------------------------------------------------------
// Resource binding
// ---------------------------------------------------------------------------

export template <std::size_t N = std::dynamic_extent>
auto SetVertexInput(const RenderPassContext& ctx, const std::span<const VertexAttributes, N> attributes, const std::uint32_t vertexSize,
                           const std::uint32_t instanceSize) noexcept -> void
{
    std::vector<deer_vulkan::VertexAttributes> vkAttribs(attributes.size());
    for (std::size_t i = 0; i < attributes.size(); ++i)
    {
        vkAttribs[i] = {
            .location   = attributes[i].location,
            .binding    = attributes[i].binding,
            .format     = static_cast<std::uint32_t>(attributes[i].pixelFormat),
            .offset     = attributes[i].offset,
            .isInstance = attributes[i].isInstance,
        };
    }
    deer_vulkan::SetVertexInput(ctx.dispatch, ctx.commandBuffer, vertexSize, instanceSize, vkAttribs);
}

export inline auto SetVertexInput(const RenderPassContext& ctx) noexcept -> void
{
    deer_vulkan::SetVertexInput(ctx.dispatch, ctx.commandBuffer);
}

export inline auto BindShader(const RenderPassContext& ctx, const Shader& shader) noexcept -> void
{
    deer_vulkan::BindShader(ctx.dispatch, ctx.commandBuffer, shader.shader);
}

export inline auto BindBuffer(const RenderPassContext& ctx, const Buffer& buffer) noexcept -> void
{
    deer_vulkan::BindInstanceBuffer(ctx.dispatch, ctx.commandBuffer, buffer.buffer);
}

export inline auto BindDescriptor(const RenderPassContext& ctx, const Descriptor& descriptor) noexcept -> void
{
    deer_vulkan::BindDescriptor(ctx.dispatch, ctx.commandBuffer, descriptor.descriptor);
}

export inline auto BindMesh(const RenderPassContext& ctx, const Mesh& mesh) noexcept -> void
{
    deer_vulkan::BindIndexBuffer(ctx.dispatch, ctx.commandBuffer, mesh.indexBuffer.buffer);
    deer_vulkan::BindVertexBuffer(ctx.dispatch, ctx.commandBuffer, mesh.vertexBuffer.buffer);
}

// ---------------------------------------------------------------------------
// Data upload
// ---------------------------------------------------------------------------

export template <typename T, auto Size = std::dynamic_extent>
auto CopyData(const RenderPassContext& ctx, const Buffer& buffer, const std::span<const T, Size> data) noexcept -> void
{
    deer_vulkan::CopyData(ctx.dispatch, ctx.device, buffer.buffer, data);
}

// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------

export inline auto DrawFullscreen(const RenderPassContext& ctx) noexcept -> void
{
    deer_vulkan::DrawFullscreen(ctx.dispatch, ctx.commandBuffer);
}

export inline auto Draw(const RenderPassContext& ctx, const std::uint32_t indexCount, const std::uint32_t instanceCount, const std::uint32_t indexOffset,
                        const std::uint32_t instanceOffset) noexcept -> void
{
    deer_vulkan::DrawIndexed(ctx.dispatch, ctx.commandBuffer, indexCount, instanceCount, indexOffset, instanceOffset);
}

} // namespace fawn_vision
