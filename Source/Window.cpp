//
// Created by joran on 30/12/2023.
//

#include "Window.h"
#include <SDL3/SDL.h>

namespace FawnVision
{
    error_code Window::Initialize() noexcept
    {
        if (m_pWindow)
        {
            return error_code::already_initialized;
        }
        if (!SDL_Init(SDL_INIT_EVERYTHING))
        {
            return error_code::not_initializable;
        }
        int screenCount{};
        const SDL_DisplayID* displayList{SDL_GetDisplays(&screenCount)};
        const SDL_DisplayMode* current{nullptr};
        for (const SDL_DisplayID* displayId{displayList}; displayId != nullptr; ++displayId)
        {
            current = SDL_GetDesktopDisplayMode(*displayId);
            if (current)
            {
                break;
            }
        }
        if (!current)
        {
            return error_code::no_display_found;
        }

        m_width = current->w / 2;
        m_height = current->h / 2;
        m_windowFlags = SDL_WINDOW_VULKAN;
        // todo : add support for DX12
        // todo : load/override with values in ini
        // todo : add project name to the title so that only the project title remanes when no editor

        // todo : set size to banner size
        m_pWindow = SDL_CreateWindow("Balbino Engine", 360, 640, SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS);
        if (!m_pWindow)
        {
            return error_code::window_not_created;
        }
        return error_code::oke;
    }

    error_code Window::Cleanup() noexcept
    {
        if (!m_pWindow)
        {
            return error_code::not_initialized;
        }
        SDL_DestroyWindow(m_pWindow);
        m_pWindow = nullptr;
        return error_code::oke;
    }

    error_code Window::SetFlags(const uint32_t flag) noexcept
    {
        if (!m_pWindow)
        {
            return error_code::not_initialized;
        }
        return ToggleFlags(~(m_windowFlags & flag));
    }

    error_code Window::ToggleFlags(const uint32_t flag) noexcept
    {
        if (!m_pWindow)
        {
            return error_code::not_initialized;
        }
        if (flag & SDL_WINDOW_FULLSCREEN)
        {
            if (m_windowFlags & SDL_WINDOW_FULLSCREEN)
            {
                if (!SDL_SetWindowFullscreen(m_pWindow, SDL_FALSE)) return error_code::bad_flag;
            }
            else
            {
                if (!SDL_SetWindowFullscreen(m_pWindow, SDL_TRUE)) return error_code::bad_flag;
            }
        }

        if (flag & SDL_WINDOW_BORDERLESS)
        {
            if (m_windowFlags & SDL_WINDOW_BORDERLESS)
            {
                if (!SDL_SetWindowBordered(m_pWindow, SDL_TRUE)) return error_code::bad_flag;
            }
            else
            {
                if (!SDL_SetWindowBordered(m_pWindow, SDL_FALSE)) return error_code::bad_flag;
            }
        }

        if (flag & SDL_WINDOW_MAXIMIZED && !(m_windowFlags & SDL_WINDOW_MAXIMIZED))
        {
            if (!SDL_MaximizeWindow(m_pWindow)) return error_code::bad_flag;
        }

        if (flag & SDL_WINDOW_MINIMIZED && !(m_windowFlags & SDL_WINDOW_MINIMIZED))
        {
            if (!SDL_MinimizeWindow(m_pWindow)) return error_code::bad_flag;
        }
        m_windowFlags ^= flag;
        return error_code::oke;
    }

    error_code Window::SetSize(const int w, const int h) noexcept
    {
        if (!m_pWindow)
        {
            return error_code::not_initialized;
        }
        m_width = w;
        m_height = h;
        SDL_SetWindowSize(m_pWindow, m_width, m_height);
        SDL_SetWindowPosition(m_pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        return error_code::oke;
    }

    error_code Window::GetSize(int& w, int& h) const
    {
        if (!m_pWindow)
        {
            return error_code::not_initialized;
        }
        w = m_width;
        h = m_height;
        return error_code::oke;
    }
} // FawnVision
