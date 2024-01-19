//
// Created by joran on 30/12/2023.
//

#include <SDL3/SDL.h>

#include "FawnVision_Core.h"
#include "ObjectDefenitions.h"

namespace FawnVision
{
    error_code Initialize()
    {
        if ( SDL_Init( SDL_INIT_EVERYTHING ) )
        {
            return error_code::not_initializable;
        }
        return error_code::oke;
    }

    error_code CreateWindow( const WindowCreateInfo& createInfo, Window& window ) noexcept
    {
        if ( window )
        {
            return error_code::already_initialized;
        }

        int screenCount {};
        const SDL_DisplayID* displayList { SDL_GetDisplays( &screenCount ) };
        const SDL_DisplayMode* current { nullptr };
        if ( createInfo.screen == -1 )
        {
            for ( const SDL_DisplayID* displayId { displayList }; displayId != nullptr; ++displayId )
            {
                current = SDL_GetDesktopDisplayMode( *displayId );
                if ( current )
                {
                    break;
                }
            }
        }
        else
        {
            if ( createInfo.screen >= screenCount )
            {
                return error_code::invalid_parameter;
            }

            current = SDL_GetDesktopDisplayMode( createInfo.screen );
        }
        if ( !current )
        {
            return error_code::no_display_found;
        }
        if ( createInfo.width > current->w || createInfo.height > current->h )
        {
            return error_code::invalid_parameter;
        }

        SDL_Window* pWindow { SDL_CreateWindow( createInfo.name, createInfo.width, createInfo.height, createInfo.flags ) };
        if ( !pWindow )
        {
            return error_code::window_not_created;
        }
        window = new ( std::nothrow ) Window_T { pWindow, createInfo.flags, createInfo.width, createInfo.height };
        if ( !window )
        {
            return error_code::out_of_memory;
        }
        window->extensions = SDL_Vulkan_GetInstanceExtensions( &window->extensionCount );
        if ( !window->extensions )
        {
            return error_code::sdl_error;
        }
        return error_code::oke;
    }

    error_code SetWindowFlags( const Window& window, const uint32_t flags ) noexcept
    {
        if ( !window )
        {
            return error_code::not_initialized;
        }

        if ( SDL_SetWindowBordered( window->pWindow, flags & SDL_WINDOW_BORDERLESS ? SDL_FALSE : SDL_TRUE ) || SDL_SetWindowResizable( window->pWindow, flags & SDL_WINDOW_RESIZABLE ? SDL_TRUE : SDL_FALSE )
             || SDL_SetWindowFullscreen( window->pWindow, flags & SDL_WINDOW_FULLSCREEN ? SDL_TRUE : SDL_FALSE ) || SDL_SetWindowFocusable( window->pWindow, flags & SDL_WINDOW_NOT_FOCUSABLE ? SDL_FALSE : SDL_TRUE ) )
        {
            return error_code::sdl_error;
        }
        if ( flags & SDL_WINDOW_MAXIMIZED )
        {
            if ( SDL_MaximizeWindow( window->pWindow ) )
            {
                return error_code::sdl_error;
            }
        }
        if ( flags & SDL_WINDOW_MINIMIZED )
        {
            if ( SDL_MinimizeWindow( window->pWindow ) )
            {
                return error_code::sdl_error;
            }
        }
        window->flags = flags;
        return error_code::oke;
    }

    error_code ToggleWindowFlags( const Window& window, const uint32_t flags ) noexcept
    {
        return SetWindowFlags( window, window->flags ^ flags );
    }

