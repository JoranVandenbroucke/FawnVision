//
// Created by joran on 30/12/2023.
//

#include "FawnVision_Window.hpp"

#include "Error.hpp"
#include "ObjectDefinitions.hpp"

#include <SDL3/SDL.h>

namespace FawnVision
{
    int32_t Initialize()
    {
        if ( SDL_Init( SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR | SDL_INIT_AUDIO ) != 0 )
        {
            SetError( error_code::generic, "SDL initialization failed: %s", SDL_GetError() );
            return -1;
        }
        return 0;
    }

    int32_t CreateWindow( const WindowCreateInfo& createInfo, Window& window ) noexcept
    {
        if ( window != nullptr )
        {
            SetError( error_code::generic, "Window is already created" );
            return -1;
        }

        int screenCount {};
        const SDL_DisplayID* displayList { SDL_GetDisplays( &screenCount ) };
        const SDL_DisplayMode* current { nullptr };
        if ( createInfo.screen == -1 )
        {
            for ( const SDL_DisplayID* displayId { displayList }; displayId != nullptr; ++displayId )
            {
                current = SDL_GetDesktopDisplayMode( *displayId );
                if ( current != nullptr )
                {
                    break;
                }
            }
        }
        else
        {
            if ( createInfo.screen >= screenCount )
            {
                SetError( error_code::generic, "Invalid screen requested. Available screens: %d Requested screen: %d", screenCount, createInfo.screen );
                return -1;
            }

            current = SDL_GetDesktopDisplayMode( createInfo.screen );
        }
        if ( current == nullptr )
        {
            SetError( error_code::generic, "Window is not initialized" );
            return -1;
        }
        if ( createInfo.width > current->w || createInfo.height > current->h )
        {
            SetError( error_code::generic, "Requested window size (%d x %d) is larger than the available screen size (%d x %d)", createInfo.width, createInfo.height, current->w, current->h );
            return -1;
        }

        SDL_Window* pWindow { SDL_CreateWindow( &createInfo.name[0], createInfo.width, createInfo.height, createInfo.flags ) };
        if ( pWindow == nullptr )
        {
            SetError( error_code::generic, "SDL window creation failed: %s", SDL_GetError() );
            return -1;
        }

        window = new ( std::nothrow ) Window_T { pWindow, createInfo.flags, createInfo.width, createInfo.height };
        if ( window == nullptr )
        {
            SetError( error_code::out_of_memory, "Failed to allocate memory for the window" );
            return -1;
        }

        window->extensions = SDL_Vulkan_GetInstanceExtensions( &window->extensionCount );
        if ( window->extensions == nullptr )
        {
            SetError( error_code::generic, "SDL Vulkan extension retrieval failed: %s", SDL_GetError() );
            return -1;
        }

        return 0;
    }

    int32_t SetWindowFlags( const Window& window, const uint32_t flags ) noexcept
    {
        if ( window == nullptr )
        {
            SetError( error_code::generic, "Window is not initialized" );
            return -1;
        }

        if ( SDL_SetWindowBordered( window->pWindow, (flags & SDL_WINDOW_BORDERLESS) != 0U ? SDL_FALSE : SDL_TRUE ) != 0
             || SDL_SetWindowResizable( window->pWindow, (flags & SDL_WINDOW_RESIZABLE) != 0U ? SDL_TRUE : SDL_FALSE ) != 0
             || SDL_SetWindowFullscreen( window->pWindow, (flags & SDL_WINDOW_FULLSCREEN) != 0U ? SDL_TRUE : SDL_FALSE ) != 0
             || SDL_SetWindowFocusable( window->pWindow, (flags & SDL_WINDOW_NOT_FOCUSABLE) != 0U ? SDL_FALSE : SDL_TRUE ) != 0 )
        {
            SetError( error_code::generic, "Failed to set window flags: %s", SDL_GetError() );
            return -1;
        }

        if ( ( flags & SDL_WINDOW_MAXIMIZED ) != 0U )
        {
            if ( SDL_MaximizeWindow( window->pWindow ) != 0 )
            {
                SetError( error_code::generic, "Failed to maximize window: %s", SDL_GetError() );
                return -1;
            }
        }

        if ( ( flags & SDL_WINDOW_MINIMIZED ) != 0U )
        {
            if ( SDL_MinimizeWindow( window->pWindow ) != 0 )
            {
                SetError( error_code::generic, "Failed to minimize window: %s", SDL_GetError() );
                return -1;
            }
        }

        window->flags = flags;
        return 0;
    }

    int32_t ToggleWindowFlags( const Window& window, const uint32_t flags ) noexcept
    {
        return SetWindowFlags( window, window->flags ^ flags );
    }

    int32_t SetWindowSize( const Window& window, const int width, const int height ) noexcept
    {
        if ( window == nullptr )
        {
            SetError( error_code::generic, "Window is not initialized" );
            return -1;
        }

        window->width  = width;
        window->height = height;
        SDL_SetWindowSize( window->pWindow, width, height );
        SDL_SetWindowPosition( window->pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
        return 0;
    }

    int32_t GetWindowSize( const Window& window, int& w, int& h ) noexcept
    {
        if ( window == nullptr )
        {
            SetError( error_code::generic, "Window is not initialized" );
            return -1;
        }

        w = window->width;
        h = window->height;
        return 0;
    }

    int32_t SetWindowPosition( const Window& window, const int x, const int y ) noexcept
    {
        if ( window == nullptr )
        {
            SetError( error_code::generic, "Window is not initialized" );
            return -1;
        }

        SDL_SetWindowPosition( window->pWindow, x, y );
        return 0;
    }

    int32_t GetWindowPosition( const Window& window, int& xPosition, int& yPosition ) noexcept
    {
        if ( window == nullptr )
        {
            SetError( error_code::generic, "Window is not initialized" );
            return -1;
        }

        SDL_GetWindowPosition( window->pWindow, &xPosition, &yPosition );
        return 0;
    }

    int32_t ReleaseWindow( Window& window ) noexcept
    {
        if ( window == nullptr )
        {
            SetError( error_code::generic, "Window is not initialized" );
            return -1;
        }

        SDL_DestroyWindow( window->pWindow );
        delete window;
        window = nullptr;
        return 0;
    }

    const char* GetWindowError()
    {
        return SDL_GetError();
    }
}// namespace FawnVision
