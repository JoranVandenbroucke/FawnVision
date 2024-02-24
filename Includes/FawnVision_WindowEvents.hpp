//
// Created by Joran on 21/02/2024.
//

#pragma once
#include <functional>

namespace FawnVision
{
    enum class window_event : uint16_t
    {
        exposed          = 1 << 0,
        fullscreen_enter = 1 << 1,
        fullscreen_leave = 1 << 2,
        hidden           = 1 << 3,
        maximize         = 1 << 4,
        minimize         = 1 << 5,
        moved            = 1 << 6,
        resize           = 1 << 7,
        close            = 1 << 8,
        focus_gained     = 1 << 9,
        focus_lost       = 1 << 10,
    };
ENABLE_ENUM_BITWISE_OPERATORS( FawnVision::window_event )

    void RegisterWindowEventCallback( window_event event, const std::function<void()>& function );
    void PollWindowEvent( const Window& window );
    void WaitForWindowEvent( const Window& window, window_event event );
}// namespace FawnVision

