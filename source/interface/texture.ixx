//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "api/vulkan/wrapper/command.hpp"
#include "api/vulkan/wrapper/image.hpp"
#include "api/vulkan/wrapper/image_view.hpp"
#include "api/vulkan/wrapper/sampler.hpp"

export module FawnVision:Texture;
import :Buffer;
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

#define GFX_CHECK_CLEANUP(expr, ...)                                                                                                                                               \
    if (deer_vulkan::vk_status _s = expr; deer_vulkan::IsError(_s)) [[unlikely]]                                                                                                   \
    {                                                                                                                                                                              \
        auto _e = deer_vulkan::GetError(_s);                                                                                                                                       \
        std::println(std::cerr, "[GFX] {}:{} — {} | Hint: {} | Context: {}", __FILE__, __LINE__, _e.message, _e.hint, std::format(__VA_ARGS__));                                   \
        Cleanup(renderer, stagingBuffer);                                                                                                                                          \
        return ToGfxStatus(_s);                                                                                                                                                    \
    }

namespace fawn_vision
{
export enum class component_swizzle : std::uint8_t {
    identity = 0,
    zero     = 1,
    one      = 2,
    r        = 3,
    g        = 4,
    b        = 5,
    a        = 6,
};

export enum class image_tiling : std::uint8_t {
    optimal = 0,
    linear  = 1,
};

// Matches VkImageType (0-2) — for VkImageViewType use image_view_type
export enum class image_type : std::uint8_t {
    type_1d = 0,
    type_2d = 1,
    type_3d = 2,
};

// Matches VkImageViewType (0-6)
export enum class image_view_type : std::uint8_t {
    type_1d         = 0,
    type_2d         = 1,
    type_3d         = 2,
    type_cube       = 3,
    type_1d_array   = 4,
    type_2d_array   = 5,
    type_cube_array = 6,
};

// format enum unchanged — values match VkFormat exactly
export enum class format : std::uint32_t {
    undefined                                  = 0,
    r4g4_unorm_pack8                           = 1,
    r4g4b4a4_unorm_pack16                      = 2,
    b4g4r4a4_unorm_pack16                      = 3,
    r5g6b5_unorm_pack16                        = 4,
    b5g6r5_unorm_pack16                        = 5,
    r5g5b5a1_unorm_pack16                      = 6,
    b5g5r5a1_unorm_pack16                      = 7,
    a1r5g5b5_unorm_pack16                      = 8,
    r8_unorm                                   = 9,
    r8_snorm                                   = 10,
    r8_uscaled                                 = 11,
    r8_sscaled                                 = 12,
    r8_uint                                    = 13,
    r8_sint                                    = 14,
    r8_srgb                                    = 15,
    r8g8_unorm                                 = 16,
    r8g8_snorm                                 = 17,
    r8g8_uscaled                               = 18,
    r8g8_sscaled                               = 19,
    r8g8_uint                                  = 20,
    r8g8_sint                                  = 21,
    r8g8_srgb                                  = 22,
    r8g8b8_unorm                               = 23,
    r8g8b8_snorm                               = 24,
    r8g8b8_uscaled                             = 25,
    r8g8b8_sscaled                             = 26,
    r8g8b8_uint                                = 27,
    r8g8b8_sint                                = 28,
    r8g8b8_srgb                                = 29,
    b8g8r8_unorm                               = 30,
    b8g8r8_snorm                               = 31,
    b8g8r8_uscaled                             = 32,
    b8g8r8_sscaled                             = 33,
    b8g8r8_uint                                = 34,
    b8g8r8_sint                                = 35,
    b8g8r8_srgb                                = 36,
    r8g8b8a8_unorm                             = 37,
    r8g8b8a8_snorm                             = 38,
    r8g8b8a8_uscaled                           = 39,
    r8g8b8a8_sscaled                           = 40,
    r8g8b8a8_uint                              = 41,
    r8g8b8a8_sint                              = 42,
    r8g8b8a8_srgb                              = 43,
    b8g8r8a8_unorm                             = 44,
    b8g8r8a8_snorm                             = 45,
    b8g8r8a8_uscaled                           = 46,
    b8g8r8a8_sscaled                           = 47,
    b8g8r8a8_uint                              = 48,
    b8g8r8a8_sint                              = 49,
    b8g8r8a8_srgb                              = 50,
    a8b8g8r8_unorm_pack32                      = 51,
    a8b8g8r8_snorm_pack32                      = 52,
    a8b8g8r8_uscaled_pack32                    = 53,
    a8b8g8r8_sscaled_pack32                    = 54,
    a8b8g8r8_uint_pack32                       = 55,
    a8b8g8r8_sint_pack32                       = 56,
    a8b8g8r8_srgb_pack32                       = 57,
    a2r10g10b10_unorm_pack32                   = 58,
    a2r10g10b10_snorm_pack32                   = 59,
    a2r10g10b10_uscaled_pack32                 = 60,
    a2r10g10b10_sscaled_pack32                 = 61,
    a2r10g10b10_uint_pack32                    = 62,
    a2r10g10b10_sint_pack32                    = 63,
    a2b10g10r10_unorm_pack32                   = 64,
    a2b10g10r10_snorm_pack32                   = 65,
    a2b10g10r10_uscaled_pack32                 = 66,
    a2b10g10r10_sscaled_pack32                 = 67,
    a2b10g10r10_uint_pack32                    = 68,
    a2b10g10r10_sint_pack32                    = 69,
    r16_unorm                                  = 70,
    r16_snorm                                  = 71,
    r16_uscaled                                = 72,
    r16_sscaled                                = 73,
    r16_uint                                   = 74,
    r16_sint                                   = 75,
    r16_sfloat                                 = 76,
    r16g16_unorm                               = 77,
    r16g16_snorm                               = 78,
    r16g16_uscaled                             = 79,
    r16g16_sscaled                             = 80,
    r16g16_uint                                = 81,
    r16g16_sint                                = 82,
    r16g16_sfloat                              = 83,
    r16g16b16_unorm                            = 84,
    r16g16b16_snorm                            = 85,
    r16g16b16_uscaled                          = 86,
    r16g16b16_sscaled                          = 87,
    r16g16b16_uint                             = 88,
    r16g16b16_sint                             = 89,
    r16g16b16_sfloat                           = 90,
    r16g16b16a16_unorm                         = 91,
    r16g16b16a16_snorm                         = 92,
    r16g16b16a16_uscaled                       = 93,
    r16g16b16a16_sscaled                       = 94,
    r16g16b16a16_uint                          = 95,
    r16g16b16a16_sint                          = 96,
    r16g16b16a16_sfloat                        = 97,
    r32_uint                                   = 98,
    r32_sint                                   = 99,
    r32_sfloat                                 = 100,
    r32g32_uint                                = 101,
    r32g32_sint                                = 102,
    r32g32_sfloat                              = 103,
    r32g32b32_uint                             = 104,
    r32g32b32_sint                             = 105,
    r32g32b32_sfloat                           = 106,
    r32g32b32a32_uint                          = 107,
    r32g32b32a32_sint                          = 108,
    r32g32b32a32_sfloat                        = 109,
    r64_uint                                   = 110,
    r64_sint                                   = 111,
    r64_sfloat                                 = 112,
    r64g64_uint                                = 113,
    r64g64_sint                                = 114,
    r64g64_sfloat                              = 115,
    r64g64b64_uint                             = 116,
    r64g64b64_sint                             = 117,
    r64g64b64_sfloat                           = 118,
    r64g64b64a64_uint                          = 119,
    r64g64b64a64_sint                          = 120,
    r64g64b64a64_sfloat                        = 121,
    b10g11r11_ufloat_pack32                    = 122,
    e5b9g9r9_ufloat_pack32                     = 123,
    d16_unorm                                  = 124,
    x8_d24_unorm_pack32                        = 125,
    d32_sfloat                                 = 126,
    s8_uint                                    = 127,
    d16_unorm_s8_uint                          = 128,
    d24_unorm_s8_uint                          = 129,
    d32_sfloat_s8_uint                         = 130,
    bc1_rgb_unorm_block                        = 131,
    bc1_rgb_srgb_block                         = 132,
    bc1_rgba_unorm_block                       = 133,
    bc1_rgba_srgb_block                        = 134,
    bc2_unorm_block                            = 135,
    bc2_srgb_block                             = 136,
    bc3_unorm_block                            = 137,
    bc3_srgb_block                             = 138,
    bc4_unorm_block                            = 139,
    bc4_snorm_block                            = 140,
    bc5_unorm_block                            = 141,
    bc5_snorm_block                            = 142,
    bc6h_ufloat_block                          = 143,
    bc6h_sfloat_block                          = 144,
    bc7_unorm_block                            = 145,
    bc7_srgb_block                             = 146,
    etc2_r8g8b8_unorm_block                    = 147,
    etc2_r8g8b8_srgb_block                     = 148,
    etc2_r8g8b8a1_unorm_block                  = 149,
    etc2_r8g8b8a1_srgb_block                   = 150,
    etc2_r8g8b8a8_unorm_block                  = 151,
    etc2_r8g8b8a8_srgb_block                   = 152,
    eac_r11_unorm_block                        = 153,
    eac_r11_snorm_block                        = 154,
    eac_r11g11_unorm_block                     = 155,
    eac_r11g11_snorm_block                     = 156,
    astc_4x4_unorm_block                       = 157,
    astc_4x4_srgb_block                        = 158,
    astc_5x4_unorm_block                       = 159,
    astc_5x4_srgb_block                        = 160,
    astc_5x5_unorm_block                       = 161,
    astc_5x5_srgb_block                        = 162,
    astc_6x5_unorm_block                       = 163,
    astc_6x5_srgb_block                        = 164,
    astc_6x6_unorm_block                       = 165,
    astc_6x6_srgb_block                        = 166,
    astc_8x5_unorm_block                       = 167,
    astc_8x5_srgb_block                        = 168,
    astc_8x6_unorm_block                       = 169,
    astc_8x6_srgb_block                        = 170,
    astc_8x8_unorm_block                       = 171,
    astc_8x8_srgb_block                        = 172,
    astc_10x5_unorm_block                      = 173,
    astc_10x5_srgb_block                       = 174,
    astc_10x6_unorm_block                      = 175,
    astc_10x6_srgb_block                       = 176,
    astc_10x8_unorm_block                      = 177,
    astc_10x8_srgb_block                       = 178,
    astc_10x10_unorm_block                     = 179,
    astc_10x10_srgb_block                      = 180,
    astc_12x10_unorm_block                     = 181,
    astc_12x10_srgb_block                      = 182,
    astc_12x12_unorm_block                     = 183,
    astc_12x12_srgb_block                      = 184,
    g8b8g8r8_422_unorm                         = 1000156000,
    b8g8r8g8_422_unorm                         = 1000156001,
    g8_b8_r8_3plane_420_unorm                  = 1000156002,
    g8_b8r8_2plane_420_unorm                   = 1000156003,
    g8_b8_r8_3plane_422_unorm                  = 1000156004,
    g8_b8r8_2plane_422_unorm                   = 1000156005,
    g8_b8_r8_3plane_444_unorm                  = 1000156006,
    r10x6_unorm_pack16                         = 1000156007,
    r10x6g10x6_unorm_2pack16                   = 1000156008,
    r10x6g10x6b10x6a10x6_unorm_4pack16         = 1000156009,
    g10x6b10x6g10x6r10x6_422_unorm_4pack16     = 1000156010,
    b10x6g10x6r10x6g10x6_422_unorm_4pack16     = 1000156011,
    g10x6_b10x6_r10x6_3plane_420_unorm_3pack16 = 1000156012,
    g10x6_b10x6r10x6_2plane_420_unorm_3pack16  = 1000156013,
    g10x6_b10x6_r10x6_3plane_422_unorm_3pack16 = 1000156014,
    g10x6_b10x6r10x6_2plane_422_unorm_3pack16  = 1000156015,
    g10x6_b10x6_r10x6_3plane_444_unorm_3pack16 = 1000156016,
    r12x4_unorm_pack16                         = 1000156017,
    r12x4g12x4_unorm_2pack16                   = 1000156018,
    r12x4g12x4b12x4a12x4_unorm_4pack16         = 1000156019,
    g12x4b12x4g12x4r12x4_422_unorm_4pack16     = 1000156020,
    b12x4g12x4r12x4g12x4_422_unorm_4pack16     = 1000156021,
    g12x4_b12x4_r12x4_3plane_420_unorm_3pack16 = 1000156022,
    g12x4_b12x4r12x4_2plane_420_unorm_3pack16  = 1000156023,
    g12x4_b12x4_r12x4_3plane_422_unorm_3pack16 = 1000156024,
    g12x4_b12x4r12x4_2plane_422_unorm_3pack16  = 1000156025,
    g12x4_b12x4_r12x4_3plane_444_unorm_3pack16 = 1000156026,
    g16b16g16r16_422_unorm                     = 1000156027,
    b16g16r16g16_422_unorm                     = 1000156028,
    g16_b16_r16_3plane_420_unorm               = 1000156029,
    g16_b16r16_2plane_420_unorm                = 1000156030,
    g16_b16_r16_3plane_422_unorm               = 1000156031,
    g16_b16r16_2plane_422_unorm                = 1000156032,
    g16_b16_r16_3plane_444_unorm               = 1000156033,
    g8_b8r8_2plane_444_unorm                   = 1000330000,
    g10x6_b10x6r10x6_2plane_444_unorm_3pack16  = 1000330001,
    g12x4_b12x4r12x4_2plane_444_unorm_3pack16  = 1000330002,
    g16_b16r16_2plane_444_unorm                = 1000330003,
    a4r4g4b4_unorm_pack16                      = 1000340000,
    a4b4g4r4_unorm_pack16                      = 1000340001,
    astc_4x4_sfloat_block                      = 1000066000,
    astc_5x4_sfloat_block                      = 1000066001,
    astc_5x5_sfloat_block                      = 1000066002,
    astc_6x5_sfloat_block                      = 1000066003,
    astc_6x6_sfloat_block                      = 1000066004,
    astc_8x5_sfloat_block                      = 1000066005,
    astc_8x6_sfloat_block                      = 1000066006,
    astc_8x8_sfloat_block                      = 1000066007,
    astc_10x5_sfloat_block                     = 1000066008,
    astc_10x6_sfloat_block                     = 1000066009,
    astc_10x8_sfloat_block                     = 1000066010,
    astc_10x10_sfloat_block                    = 1000066011,
    astc_12x10_sfloat_block                    = 1000066012,
    astc_12x12_sfloat_block                    = 1000066013,
    pvrtc1_2bpp_unorm_block_img                = 1000054000,
    pvrtc1_4bpp_unorm_block_img                = 1000054001,
    pvrtc2_2bpp_unorm_block_img                = 1000054002,
    pvrtc2_4bpp_unorm_block_img                = 1000054003,
    pvrtc1_2bpp_srgb_block_img                 = 1000054004,
    pvrtc1_4bpp_srgb_block_img                 = 1000054005,
    pvrtc2_2bpp_srgb_block_img                 = 1000054006,
    pvrtc2_4bpp_srgb_block_img                 = 1000054007,
    r16g16_sfixed5_nv                          = 1000464000,
    a1b5g5r5_unorm_pack16_khr                  = 1000470000,
    a8_unorm_khr                               = 1000470001,
};

export enum class image_aspect : std::uint32_t {
    none           = 0,
    color          = 0x00000001,
    depth          = 0x00000002,
    stencil        = 0x00000004,
    metadata       = 0x00000008,
    plane_0        = 0x00000010,
    plane_1        = 0x00000020,
    plane_2        = 0x00000040,
    memory_plane_0 = 0x00000080,
    memory_plane_1 = 0x00000100,
    memory_plane_2 = 0x00000200,
    memory_plane_3 = 0x00000400,
};

export enum class image_usage : std::uint32_t {
    transfer_src                     = 0x00000001,
    transfer_dst                     = 0x00000002,
    sampled                          = 0x00000004,
    storage                          = 0x00000008,
    color_attachment                 = 0x00000010,
    depth_stencil_attachment         = 0x00000020,
    transient_attachment             = 0x00000040,
    input_attachment                 = 0x00000080,
    video_decode_dst                 = 0x00000400,
    video_decode_src                 = 0x00000800,
    video_decode_dpb                 = 0x00001000,
    fragment_density_map             = 0x00000200,
    fragment_shading_rate_attachment = 0x00000100,
    host_transfer                    = 0x00400000,
    video_encode_dst                 = 0x00002000,
    video_encode_src                 = 0x00004000,
    video_encode_dpb                 = 0x00008000,
    attachment_feedback_loop         = 0x00080000,
    invocation_mask                  = 0x00040000,
    sample_weight                    = 0x00100000,
    sample_block_match               = 0x00200000,
};

export constexpr image_aspect operator|(const image_aspect& lhs, const image_aspect& rhs)
{
    using value_t = std::underlying_type_t<image_aspect>;
    return static_cast<image_aspect>(static_cast<value_t>(lhs) | static_cast<value_t>(rhs));
}

export constexpr image_usage operator|(const image_usage& lhs, const image_usage& rhs)
{
    using value_t = std::underlying_type_t<image_usage>;
    return static_cast<image_usage>(static_cast<value_t>(lhs) | static_cast<value_t>(rhs));
}

constexpr image_aspect operator&(const image_aspect& lhs, const image_aspect& rhs)
{
    using value_t = std::underlying_type_t<image_aspect>;
    return static_cast<image_aspect>(static_cast<value_t>(lhs) & static_cast<value_t>(rhs));
}

constexpr bool operator==(const image_aspect& lhs, std::uint32_t rhs)
{
    using value_t = std::underlying_type_t<image_aspect>;
    return static_cast<value_t>(lhs) == rhs;
}

constexpr bool operator!=(const image_aspect& lhs, std::uint32_t rhs)
{
    return !(lhs == rhs);
}

export struct ImageTextureCreateInfo
{
    image_view_type imageType{}; // view type (1D/2D/3D/cube/array)
    format imageFormat{};
    image_layout layout{}; // final layout after upload
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t depth{};
    std::uint32_t mipCount{}; // 0 = auto-compute from dimensions
    std::uint32_t arrayCount{};
    std::uint32_t sampleCount{};
    image_tiling tiling{};
    image_usage usage{};
    memory_property memoryProperty{};
    component_swizzle rSwizzle{};
    component_swizzle gSwizzle{};
    component_swizzle bSwizzle{};
    component_swizzle aSwizzle{};
    image_aspect aspectFlag{};
    std::uint32_t mipOffset{};
    std::uint32_t layerOffset{};
    std::uint32_t layerCount{};
    std::span<const std::uint8_t> pixelData; // non-owning — caller keeps data alive
};

export struct RenderTextureCreateInfo
{
    format imageFormat{};
    image_aspect aspect{};
    std::uint32_t width{};
    std::uint32_t height{};
};

export struct Texture
{
    deer_vulkan::Image image{};
    deer_vulkan::ImageView view{};
    deer_vulkan::Sampler sampler{};
};

// Maps image_view_type → VkImageType (image_type) for ImageCreateInfo.
[[nodiscard]] constexpr auto ViewTypeToImageType(const image_view_type vt) noexcept -> std::uint8_t
{
    switch (vt)
    {
    case image_view_type::type_1d:
    case image_view_type::type_1d_array: return static_cast<std::uint8_t>(image_type::type_1d);
    case image_view_type::type_3d: return static_cast<std::uint8_t>(image_type::type_3d);
    default: // 2D, cube, cube_array, 2D_array all use VkImageType 2D
        return static_cast<std::uint8_t>(image_type::type_2d);
    }
}

export [[nodiscard]] inline auto Initialize(const Renderer& renderer, const ImageTextureCreateInfo& createInfo, Texture& texture) noexcept -> gfx_status
{
    if (createInfo.pixelData.empty()) [[unlikely]]
    {
        return gfx_status::not_ok;
    }

    const std::uint32_t idealMip = (createInfo.mipCount == 0U)
        ? static_cast<std::uint32_t>(std::floor(std::log2(static_cast<float>(std::max({createInfo.width, createInfo.height, createInfo.depth}))))) + 1U
        : createInfo.mipCount;

    // Upload via staging buffer
    Buffer stagingBuffer;
    const uint64_t byteSize = static_cast<uint64_t>(createInfo.pixelData.size_bytes());
    if (Initialize(renderer, byteSize, buffer_usage::transfer_src, memory_property::host_visible | memory_property::host_coherent, stagingBuffer) != gfx_status::ok) [[unlikely]]
    {
        return gfx_status::not_ok;
    }

    CopyData(renderer, stagingBuffer, createInfo.pixelData);

    const deer_vulkan::ImageCreateInfo imageInfo{
        .format         = static_cast<std::uint32_t>(createInfo.imageFormat),
        .width          = createInfo.width,
        .height         = createInfo.height,
        .depth          = createInfo.depth,
        .mipCount       = idealMip,
        .arrayCount     = createInfo.arrayCount,
        .sampleCount    = createInfo.sampleCount,
        .usage          = static_cast<std::uint32_t>(createInfo.usage),
        .memoryProperty = static_cast<std::uint32_t>(createInfo.memoryProperty),
        .imageType      = ViewTypeToImageType(createInfo.imageType),
        .tiling         = static_cast<std::uint8_t>(createInfo.tiling),
    };

    GFX_CHECK_CLEANUP(deer_vulkan::Initialize(renderer.dispatch, renderer.device, renderer.physical, imageInfo, texture.image), "Failed to Initialize image.")
    deer_vulkan::TransitionImageLayout(renderer.dispatch, renderer.commandBuffer, texture.image, static_cast<std::uint32_t>(image_layout::transfer_dst_optimal),
                                       createInfo.mipOffset, idealMip, createInfo.layerOffset, createInfo.layerCount);
    deer_vulkan::CopyToImage(renderer.dispatch, renderer.commandBuffer, stagingBuffer.buffer, texture.image, createInfo.width, createInfo.height, createInfo.depth);

    Cleanup(renderer, stagingBuffer);

    if (createInfo.mipCount == idealMip)
    {
        deer_vulkan::TransitionImageLayout(renderer.dispatch, renderer.commandBuffer, texture.image, static_cast<std::uint32_t>(createInfo.layout), createInfo.mipOffset, idealMip,
                                           createInfo.layerOffset, createInfo.layerCount);
    }
    else
    {
        GFX_CHECK(deer_vulkan::GenerateMips(renderer.dispatch, renderer.commandBuffer, texture.image, createInfo.width, createInfo.height, idealMip), "Failed to generate mips.")
    }

    const deer_vulkan::ImageViewCreateInfo viewInfo{
        .format      = static_cast<std::uint32_t>(createInfo.imageFormat),
        .aspectFlag  = static_cast<std::uint32_t>(createInfo.aspectFlag),
        .mipOffset   = createInfo.mipOffset,
        .mipCount    = idealMip,
        .layerOffset = createInfo.layerOffset,
        .layerCount  = createInfo.layerCount,
        .imageType   = static_cast<std::uint8_t>(createInfo.imageType),
        .rSwizzle    = static_cast<std::uint8_t>(createInfo.rSwizzle),
        .gSwizzle    = static_cast<std::uint8_t>(createInfo.gSwizzle),
        .bSwizzle    = static_cast<std::uint8_t>(createInfo.bSwizzle),
        .aSwizzle    = static_cast<std::uint8_t>(createInfo.aSwizzle),
    };
    GFX_CHECK(deer_vulkan::Initialize(renderer.dispatch, renderer.device, texture.image, viewInfo, texture.view), "Failed to Initialize the image view.")

    const deer_vulkan::SamplerCreateInfo samplerInfo{
        .useLinear        = true,
        .mipLinear        = true,
        .repeatU          = 0U,
        .repeatV          = 0U,
        .repeatW          = 0U,
        .anisotropyEnable = false,
        .maxAnisotropy    = 0.0F,
        .maxLod           = static_cast<float>(idealMip),
    };
    GFX_CHECK(deer_vulkan::Initialize(renderer.dispatch, renderer.device, samplerInfo, texture.sampler), "Failed to Initialize the sampler.")

    return gfx_status::ok;
}

export [[nodiscard]] inline auto Initialize(const Renderer& renderer, const RenderTextureCreateInfo& createInfo, Texture& texture) noexcept -> gfx_status
{
    const bool isDepth = ((createInfo.aspect & image_aspect::depth) | (createInfo.aspect & image_aspect::stencil)) != 0;

    const image_usage usage = isDepth ? (image_usage::depth_stencil_attachment | image_usage::sampled) : (image_usage::color_attachment | image_usage::sampled);

    const deer_vulkan::ImageCreateInfo imageInfo{
        .format         = static_cast<std::uint32_t>(createInfo.imageFormat),
        .width          = createInfo.width,
        .height         = createInfo.height,
        .depth          = 1U,
        .mipCount       = 1U,
        .arrayCount     = 1U,
        .sampleCount    = 1U,
        .usage          = static_cast<std::uint32_t>(usage),
        .memoryProperty = static_cast<std::uint32_t>(memory_property::device_local),
        .imageType      = static_cast<std::uint8_t>(image_type::type_2d),
        .tiling         = static_cast<std::uint8_t>(image_tiling::optimal),
    };

    GFX_CHECK(deer_vulkan::Initialize(renderer.dispatch, renderer.device, renderer.physical, imageInfo, texture.image), "Failed to Initialize render target image.")

    const deer_vulkan::ImageViewCreateInfo viewInfo{
        .format      = static_cast<std::uint32_t>(createInfo.imageFormat),
        .aspectFlag  = static_cast<std::uint32_t>(createInfo.aspect),
        .mipOffset   = 0U,
        .mipCount    = 1U,
        .layerOffset = 0U,
        .layerCount  = 1U,
        .imageType   = static_cast<std::uint8_t>(image_view_type::type_2d),
        .rSwizzle    = static_cast<std::uint8_t>(component_swizzle::r),
        .gSwizzle    = static_cast<std::uint8_t>(component_swizzle::g),
        .bSwizzle    = static_cast<std::uint8_t>(component_swizzle::b),
        .aSwizzle    = static_cast<std::uint8_t>(component_swizzle::a),
    };

    GFX_CHECK(deer_vulkan::Initialize(renderer.dispatch, renderer.device, texture.image, viewInfo, texture.view), "Failed to Initialize render target image view.")

    constexpr deer_vulkan::SamplerCreateInfo samplerInfo{
        .useLinear        = true,
        .mipLinear        = true,
        .repeatU          = 2U,
        .repeatV          = 2U,
        .repeatW          = 2U,
        .anisotropyEnable = true,
        .maxAnisotropy    = 8.0F,
        .maxLod           = 1.0F,
    };
    GFX_CHECK(deer_vulkan::Initialize(renderer.dispatch, renderer.device, samplerInfo, texture.sampler), "Failed to Initialize render target sampler.")

    return gfx_status::ok;
}

export inline auto Cleanup(const Renderer& renderer, Texture& texture) noexcept -> void
{
    Cleanup(renderer.dispatch, renderer.device, texture.sampler);
    Cleanup(renderer.dispatch, renderer.device, texture.view);
    Cleanup(renderer.dispatch, renderer.device, texture.image);
}
} // namespace fawn_vision
