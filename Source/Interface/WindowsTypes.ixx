//
// Copyright (c) 2024.
// Author: Joran.
//
module;
#include <cstdint>
#include <SDL3/SDL_video.h>

export module FawnVision.WindowTypes;

namespace FawnVision
{
export struct SWindow
{
    SDL_Window* pWindow{nullptr};
    int32_t width{0};
    int32_t height{0};
    uint32_t flags{0};
    uint32_t extensionCount{0};
    char const* const* extensions{nullptr};
};

export struct SWindowCreateInfo
{
    const char8_t* const name{};
    uint8_t screen{0};
    int32_t width{0};
    int32_t height{0};
    uint32_t flags{0};
};
} // namespace FawnVision
