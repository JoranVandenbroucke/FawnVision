// buffer.
//  Copyright (c) 2024.
//  Author: Joran.
//

module;
#include "api/vulkan/wrapper/buffer.hpp"

export module FawnVision:Buffer;
import :Enum;
import :Renderer;

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
export enum class buffer_usage : std::uint32_t {
    transfer_src                                 = 0x00000001,
    transfer_dst                                 = 0x00000002,
    uniform_texel_buffer                         = 0x00000004,
    storage_texel_buffer                         = 0x00000008,
    uniform_buffer                               = 0x00000010,
    storage_buffer                               = 0x00000020,
    index_buffer                                 = 0x00000040,
    vertex_buffer                                = 0x00000080,
    indirect_buffer                              = 0x00000100,
    shader_device_address                        = 0x00020000,
    video_decode_src                             = 0x00002000,
    video_decode_dst                             = 0x00004000,
    transform_feedback_buffer                    = 0x00000800,
    transform_feedback_counter_buffer            = 0x00001000,
    conditional_rendering                        = 0x00000200,
    acceleration_structure_build_input_read_only = 0x00080000,
    acceleration_structure_storage               = 0x00100000,
    shader_binding_table                         = 0x00000400,
    ray_tracing                                  = shader_binding_table,
    video_encode_dst                             = 0x00008000,
    video_encode_src                             = 0x00010000,
    sampler_descriptor_buffer                    = 0x00200000,
    resource_descriptor_buffer                   = 0x00400000,
    push_descriptors_descriptor_buffer           = 0x04000000,
    micromap_build_input_read_only               = 0x00800000,
    micromap_storage                             = 0x01000000,
};

export enum class memory_property : std::uint32_t {
    device_local     = 0x00000001,
    host_visible     = 0x00000002,
    host_coherent    = 0x00000004,
    host_cached      = 0x00000008,
    lazily_allocated = 0x00000010,
    protected_memory = 0x00000020,
    device_coherent  = 0x00000040,
    device_uncached  = 0x00000080,
    rdma_capable     = 0x00000100,
};

export constexpr buffer_usage operator|(const buffer_usage& lhs, const buffer_usage& rhs)
{
    using value_t = std::underlying_type_t<buffer_usage>;
    return static_cast<buffer_usage>(static_cast<value_t>(lhs) | static_cast<value_t>(rhs));
}

export constexpr memory_property operator|(const memory_property& lhs, const memory_property& rhs)
{
    using value_t = std::underlying_type_t<memory_property>;
    return static_cast<memory_property>(static_cast<value_t>(lhs) | static_cast<value_t>(rhs));
}

export struct Buffer
{
    deer_vulkan::Buffer buffer{};
};

export [[nodiscard]] inline auto Initialize(const Renderer& renderer, const uint64_t size, const buffer_usage bufferUsage, const memory_property memoryProperties,
                                            Buffer& buffer) noexcept -> gfx_status
{
    const deer_vulkan::BufferCreateInfo createInfo{
        .size{size},
        .usage{static_cast<std::uint32_t>(bufferUsage)},
        .memoryProperty{static_cast<std::uint32_t>(memoryProperties)},
    };

    GFX_CHECK(deer_vulkan::Initialize(renderer.dispatch, renderer.device, renderer.physical, createInfo, buffer.buffer), "Something went wronge while initializing the buffer");

    return gfx_status::ok;
}

export inline auto Cleanup(const Renderer& renderer, Buffer& buffer) noexcept -> void
{
    deer_vulkan::Cleanup(renderer.dispatch, renderer.device, buffer.buffer);
}

export template <typename T, auto Size = std::dynamic_extent>
auto CopyData(const Renderer& renderer, const Buffer& buffer, const std::span<const T, Size> data) noexcept -> void
{
    deer_vulkan::CopyData(renderer.dispatch, renderer.device, buffer.buffer, data);
}

} // namespace fawn_vision
