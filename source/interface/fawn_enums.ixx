//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "api/vulkan/deer_vulkan_core.hpp"

export module FawnVision:Enum;
import FawnAlgebra;
import std;

namespace fawn_vision
{
export enum class image_layout : std::uint32_t {
    undefined                                    = 0,
    general                                      = 1,
    color_attachment_optimal                     = 2,
    depth_stencil_attachment_optimal             = 3,
    depth_stencil_read_only_optimal              = 4,
    shader_read_only_optimal                     = 5,
    transfer_src_optimal                         = 6,
    transfer_dst_optimal                         = 7,
    preinitialized                               = 8,
    depth_read_only_stencil_attachment_optimal   = 1000117000,
    depth_attachment_stencil_read_only_optimal   = 1000117001,
    depth_attachment_optimal                     = 1000241000,
    depth_read_only_optimal                      = 1000241001,
    stencil_attachment_optimal                   = 1000241002,
    stencil_read_only_optimal                    = 1000241003,
    read_only_optimal                            = 1000314000,
    attachment_optimal                           = 1000314001,
    present_src_khr                              = 1000001002,
    video_decode_dst_khr                         = 1000024000,
    video_decode_src_khr                         = 1000024001,
    video_decode_dpb_khr                         = 1000024002,
    shared_present_khr                           = 1000111000,
    fragment_density_map_optimal_ext             = 1000218000,
    fragment_shading_rate_attachment_optimal_khr = 1000164003,
    rendering_local_read_khr                     = 1000232000,
    video_encode_dst_khr                         = 1000299000,
    video_encode_src_khr                         = 1000299001,
    video_encode_dpb_khr                         = 1000299002,
    attachment_feedback_loop_optimal_ext         = 1000339000,
};

export enum class gfx_status : std::int8_t {
    ok         = 0,
    regenerate = 1,  // swapchain out of date / suboptimal — recreate and retry
    not_ok     = -1, // unrecoverable error, caller should shut down
};

[[nodiscard]] constexpr auto ToGfxStatus(const deer_vulkan::vk_status status) noexcept -> gfx_status
{
    using vs = deer_vulkan::vk_status;
    switch (status)
    {
    case vs::ok: [[fallthrough]];
    case vs::not_ready: [[fallthrough]]; // caller is polling — still fine
    case vs::already_initialized: return gfx_status::ok;

    case vs::out_of_date: [[fallthrough]];
    case vs::suboptimal: return gfx_status::regenerate;

    default: return gfx_status::not_ok;
    }
}

} // namespace fawn_vision
