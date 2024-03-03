//
// Copyright (c) 2023.
// Author: Joran
//

#include "FawnVision_Window.hpp"

#include "Error.hpp"
#include "ObjectDefinitions.hpp"

#include <SDL3/SDL.h>

namespace FawnVision
{
auto Initialize() -> int32_t
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR ) != 0)
    {
        SetError(error_code::generic, "SDL initialization failed: %s", SDL_GetError());
        return -1;
    }
    return 0;
}

auto CreateWindow(const WindowCreateInfo& createInfo, Window& window) noexcept -> int32_t
{
    if (window != nullptr)
    {
        SetError(error_code::generic, "Window is already created");
        return -1;
    }

    int screenCount{};
    const SDL_DisplayID* pDisplayList{SDL_GetDisplays(&screenCount)};
    const SDL_DisplayMode* pCurrent{nullptr};
    if (createInfo.screen == -1)
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
            SetError(error_code::generic, "Invalid screen requested. Available screens: %d Requested screen: %d", screenCount, createInfo.screen);
            return -1;
        }

        pCurrent = SDL_GetDesktopDisplayMode(createInfo.screen);
    }
    if (pCurrent == nullptr)
    {
        SetError(error_code::generic, "Window is not initialized");
        return -1;
    }
    if (createInfo.width > pCurrent->w || createInfo.height > pCurrent->h)
    {
        SetError(error_code::generic, "Requested window size (%d x %d) is larger than the available screen size (%d x %d)", createInfo.width, createInfo.height, pCurrent->w, pCurrent->h);
        return -1;
    }

    SDL_Window* pWindow{SDL_CreateWindow(&createInfo.name[0], createInfo.width, createInfo.height, createInfo.flags)};
    if (pWindow == nullptr)
    {
        SetError(error_code::generic, "SDL window creation failed: %s", SDL_GetError());
        return -1;
    }

    window = new(std::nothrow) Window_T{pWindow, createInfo.flags, createInfo.width, createInfo.height};
    if (window == nullptr)
    {
        SetError(error_code::out_of_memory, "Failed to allocate memory for the window");
        return -1;
    }

    window->extensions = SDL_Vulkan_GetInstanceExtensions(&window->extensionCount);
    if (window->extensions == nullptr)
    {
        SetError(error_code::generic, "SDL Vulkan extension retrieval failed: %s", SDL_GetError());
        return -1;
    }

    return 0;
}

auto SetWindowFlags(const Window& window, const uint32_t flags) noexcept -> int32_t
{
    if (window == nullptr)
    {
        SetError(error_code::generic, "Window is not initialized");
        return -1;
    }

    if (SDL_SetWindowBordered(window->pWindow, (flags & SDL_WINDOW_BORDERLESS) != 0U ? SDL_FALSE : SDL_TRUE) != 0
        || SDL_SetWindowResizable(window->pWindow, (flags & SDL_WINDOW_RESIZABLE) != 0U ? SDL_TRUE : SDL_FALSE) != 0
        || SDL_SetWindowFullscreen(window->pWindow, (flags & SDL_WINDOW_FULLSCREEN) != 0U ? SDL_TRUE : SDL_FALSE) != 0
        || SDL_SetWindowFocusable(window->pWindow, (flags & SDL_WINDOW_NOT_FOCUSABLE) != 0U ? SDL_FALSE : SDL_TRUE) != 0)
    {
        SetError(error_code::generic, "Failed to set window flags: %s", SDL_GetError());
        return -1;
    }

    if ((flags & SDL_WINDOW_MAXIMIZED) != 0U)
    {
        if (SDL_MaximizeWindow(window->pWindow) != 0)
        {
            SetError(error_code::generic, "Failed to maximize window: %s", SDL_GetError());
            return -1;
        }
    }

    if ((flags & SDL_WINDOW_MINIMIZED) != 0U)
    {
        if (SDL_MinimizeWindow(window->pWindow) != 0)
        {
            SetError(error_code::generic, "Failed to minimize window: %s", SDL_GetError());
            return -1;
        }
    }

    window->flags = flags;
    return 0;
}

auto ToggleWindowFlags(const Window& window, const uint32_t flags) noexcept -> int32_t
{
    return SetWindowFlags(window, window->flags ^ flags);
}

auto SetWindowSize(const Window& window, const int width, const int height) noexcept -> int32_t
{
    if (window == nullptr)
    {
        SetError(error_code::generic, "Window is not initialized");
        return -1;
    }

    window->width = width;
    window->height = height;
    SDL_SetWindowSize(window->pWindow, width, height);
    SDL_SetWindowPosition(window->pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    return 0;
}

auto GetWindowSize(const Window& window, int& width, int& height) noexcept -> int32_t
{
    if (window == nullptr)
    {
        SetError(error_code::generic, "Window is not initialized");
        return -1;
    }

    width = window->width;
    height = window->height;
    return 0;
}

auto SetWindowPosition(const Window& window, const int xPosition, const int yPosition) noexcept -> int32_t
{
    if (window == nullptr)
    {
        SetError(error_code::generic, "Window is not initialized");
        return -1;
    }

    SDL_SetWindowPosition(window->pWindow, xPosition, yPosition);
    return 0;
}

auto GetWindowPosition(const Window& window, int& xPosition, int& yPosition) noexcept -> int32_t
{
    if (window == nullptr)
    {
        SetError(error_code::generic, "Window is not initialized");
        return -1;
    }

    SDL_GetWindowPosition(window->pWindow, &xPosition, &yPosition);
    return 0;
}

auto ReleaseWindow(Window& window) noexcept -> int32_t
{
    if (window == nullptr)
    {
        SetError(error_code::generic, "Window is not initialized");
        return -1;
    }

    SDL_DestroyWindow(window->pWindow);
    delete window;
    window = nullptr;
    return 0;
}
} // namespace FawnVision