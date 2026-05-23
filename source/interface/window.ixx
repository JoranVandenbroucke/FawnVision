//
// Copyright (c) 2026.
// Author: Joran
//

module;
#include <SDL3/SDL.h>
#include <SDL3/SDL_filesystem.h>
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

export struct WindowSettings
{
    std::int32_t width{};
    std::int32_t height{};
    std::int32_t x{SDL_WINDOWPOS_CENTERED};
    std::int32_t y{SDL_WINDOWPOS_CENTERED};
    bool fullscreen{};
    bool maximized{};
    bool valid{};
};

export [[nodiscard]] inline auto IsHyprlandSession() noexcept -> bool
{
    return std::getenv("HYPRLAND_INSTANCE_SIGNATURE") != nullptr;
}

namespace detail
{
constexpr const char* appId{"balbino"};
constexpr const char* prefOrganization{"Balbino"};
constexpr const char* prefApplication{"Deer"};
constexpr const char* windowSettingsFileName{"window.cfg"};

[[nodiscard]] inline auto IsWaylandSession() noexcept -> bool
{
    return std::getenv("WAYLAND_DISPLAY") != nullptr;
}

inline auto ConfigurePlatformVideoHints() noexcept -> void
{
    static_cast<void>(SDL_SetHint(SDL_HINT_APP_ID, appId));
    static_cast<void>(SDL_SetHint(SDL_HINT_APP_NAME, "Balbino Engine"));

    if (IsHyprlandSession())
    {
        static_cast<void>(SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_PREFER_LIBDECOR, "0"));
    }
}

[[nodiscard]] inline auto GetWindowSettingsPath() noexcept -> std::optional<std::string>
{
    char* const prefPath{SDL_GetPrefPath(prefOrganization, prefApplication)};
    if (prefPath == nullptr) [[unlikely]]
    {
        return std::nullopt;
    }

    std::string settingsPath{prefPath};
    settingsPath += windowSettingsFileName;
    SDL_free(prefPath);
    return settingsPath;
}

[[nodiscard]] inline auto ParseBool(std::string_view value, bool& output) noexcept -> bool
{
    if (value == "1" || value == "true")
    {
        output = true;
        return true;
    }
    if (value == "0" || value == "false")
    {
        output = false;
        return true;
    }
    return false;
}

[[nodiscard]] inline auto ParseInt(std::string_view value, std::int32_t& output) noexcept -> bool
{
    std::int32_t parsed{};
    const auto [pointer, errorCode]{std::from_chars(value.data(), value.data() + value.size(), parsed)};
    if (errorCode != std::errc{} || pointer != value.data() + value.size()) [[unlikely]]
    {
        return false;
    }

    output = parsed;
    return true;
}
} // namespace detail

export struct Window
{
    SDL_Window* pWindow{nullptr};
    std::int32_t width{};
    std::int32_t height{};
    window_flags flags{window_flags::none};
};

[[nodiscard]] inline auto ResolveDisplayId(const std::uint8_t screen) noexcept -> SDL_DisplayID
{
    if (screen == 0)
    {
        if (const SDL_DisplayID primaryDisplay{SDL_GetPrimaryDisplay()}; primaryDisplay != 0)
        {
            return primaryDisplay;
        }
    }

    int displayCount{};
    const SDL_DisplayID* const pDisplays{SDL_GetDisplays(&displayCount)};
    if (pDisplays == nullptr || displayCount <= 0) [[unlikely]]
    {
        return 0;
    }

    if (screen >= static_cast<std::uint8_t>(displayCount)) [[unlikely]]
    {
        return 0;
    }

    return pDisplays[screen];
}

export [[nodiscard]] inline auto SyncWindowSize(Window& window) noexcept -> bool
{
    if (window.pWindow == nullptr) [[unlikely]]
    {
        return false;
    }

    if (!SDL_GetWindowSize(window.pWindow, &window.width, &window.height)) [[unlikely]]
    {
        return false;
    }

    return window.width > 0 && window.height > 0;
}

export [[nodiscard]] inline auto InitializeSDL() noexcept -> bool
{
    detail::ConfigurePlatformVideoHints();
    return SDL_Init(SDL_INIT_VIDEO);
}

export [[nodiscard]] inline auto LoadWindowSettings() noexcept -> WindowSettings
{
    WindowSettings settings{};

    const std::optional settingsPath{detail::GetWindowSettingsPath()};
    if (!settingsPath.has_value()) [[unlikely]]
    {
        return settings;
    }

    std::ifstream input{settingsPath.value()};
    if (!input.is_open()) [[unlikely]]
    {
        return settings;
    }

    std::string line{};
    while (std::getline(input, line))
    {
        const std::size_t separator{line.find('=')};
        if (separator == std::string::npos) [[unlikely]]
        {
            continue;
        }

        const std::string_view key{line.data(), separator};
        const std::string_view value{line.data() + separator + 1, line.size() - separator - 1};

        if (key == "width")
        {
            static_cast<void>(detail::ParseInt(value, settings.width));
        }
        else if (key == "height")
        {
            static_cast<void>(detail::ParseInt(value, settings.height));
        }
        else if (key == "x")
        {
            static_cast<void>(detail::ParseInt(value, settings.x));
        }
        else if (key == "y")
        {
            static_cast<void>(detail::ParseInt(value, settings.y));
        }
        else if (key == "fullscreen")
        {
            static_cast<void>(detail::ParseBool(value, settings.fullscreen));
        }
        else if (key == "maximized")
        {
            static_cast<void>(detail::ParseBool(value, settings.maximized));
        }
        else if (key == "valid")
        {
            static_cast<void>(detail::ParseBool(value, settings.valid));
        }
    }

    return settings;
}

