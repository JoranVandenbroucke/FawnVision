//
// Copyright (c) 2023.
// Author: Joran
//

module;

#include "compiler.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <bit>
#include <cstdint>

export module FawnVision.Window;

import FawnVision.WindowConstants;
import FawnVision.WindowTypes;

namespace FawnVision
{
export inline auto InitializeSDL() -> int32_t
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR) != 0)
    {
        return -1;
    }
    return 0;
}

export inline auto CreateWindow(const SWindowCreateInfo& createInfo, SWindow& window) noexcept -> int32_t
{
    int screenCount{};
    const SDL_DisplayID* pDisplayList{SDL_GetDisplays(&screenCount)};
    const SDL_DisplayMode* pCurrent{nullptr};

    if (createInfo.screen == 0)
    {
        for (const SDL_DisplayID* pDisplayId{pDisplayList}; pDisplayId != nullptr; ++pDisplayId)
        {
            pCurrent = SDL_GetDesktopDisplayMode(*pDisplayId);
            if (pCurrent != nullptr)
            {
                break;
            }
        }
    }
    else
    {
        if (createInfo.screen >= screenCount)
        {
            return -1;
        }
        pCurrent = SDL_GetDesktopDisplayMode(createInfo.screen);
    }

    if (pCurrent == nullptr || createInfo.width > pCurrent->w || createInfo.height > pCurrent->h)
    {
        return -1;
    }

    SDL_Window* pWindow{SDL_CreateWindow(std::bit_cast<const char*>(createInfo.name), createInfo.width, createInfo.height, createInfo.flags)};
    if (pWindow == nullptr)
    {
        return -1;
    }

    window            = SWindow{pWindow, createInfo.width, createInfo.height, createInfo.flags};
    window.extensions = SDL_Vulkan_GetInstanceExtensions(&window.extensionCount);
    if (window.extensions == nullptr)
    {
        return -1;
    }

    return 0;
}

export inline void ReleaseWindow(const SWindow& window) noexcept
{
    SDL_DestroyWindow(window.pWindow);
}

export inline auto SetWindowFlags(SWindow& window, const uint32_t flags) noexcept -> int32_t
{
    if (SDL_SetWindowBordered(window.pWindow, (flags & SDL_WINDOW_BORDERLESS) != 0U ? SDL_FALSE : SDL_TRUE) != 0 || SDL_SetWindowResizable(window.pWindow, (flags & SDL_WINDOW_RESIZABLE) != 0U ? SDL_TRUE : SDL_FALSE) != 0
        || SDL_SetWindowFullscreen(window.pWindow, (flags & SDL_WINDOW_FULLSCREEN) != 0U ? SDL_TRUE : SDL_FALSE) != 0 || SDL_SetWindowFocusable(window.pWindow, (flags & SDL_WINDOW_NOT_FOCUSABLE) != 0U ? SDL_FALSE : SDL_TRUE) != 0)
    {
        return -1;
    }

    if ((flags & SDL_WINDOW_MAXIMIZED) != 0U && SDL_MaximizeWindow(window.pWindow) != 0)
    {
        return -1;
    }

    if ((flags & SDL_WINDOW_MINIMIZED) != 0U && SDL_MinimizeWindow(window.pWindow) != 0)
    {
        return -1;
    }

    window.flags = flags;
    return 0;
}

export inline auto ToggleWindowFlags(SWindow& window, const uint32_t flags) noexcept -> int32_t
{
    return SetWindowFlags(window, window.flags ^ flags);
}

export inline auto SetWindowSize(SWindow& window, const int width, const int height) noexcept -> int32_t
{
    window.width  = width;
    window.height = height;
    SDL_SetWindowSize(window.pWindow, width, height);
    SDL_SetWindowPosition(window.pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    return 0;
}

export BALBINO_CONSTEXPR_SINCE_CXX11 auto GetWindowSize(const SWindow& window, int& width, int& height) noexcept -> int32_t
{
    width  = window.width;
    height = window.height;
    return 0;
}

export BALBINO_CONSTEXPR_SINCE_CXX11 auto GetWindowSize(SWindow& window) noexcept -> int32_t
{
    SDL_GetWindowSize(window.pWindow, &window.width, &window.height);
    return 0;
}

export inline auto SetWindowPosition(const SWindow& window, const int xPosition = SDL_WINDOWPOS_CENTERED, const int yPosition = SDL_WINDOWPOS_CENTERED) noexcept -> int32_t
{
    SDL_SetWindowPosition(window.pWindow, xPosition, yPosition);
    return 0;
}

export inline auto GetWindowPosition(const SWindow& window, int& xPosition, int& yPosition) noexcept -> int32_t
{
    SDL_GetWindowPosition(window.pWindow, &xPosition, &yPosition);
    return 0;
}
} // namespace FawnVision
