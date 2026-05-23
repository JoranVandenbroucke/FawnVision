//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "api/vulkan/wrapper/shader.hpp"

export module FawnVision:Shader;
import :Enum;
import :Renderer;
import :Descriptor;
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
export struct ShaderData
{
    shader_stage stage{};
    std::vector<uint8_t> code{}; // owns the SPIR-V bytes
};

export struct Shader
{
    deer_vulkan::Shader shader{};
};

export template <auto Size = std::dynamic_extent>
[[nodiscard]] inline auto CreateShader(const Renderer& renderer, const std::span<const ShaderData, Size>& shaderCreateInfo, Shader& shader,
                                       const Descriptor* descriptor = nullptr) noexcept -> gfx_status
{
    const std::size_t count = shaderCreateInfo.size();

    std::vector<std::uint32_t> stages(count);
    std::vector<std::vector<std::uint8_t>> codes(count);

    for (std::size_t i = 0; i < count; ++i)
    {
        stages[i] = static_cast<std::uint32_t>(shaderCreateInfo[i].stage);
        codes[i]  = shaderCreateInfo[i].code; // copy is unavoidable — Vulkan layer owns nothing
    }

    const deer_vulkan::Descriptor empty{};
    const deer_vulkan::Descriptor& vkDesc = descriptor ? descriptor->descriptor : empty;
    GFX_CHECK(deer_vulkan::Initialize(renderer.dispatch, renderer.device, std::span<const std::uint32_t, std::dynamic_extent>(stages),
                                      std::span<const std::vector<std::uint8_t>, std::dynamic_extent>(codes), vkDesc, shader.shader),
              "failed to initialize shader");
    return gfx_status::ok;
}

export inline auto Cleanup(const Renderer& renderer, Shader& shader) noexcept -> void
{
    Cleanup(renderer.dispatch, renderer.device, shader.shader);
}
} // namespace fawn_vision
