//
// Created by joran on 02/01/2024.
//

#pragma once
#include "FawnVision_Core.h"

namespace FawnVision
{
    struct Window_T
    {
        SDL_Window* pWindow { nullptr };
        uint32_t flags {};
        int32_t width {};
        int32_t height {};

#ifdef BALBINO_VULKAN
        char const* const* extensions { BALBINO_NULL };
        uint32_t extensionCount {};
#endif
    };

    struct RenderSettings
    {
        uint8_t aaMode {};
        uint8_t vsyncMode {};
    };

    struct Renderer_T
    {
        RenderSettings settings{};
        CInstance instance{};
        CPresenter presenter{};
        CRenderPipeline renderPipeline{};
    };
}// namespace FawnVision
