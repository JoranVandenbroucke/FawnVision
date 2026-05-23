//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "api/vulkan/wrapper/buffer.hpp"
#include "api/vulkan/wrapper/descriptor.hpp"

export module FawnVision:Descriptor;
import :Enum;
import :Renderer;
import :Texture;

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
export enum class shader_stage : std::uint32_t {
    vertex                  = 0x00000001,
    tessellation_control    = 0x00000002,
    tessellation_evaluation = 0x00000004,
    geometry                = 0x00000008,
    fragment                = 0x00000010,
    compute                 = 0x00000020,
    all_graphics            = 0x0000001F,
    raygen                  = 0x00000100,
    any_ray_hit             = 0x00000200,
    closest_ray_hit         = 0x00000400,
    ray_miss                = 0x00000800,
    ray_intersection        = 0x00001000,
    callable                = 0x00002000,
    task                    = 0x00000040,
    mesh                    = 0x00000080,
    subpass_shading         = 0x00004000,
    cluster_culling         = 0x00080000,
    all                     = 0x7FFFFFFF,
};

export enum class descriptor_type : std::uint32_t {
    sampler                = 0,
    combined_image_sampler = 1,
    sampled_image          = 2,
    storage_image          = 3,
    uniform_texel_buffer   = 4,
    storage_texel_buffer   = 5,
    uniform_buffer         = 6,
    storage_buffer         = 7,
    uniform_buffer_dynamic = 8,
    storage_buffer_dynamic = 9,
    input_attachment       = 10,
};

export struct DescriptorData
{
    std::uint32_t binding{};
    descriptor_type type{};
    std::uint32_t count{};
    shader_stage stageFlags{};
};

export struct Descriptor
{
    deer_vulkan::Descriptor descriptor{};
};

export template <auto Size = std::dynamic_extent>
[[nodiscard]] inline auto Initialize(const Renderer& renderer, const std::span<const DescriptorData, Size>& createInfo, Descriptor& descriptor) noexcept -> gfx_status
{
    std::vector<deer_vulkan::DescriptorLayout> layouts(createInfo.size());
    for (std::size_t i{}; i < createInfo.size(); ++i)
    {
        layouts[i] = {
            .binding         = createInfo[i].binding,
            .descriptorType  = static_cast<std::uint32_t>(createInfo[i].type),
            .descriptorCount = createInfo[i].count,
            .stageFlags      = static_cast<std::uint32_t>(createInfo[i].stageFlags),
        };
    }

    GFX_CHECK(deer_vulkan::Initialize(renderer.dispatch, renderer.device, layouts, descriptor.descriptor), "Something went wrong while setting up the discriptor set");
    return gfx_status::ok;
}

export inline void Cleanup(const Renderer& renderer, Descriptor& descriptor) noexcept
{
    Cleanup(renderer.dispatch, renderer.device, descriptor.descriptor);
}

export inline void UpdateDescriptorSet(const Renderer& renderer, const Descriptor& descriptor) noexcept
{
    deer_vulkan::UpdateDescriptor(renderer.dispatch, renderer.device, descriptor.descriptor);
}

export inline void BindTextureToDescriptor(Descriptor& descriptor, const Texture& texture, const std::uint32_t binding) noexcept
{
    deer_vulkan::BindImage(descriptor.descriptor, texture.sampler, texture.view, texture.image, binding);
}

} // namespace fawn_vision
