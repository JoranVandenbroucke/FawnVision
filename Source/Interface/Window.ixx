//
// Copyright (c) 2023.
// Author: Joran
//

module;

#include <array>
#include <cstdint>
#include <cstring>
#include <new>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

export module FawnVision.Window;

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
    std::array<char8_t, 64> name{};
    uint8_t screen{0};
    int32_t width{0};
    int32_t height{0};
    uint32_t flags{0};
};

export auto Initialize() -> int32_t
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR) != 0)
    {
        return -1;
    }
    return 0;
}

export auto CreateWindow(const SWindowCreateInfo& createInfo, SWindow& window) noexcept -> int32_t
{
    constexpr SWindow empty{};
    if (std::memcmp(&window, &empty, sizeof(SWindow)) != 0)
    {
        return -1;
    }

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
    if (pCurrent == nullptr)
    {
        // SetError(error_code::generic, "Window is not initialized");
        return -1;
    }
    if (createInfo.width > pCurrent->w || createInfo.height > pCurrent->h)
    {
        return -1;
    }

    SDL_Window* pWindow{SDL_CreateWindow(std::bit_cast<const char*>(createInfo.name.data()), createInfo.width, createInfo.height, createInfo.flags)};
    if (pWindow == nullptr)
    {
        return -1;
    }

    window = SWindow{pWindow, createInfo.width, createInfo.height, createInfo.flags};

    window.extensions = SDL_Vulkan_GetInstanceExtensions(&window.extensionCount);
    if (window.extensions == nullptr)
    {
        return -1;
    }

    return 0;
}

export auto SetWindowFlags(SWindow& window, const uint32_t flags) noexcept -> int32_t
{
    constexpr SWindow empty{};
    if (std::memcmp(&window, &empty, sizeof(SWindow)) == 0)
    {
        return -1;
    }

    if (SDL_SetWindowBordered(window.pWindow, (flags & SDL_WINDOW_BORDERLESS) != 0U ? SDL_FALSE : SDL_TRUE) != 0
        || SDL_SetWindowResizable(window.pWindow, (flags & SDL_WINDOW_RESIZABLE) != 0U ? SDL_TRUE : SDL_FALSE) != 0
        || SDL_SetWindowFullscreen(window.pWindow, (flags & SDL_WINDOW_FULLSCREEN) != 0U ? SDL_TRUE : SDL_FALSE) != 0
        || SDL_SetWindowFocusable(window.pWindow, (flags & SDL_WINDOW_NOT_FOCUSABLE) != 0U ? SDL_FALSE : SDL_TRUE) != 0)
    {
        return -1;
    }

    if ((flags & SDL_WINDOW_MAXIMIZED) != 0U)
    {
        if (SDL_MaximizeWindow(window.pWindow) != 0)
        {
            return -1;
        }
    }

    if ((flags & SDL_WINDOW_MINIMIZED) != 0U)
    {
        if (SDL_MinimizeWindow(window.pWindow) != 0)
        {
            return -1;
        }
    }

    window.flags = flags;
    return 0;
}

export auto ToggleWindowFlags(SWindow& window, const uint32_t flags) noexcept -> int32_t
{
    return SetWindowFlags(window, window.flags ^ flags);
}

export auto SetWindowSize(SWindow& window, const int width, const int height) noexcept -> int32_t
{
    constexpr SWindow empty{};
    if (std::memcmp(&window, &empty, sizeof(SWindow)) == 0)
    {
        return -1;
    }

    window.width = width;
    window.height = height;
    SDL_SetWindowSize(window.pWindow, width, height);
    SDL_SetWindowPosition(window.pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    return 0;
}

export auto GetWindowSize(const SWindow& window, int& width, int& height) noexcept -> int32_t
{
    constexpr SWindow empty{};
    if (std::memcmp(&window, &empty, sizeof(SWindow)) == 0)
    {
        return -1;
    }

    width = window.width;
    height = window.height;
    return 0;
}

export auto SetWindowPosition(const SWindow& window, const int xPosition, const int yPosition) noexcept -> int32_t
{
    constexpr SWindow empty{};
    if (std::memcmp(&window, &empty, sizeof(SWindow)) == 0)
    {
        return -1;
    }

    SDL_SetWindowPosition(window.pWindow, xPosition, yPosition);
    return 0;
}

export auto GetWindowPosition(const SWindow& window, int& xPosition, int& yPosition) noexcept -> int32_t
{
    constexpr SWindow empty{};
    if (std::memcmp(&window, &empty, sizeof(SWindow)) == 0)
    {
        return -1;
    }

    SDL_GetWindowPosition(window.pWindow, &xPosition, &yPosition);
    return 0;
}

export auto ReleaseWindow(SWindow& window) noexcept -> int32_t
{
    constexpr SWindow empty{};
    if (std::memcmp(&window, &empty, sizeof(SWindow)) == 0)
    {
        return -1;
    }

    SDL_DestroyWindow(window.pWindow);
    std::memset(&window, sizeof(SWindow), 0);
    return 0;
}
} // namespace FawnVision