    error_code SetWindowSize( const Window& window, const int w, const int h ) noexcept
    {
        if ( !window )
        {
            return error_code::not_initialized;
        }
        window->width  = w;
        window->height = h;
        SDL_SetWindowSize( window->pWindow, w, h );
        SDL_SetWindowPosition( window->pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
        return error_code::oke;
    }

    error_code GetWindowSize( const Window& window, int& w, int& h ) noexcept
    {
        if ( !window )
        {
            return error_code::not_initialized;
        }
        w = window->width;
        h = window->height;
        return error_code::oke;
    }

    error_code SetWindowPosition( const Window& window, const int x, const int y ) noexcept
    {
        if ( !window )
        {
            return error_code::not_initialized;
        }
        SDL_SetWindowPosition( window->pWindow, x, y );
        return error_code::oke;
    }

    error_code GetWindowPosition( const Window& window, int& x, int& y ) noexcept
    {
        if ( !window )
        {
            return error_code::not_initialized;
        }
        SDL_GetWindowPosition( window->pWindow, &x, &y );
        return error_code::oke;
    }

    error_code ReleaseWindow( Window& window ) noexcept
    {
        if ( !window )
        {
            return error_code::not_initialized;
        }
        SDL_DestroyWindow( window->pWindow );
        delete window;
        window = BALBINO_NULL;
        return error_code::oke;
    }

    const char* GetWindowError()
    {
        return SDL_GetError();
    }

    void ProcessCommonEvents( bool& loop, const SDL_Event& event )
    {
        switch ( event.type )
        {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_TERMINATING:
            {
                loop = false;
                break;
            }
            case SDL_EVENT_LOW_MEMORY:
            case SDL_EVENT_WILL_ENTER_BACKGROUND:
            case SDL_EVENT_DID_ENTER_BACKGROUND:
            case SDL_EVENT_WILL_ENTER_FOREGROUND:
            case SDL_EVENT_DID_ENTER_FOREGROUND:
            case SDL_EVENT_LOCALE_CHANGED:
            case SDL_EVENT_SYSTEM_THEME_CHANGED:// todo : check if ui theme needs to be changed (callbacks)
            default: break;
        }
    }

    void ProcessWindowEvents( const Window& window, bool& loop, const SDL_Event& event )
    {
        switch ( event.type )
        {
            case SDL_EVENT_WINDOW_HIDDEN:
            case SDL_EVENT_WINDOW_MINIMIZED:
            {
                SDL_Event minEvent;
                bool isMin { true };
                while ( isMin && loop )
                {
                    if ( SDL_WaitEvent( &minEvent ) )
                    {
                        switch ( minEvent.type )
                        {
                            case SDL_EVENT_WINDOW_EXPOSED:
                            case SDL_EVENT_WINDOW_HIDDEN:
                            case SDL_EVENT_WINDOW_MAXIMIZED:
                            case SDL_EVENT_WINDOW_MINIMIZED:
                            case SDL_EVENT_WINDOW_RESTORED:
                            case SDL_EVENT_WINDOW_SHOWN:
                            {
                                isMin = false;
                                break;
                            }
                            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                            {
                                loop = false;
                                break;
                            }
                            default: break;
                        }
                    }
                }
                break;
            }
            case SDL_EVENT_WINDOW_EXPOSED:
            case SDL_EVENT_WINDOW_MAXIMIZED:
            case SDL_EVENT_WINDOW_RESTORED:
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_SHOWN:
                SDL_GetWindowSize( window->pWindow, &window->width, &window->height );
                window->flags = SDL_GetWindowFlags( window->pWindow );
                // todo : call all resize callbacks;
                break;
            case SDL_EVENT_WINDOW_MOVED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            default: break;
        }
    }

    void ProcessEvents( const Window& window, bool& loop ) noexcept
    {
        SDL_Event event;
        while ( SDL_PollEvent( &event ) )
        {
            if ( event.type > SDL_EVENT_FIRST && event.type < SDL_EVENT_DISPLAY_FIRST )
            {
                ProcessCommonEvents( loop, event );
            }
            else if ( event.type > SDL_EVENT_WINDOW_FIRST && event.type < SDL_EVENT_WINDOW_LAST )
            {
                ProcessWindowEvents( window, loop, event );
            }
            else if ( event.type > SDL_EVENT_WINDOW_LAST && event.type <= SDL_EVENT_FINGER_MOTION )
            {
                return;// todo : process input callbacks
            }
        }
    }
}// namespace FawnVision