export [[nodiscard]] inline auto SaveWindowSettings(const Window& window) noexcept -> bool
{
    if (window.pWindow == nullptr) [[unlikely]]
    {
        return false;
    }

    const std::optional settingsPath{detail::GetWindowSettingsPath()};
    if (!settingsPath.has_value()) [[unlikely]]
    {
        return false;
    }

    WindowSettings settings{
        .valid = true,
    };

    if (!SDL_GetWindowSize(window.pWindow, &settings.width, &settings.height)) [[unlikely]]
    {
        return false;
    }
    if (!SDL_GetWindowPosition(window.pWindow, &settings.x, &settings.y)) [[unlikely]]
    {
        return false;
    }

    const SDL_WindowFlags sdlFlags{SDL_GetWindowFlags(window.pWindow)};
    settings.fullscreen = (sdlFlags & SDL_WINDOW_FULLSCREEN) != 0U;
    settings.maximized  = (sdlFlags & SDL_WINDOW_MAXIMIZED) != 0U;

    std::ofstream output{settingsPath.value(), std::ios::trunc};
    if (!output.is_open()) [[unlikely]]
    {
        return false;
    }

    output << "valid=1\n";
    output << "width=" << settings.width << '\n';
    output << "height=" << settings.height << '\n';
    output << "x=" << settings.x << '\n';
    output << "y=" << settings.y << '\n';
    output << "fullscreen=" << (settings.fullscreen ? 1 : 0) << '\n';
    output << "maximized=" << (settings.maximized ? 1 : 0) << '\n';
    return static_cast<bool>(output);
}

export [[nodiscard]] inline auto CreateWindow(const WindowCreateInfo& createInfo, Window& window) noexcept -> bool
{
    if (createInfo.width <= 0 || createInfo.height <= 0) [[unlikely]]
    {
        return false;
    }

    const SDL_DisplayID displayId{ResolveDisplayId(createInfo.screen)};
    if (displayId == 0) [[unlikely]]
    {
        return false;
    }

    SDL_Rect usableBounds{};
    if (!SDL_GetDisplayUsableBounds(displayId, &usableBounds) || usableBounds.w <= 0 || usableBounds.h <= 0) [[unlikely]]
    {
        return false;
    }

    if (createInfo.width > usableBounds.w || createInfo.height > usableBounds.h) [[unlikely]]
    {
        return false;
    }

    const std::int32_t xPosition{usableBounds.x + (usableBounds.w - createInfo.width) / 2};
    const std::int32_t yPosition{usableBounds.y + (usableBounds.h - createInfo.height) / 2};

    SDL_Window* const pWindow{SDL_CreateWindow(std::bit_cast<const char*>(createInfo.name), createInfo.width, createInfo.height,
                                               static_cast<SDL_WindowFlags>(createInfo.flags))};

    if (pWindow == nullptr) [[unlikely]]
    {
        return false;
    }

    if (!detail::IsWaylandSession())
    {
        static_cast<void>(SDL_SetWindowPosition(pWindow, xPosition, yPosition));
    }

    window.pWindow = pWindow;
    window.flags   = createInfo.flags;
    window.width   = createInfo.width;
    window.height  = createInfo.height;

    return SyncWindowSize(window);
}

export inline auto ReleaseWindow(const Window& window) noexcept -> void
{
    if (window.pWindow != nullptr)
    {
        SDL_DestroyWindow(window.pWindow);
    }
}

export [[nodiscard]] inline auto SetWindowFlags(Window& window, const window_flags flags) noexcept -> std::int32_t
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

export [[nodiscard]] inline auto ApplySavedWindowLayout(Window& window, const WindowSettings& settings) noexcept -> bool
{
    if (window.pWindow == nullptr) [[unlikely]]
    {
        return false;
    }

    window_flags targetFlags{window.flags | window_flags::resizable};
    if (HasFlag(targetFlags, window_flags::borderless))
    {
        targetFlags = targetFlags ^ window_flags::borderless;
    }

    if (settings.valid && settings.fullscreen)
    {
        targetFlags = targetFlags | window_flags::fullscreen;
        return SetWindowFlags(window, targetFlags) != 0;
    }

    if (settings.valid && settings.maximized)
    {
        targetFlags = targetFlags | window_flags::maximized;
        return SetWindowFlags(window, targetFlags) != 0;
    }

    if (settings.valid && settings.width > 0 && settings.height > 0)
    {
        if (!SetWindowFlags(window, targetFlags)) [[unlikely]]
        {
            return false;
        }
        if (!SDL_SetWindowSize(window.pWindow, settings.width, settings.height)) [[unlikely]]
        {
            return false;
        }

        window.width  = settings.width;
        window.height = settings.height;

        if (!detail::IsWaylandSession())
        {
            static_cast<void>(SDL_SetWindowPosition(window.pWindow, settings.x, settings.y));
        }

        return true;
    }

    targetFlags = targetFlags | window_flags::fullscreen;
    return SetWindowFlags(window, targetFlags) != 0;
}

export [[nodiscard]] inline auto ToggleWindowFlags(Window& window, const window_flags flags) noexcept -> std::int32_t
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
