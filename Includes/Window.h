//
// Created by joran on 30/12/2023.
//

#pragma once
#include "FawnVision_Core.h"

struct SDL_Window;

namespace FawnVision
{
    class Window
    {
    public:
        Window() = default;

        error_code Initialize() noexcept;
        error_code Cleanup() noexcept;
        error_code SetFlags(uint32_t flag) noexcept;
        error_code ToggleFlags(uint32_t flag) noexcept;
        error_code SetSize(int w, int h)noexcept;
        error_code GetSize(int& w, int& h) const;

    private:
        int32_t m_width{};
        int32_t m_height{};
        uint32_t m_windowFlags{};
        SDL_Window* m_pWindow{nullptr};
    };
} // FawnVision
