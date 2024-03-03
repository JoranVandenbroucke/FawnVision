//
// Copyright (c) 2023.
// Author: Joran
//

#pragma once
#include "Versions.hpp"

#include <cstdint>
#include <vector>

#define FV_DEFINE_HANDLE(object) using object = struct object##_T*

#ifdef BALBINO_VULKAN
#    include "DeerVulkan.h"
#endif

namespace FawnVision
{
constexpr uint32_t VERSION{MAKE_VERSION(1, 1, 0)}; // Version 0.1
constexpr uint32_t MAX_STRING_SIZE{128};

#ifdef BALBINO_VULKAN
#    define CInstance DeerVulkan::CInstance
#    define CPresenter DeerVulkan::CPresenter
#    define CShader DeerVulkan::CShader
#else
#endif

enum window_flags : uint32_t
{
    fullscreen         = 0x00000001U,
    opengl             = 0x00000002U,
    occluded           = 0x00000004U,
    hidden             = 0x00000008U,
    borderless         = 0x00000010U,
    resizable          = 0x00000020U,
    minimized          = 0x00000040U,
    maximized          = 0x00000080U,
    mouse_grabbed      = 0x00000100U,
    input_focus        = 0x00000200U,
    mouse_focus        = 0x00000400U,
    external           = 0x00000800U,
    high_pixel_density = 0x00002000U,
    mouse_capture      = 0x00004000U,
    always_on_top      = 0x00008000U,
    utility            = 0x00020000U,
    tooltip            = 0x00040000U,
    popup_menu         = 0x00080000U,
    keyboard_grabbed   = 0x00100000U,
    vulkan             = 0x10000000U,
    metal              = 0x20000000U,
    transparent        = 0x40000000U,
    not_focusable      = 0x80000000U,
};

enum class texture_type : uint8_t
{
    type_1d         = 0,
    type_2d         = 1,
    type_3d         = 2,
    type_cube       = 3,
    type_1d_array   = 4,
    type_2d_array   = 5,
    type_cube_array = 6,
    max_type
};

enum class texture_format : uint8_t
{
    r8_g8_b8_a8_srgb = 0,
    max_format
};
enum class component_swizzle : uint8_t
{
    identity = 0,
    zero     = 1,
    one      = 2,
    r        = 3,
    g        = 4,
    b        = 5,
    a        = 6,
    max_swizzle
};
enum class compare_operator : uint8_t
{
    never            = 0,
    less             = 1,
    equal            = 2,
    less_or_equal    = 3,
    greater          = 4,
    not_equal        = 5,
    greater_or_equal = 6,
    always           = 7,
    max_operator
};

enum class primitive_topology : uint8_t
{
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
    max_topology
};

enum class polygon_mode : uint8_t
{
    fill  = 0,
    line  = 1,
    point = 2,
    max_enum
};

enum class shader_type : uint16_t
{
    vertex                  = 0X00000001,
    tessellation_control    = 0X00000002,
    tessellation_evaluation = 0X00000004,
    geometry                = 0X00000008,
    fragment                = 0X00000010,
    compute                 = 0X00000020,
    task_bit                = 0X00000040,
    mesh_bit                = 0X00000080,
    raygen                  = 0X00000100,
    any_hit                 = 0X00000200,
    closest_hit             = 0X00000400,
    miss_bit                = 0X00000800,
    intersection            = 0X00001000,
    callable                = 0X00002000,
    subpass_shading         = 0X00004000,
    max_shader_type
};
FV_DEFINE_HANDLE(Window);
FV_DEFINE_HANDLE(Renderer);
FV_DEFINE_HANDLE(Texture);
FV_DEFINE_HANDLE(Shader);
FV_DEFINE_HANDLE(Material);
FV_DEFINE_HANDLE(Mesh);

struct ComponentMapping
{
    component_swizzle r{};
    component_swizzle g{};
    component_swizzle b{};
    component_swizzle a{};
};

struct DepthSettings
{
    compare_operator depthCompareOp{compare_operator::always};
    bool enableDepthTest{true};
    bool enableWriteDepth{true};
};

struct WindowCreateInfo
{
    char name[MAX_STRING_SIZE]{};
    int32_t width{};
    int32_t height{};
    uint32_t flags{};
    int32_t screen{-1};
};
struct RendererCreateInfo
{
    Window* window{BALBINO_NULL};
    uint32_t appVersion{};
};
struct TextureCreateInfo
{
    const uint8_t* pixelData{};
    ComponentMapping componentMapping{};
    uint32_t width{};
    uint32_t height{};
    uint32_t depth{};
    uint32_t layerCount{};
    int32_t mipsCount{};
    uint8_t sampleCount{};
    bool tiling{};
    texture_type type{};
    texture_format format{};
};
struct ShaderCreateInfo
{
    std::vector<uint8_t> code;
    char name[MAX_STRING_SIZE]{};
    shader_type shaderType{};
    bool isBinary{};
};
struct MaterialCreateInfo
{
    Shader shader;
    primitive_topology primitiveTopology{};
};
struct MeshCreateInfo
{
    std::vector<Material> shaders;
};
struct RenderPassInfo
{
    struct Viewport
    {
        float xPos{};
        float yPos{};
        float width{};
        float height{};
    };
    enum class render_pass_type : uint8_t
    {
        object_shader,
        override_shader,
        fullscreen_shader,
        max_render_pass_type,
    };
    Viewport viewport{};
    Material material{BALBINO_NULL};
    DepthSettings depthSettings{};
    render_pass_type renderPassType{render_pass_type::object_shader};
    bool isFrontFaceClockWice{true};
};
auto Initialize() -> int32_t;
} // namespace FawnVision
