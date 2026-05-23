//
// Copyright (c) 2026.
// Author: Joran
//

module;
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

export module FawnVision:Window;
import FawnAlgebra;

import std;

namespace fawn_vision
{

export enum class window_flags : std::uint32_t {
    none               = 0,
    fullscreen         = 0x00000001U,
    open_gl            = 0x00000002U,
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

export enum class event_type : std::uint32_t {
    quit              = SDL_EVENT_QUIT,
    window_resized    = SDL_EVENT_WINDOW_RESIZED,
    window_closed     = SDL_EVENT_WINDOW_CLOSE_REQUESTED,
    key_down          = SDL_EVENT_KEY_DOWN,
    key_up            = SDL_EVENT_KEY_UP,
    mouse_move        = SDL_EVENT_MOUSE_MOTION,
    mouse_button_down = SDL_EVENT_MOUSE_BUTTON_DOWN,
    mouse_button_up   = SDL_EVENT_MOUSE_BUTTON_UP,
    mouse_wheel       = SDL_EVENT_MOUSE_WHEEL,
};

export struct EventContext
{
    event_type type;
    const SDL_Event& raw; // expose raw for callers that need scancode etc.
};

// Use the same flag operator pattern as :Enum rather than flagA hand-rolled one.
export [[nodiscard]] constexpr auto operator|(const window_flags flagA, const window_flags flagB) noexcept -> window_flags
{
    using U = std::underlying_type_t<window_flags>;
    return static_cast<window_flags>(static_cast<U>(flagA) | static_cast<U>(flagB));
}
export [[nodiscard]] constexpr auto operator&(const window_flags flagA, const window_flags flagB) noexcept -> window_flags
{
    using U = std::underlying_type_t<window_flags>;
    return static_cast<window_flags>(static_cast<U>(flagA) & static_cast<U>(flagB));
}
export [[nodiscard]] constexpr auto operator^(const window_flags flagA, const window_flags flagB) noexcept -> window_flags
{
    using U = std::underlying_type_t<window_flags>;
    return static_cast<window_flags>(static_cast<U>(flagA) ^ static_cast<U>(flagB));
}
export [[nodiscard]] constexpr auto HasFlag(const window_flags value, const window_flags flag) noexcept -> bool
{
    return (value & flag) == flag;
}

export struct WindowCreateInfo
{
    const char8_t* name{nullptr};
    std::uint8_t screen{};
    std::int32_t width{};
    std::int32_t height{};
    window_flags flags{window_flags::none};
};

export struct Window
{
    SDL_Window* pWindow{nullptr};
    std::int32_t width{};
    std::int32_t height{};
    window_flags flags{window_flags::none};
};

export [[nodiscard]] inline auto InitializeSDL() noexcept -> bool
{
    return SDL_Init(SDL_INIT_VIDEO);
}

export [[nodiscard]] inline auto CreateWindow(const WindowCreateInfo& createInfo, Window& window) noexcept -> bool
{
    if (createInfo.width <= 0 || createInfo.height <= 0) [[unlikely]]
    {
        return false;
    }

    int screenCount{};
    const SDL_DisplayID* const pDisplayList = SDL_GetDisplays(&screenCount);
    if (pDisplayList == nullptr || screenCount == 0) [[unlikely]]
    {
        return false;
    }

    SDL_DisplayID targetDisplay{};
    if (createInfo.screen == 0)
    {
        // Walk null-terminated array — check the VALUE not the pointer
        for (const SDL_DisplayID* pId = pDisplayList; *pId != 0; ++pId)
        {
            const SDL_DisplayMode* pMode = SDL_GetDesktopDisplayMode(*pId);
            if (pMode != nullptr)
            {
                targetDisplay = *pId;
                break;
            }
        }
    }
    else
    {
        if (createInfo.screen >= static_cast<std::uint8_t>(screenCount)) [[unlikely]]
        {
            return false;
        }
        targetDisplay = pDisplayList[createInfo.screen];
    }

    if (targetDisplay == 0) [[unlikely]]
    {
        return false;
    }

    const SDL_DisplayMode* const pMode = SDL_GetDesktopDisplayMode(targetDisplay);
    if (pMode == nullptr || createInfo.width > pMode->w || createInfo.height > pMode->h) [[unlikely]]
    {
        return false;
    }

    SDL_Window* const pWindow = SDL_CreateWindow(std::bit_cast<const char*>(createInfo.name), createInfo.width, createInfo.height, static_cast<SDL_WindowFlags>(createInfo.flags));

    if (pWindow == nullptr) [[unlikely]]
    {
        return false;
    }

    window.pWindow = pWindow;
    window.width   = createInfo.width;
    window.height  = createInfo.height;
    window.flags   = createInfo.flags;
    return true;
}

export inline auto ReleaseWindow(const Window& window) noexcept -> void
{
    if (window.pWindow != nullptr)
    {
        SDL_DestroyWindow(window.pWindow);
    }
}

export [[nodiscard]] inline auto SetWindowFlags(Window& window, window_flags flags) noexcept -> std::int32_t
{
    if (window.pWindow == nullptr) [[unlikely]]
    {
        return false;
    }

    if (!SDL_SetWindowBordered(window.pWindow, !HasFlag(flags, window_flags::borderless)))
    {
        return false;
    }
    if (!SDL_SetWindowResizable(window.pWindow, HasFlag(flags, window_flags::resizable)))
    {
        return false;
    }
    if (!SDL_SetWindowFullscreen(window.pWindow, HasFlag(flags, window_flags::fullscreen)))
    {
        return false;
    }
    if (!SDL_SetWindowFocusable(window.pWindow, !HasFlag(flags, window_flags::not_focusable)))
    {
        return false;
    }

    if (HasFlag(flags, window_flags::maximized) && !SDL_MaximizeWindow(window.pWindow))
    {
        return false;
    }
    if (HasFlag(flags, window_flags::minimized) && !SDL_MinimizeWindow(window.pWindow))
    {
        return false;
    }

    window.flags = flags;
    return true;
}

export [[nodiscard]] inline auto ToggleWindowFlags(Window& window, window_flags flags) noexcept -> std::int32_t
{
    return SetWindowFlags(window, window.flags ^ flags);
}

export [[nodiscard]] inline auto SetWindowPosition(const Window& window, const int xPosition = SDL_WINDOWPOS_CENTERED, const int yPosition = SDL_WINDOWPOS_CENTERED) noexcept
    -> std::int32_t
{
    if (window.pWindow == nullptr) [[unlikely]]
    {
        return false;
    }
    return SDL_SetWindowPosition(window.pWindow, xPosition, yPosition);
}

export [[nodiscard]] inline auto GetWindowPosition(const Window& window, int& xPosition, int& yPosition) noexcept -> std::int32_t
{
    if (window.pWindow == nullptr) [[unlikely]]
    {
        return false;
    }
    return SDL_GetWindowPosition(window.pWindow, &xPosition, &yPosition);
}

export [[nodiscard]] inline auto SetWindowSize(Window& window, const int width, const int height) noexcept -> std::int32_t
{
    if (window.pWindow == nullptr || width <= 0 || height <= 0) [[unlikely]]
    {
        return false;
    }
    if (!SDL_SetWindowSize(window.pWindow, width, height))
    {
        return false;
    }
    SDL_SetWindowPosition(window.pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    window.width  = width;
    window.height = height;
    return true;
}

export [[nodiscard]] inline auto GetWindowSize(Window& window) noexcept -> std::int32_t
{
    if (window.pWindow == nullptr) [[unlikely]]
    {
        return false;
    }
    return SDL_GetWindowSize(window.pWindow, &window.width, &window.height);
}

export [[nodiscard]] constexpr auto GetWindowSize(const Window& window, int& width, int& height) noexcept -> std::int32_t
{
    width  = window.width;
    height = window.height;
    return true;
}

export template <std::size_t N = std::dynamic_extent>
void PollEvents(std::span<const std::pair<event_type, std::function<void(const EventContext&)>>, N> handlers) noexcept
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        const auto mapped = static_cast<event_type>(event.type);
        for (const auto& [type, handler] : handlers)
        {
            if (type == mapped)
            {
                handler(EventContext{mapped, event});
            }
        }
    }
}
} // namespace fawn_vision
