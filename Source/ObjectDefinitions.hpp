//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "FawnVision_Core.hpp"

namespace FawnVision
{
struct Window_T final
{
    SDL_Window* pWindow{nullptr};
    uint32_t flags{};
    int32_t width{};
    int32_t height{};

#ifdef BALBINO_VULKAN
    const char* const* extensions{BALBINO_NULL};
    uint32_t extensionCount{};
#endif
};

struct SRenderSettings final
{
    uint8_t aaMode{};
    uint8_t vsyncMode{};
};

struct Renderer_T final
{
    SRenderSettings settings{};
    CInstance instance{};
    CPresenter presenter{};
    std::vector<RenderPassInfo> renderPasses;
    size_t currentRenderPassIndex{};
};

struct Shader_T final
{
    CShader shader{};
};

struct Material_T final
{
    Shader shader{BALBINO_NULL};
    uint32_t primitiveTopology{};
};
} // namespace FawnVision