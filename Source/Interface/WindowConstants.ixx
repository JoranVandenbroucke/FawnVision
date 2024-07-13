//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include <type_traits>
#include "compiler.hpp"

export module FawnVision.WindowConstants;

namespace FawnVision
{
export enum class window_flags : uint32_t {
    fullscreen         = 0x00000001U,
    openGL             = 0x00000002U,
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
    mouseCapture       = 0x00004000U,
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

export BALBINO_CONSTEXPR_SINCE_CXX17 window_flags operator|(window_flags lhs, window_flags rhs)
{
    using UnderlyingType = std::underlying_type_t<window_flags>;
    return static_cast<window_flags>(static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs));
}
} // namespace FawnVision
