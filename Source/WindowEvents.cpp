//
// Created by Joran on 21/02/2024.
//

#include "WindowEvents.hpp"

#include "../../../../3rdParty/SDL/src/video/SDL_sysvideo.h"
#include "ObjectDefinitions.hpp"

#include <SDL_events.h>

namespace FawnVision
{

    void RegisterWindowEventCallback( const window_event event, const std::function<void()>& function )
    {
        g_windowEventManager.RegisterCallback( event, function );
    }
    void PollWindowEvent( const Window& window )
    {
        g_windowEventManager.HandleEvents( window );
    }
    void WaitForWindowEvent( const Window& window, const window_event event )
    {
        while ( true )
        {
            SDL_Event sdlEvent;
            if ( SDL_WaitEvent( &sdlEvent ) )
            {
                if ( sdlEvent.window.windowID != window->pWindow->id )
                    continue;
                switch ( sdlEvent.window.type )
                {
                    case SDL_EVENT_WINDOW_SHOWN:           /**< Window has been shown */
                    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: /**< The window manager requests that the window be closed */
                        if ( event == window_event::close )
                            return;
                    case SDL_EVENT_WINDOW_HIDDEN: /**< Window has been hidden */
                        if ( event == window_event::hidden )
                            return;
                    case SDL_EVENT_WINDOW_EXPOSED: /**< Window has been exposed and should be redrawn */
                        if ( event == window_event::exposed )
                            return;
                    case SDL_EVENT_WINDOW_MOVED: /**< Window has been moved to data1, data2 */
                        if ( event == window_event::moved )
                            return;
                    case SDL_EVENT_WINDOW_RESIZED: /**< Window has been resized to data1xdata2 */
                        if ( event == window_event::resize )
                            return;
                    case SDL_EVENT_WINDOW_MINIMIZED: /**< Window has been minimized */
                        if ( event == window_event::resize || event == window_event::minimize )
                            return;
                    case SDL_EVENT_WINDOW_MAXIMIZED: /**< Window has been maximized */
                        if ( event == window_event::resize || event == window_event::maximize )
                            return;
                    case SDL_EVENT_WINDOW_RESTORED: /**< Window has been restored to normal size and position */
                        if ( event == window_event::resize || event == window_event::exposed )
                        case SDL_EVENT_WINDOW_FOCUS_GAINED: /**< Window has gained keyboard focus */
                        case SDL_EVENT_WINDOW_TAKE_FOCUS:   /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
                            if ( event == window_event::focus_gained )
                                return;
                    case SDL_EVENT_WINDOW_FOCUS_LOST: /**< Window has lost keyboard focus */
                        if ( event == window_event::focus_lost )
                            return;
                    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: /**< The window has entered fullscreen mode */
                        if ( event == window_event::resize || event == window_event::fullscreen_enter )
                            return;
                    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: /**< The window has left fullscreen mode */
                        if ( event == window_event::resize || event == window_event::fullscreen_leave )
                            return;
                    default: break;
                }
            }
        }
    }

    void WindowEventManager::HandleEvents( const Window& window ) const
    {
        SDL_Event event;
        while ( SDL_PollEvent( &event ) )
        {
            if ( event.window.windowID != window->pWindow->id )
                continue;
            switch ( event.window.type )
            {
                case SDL_EVENT_WINDOW_SHOWN:           /**< Window has been shown */
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED: /**< The window manager requests that the window be closed */
                    if ( m_subbedFunctions.contains( window_event::close ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::close ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_HIDDEN: /**< Window has been hidden */
                    if ( m_subbedFunctions.contains( window_event::hidden ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::hidden ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_EXPOSED: /**< Window has been exposed and should be redrawn */
                    if ( m_subbedFunctions.contains( window_event::exposed ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::exposed ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_MOVED: /**< Window has been moved to data1, data2 */
                    if ( m_subbedFunctions.contains( window_event::moved ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::moved ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;

                case SDL_EVENT_WINDOW_RESIZED: /**< Window has been resized to data1xdata2 */
                    if ( m_subbedFunctions.contains( window_event::resize ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::resize ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_MINIMIZED: /**< Window has been minimized */
                    if ( m_subbedFunctions.contains( window_event::resize ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::resize ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    if ( m_subbedFunctions.contains( window_event::minimize ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::minimize ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_MAXIMIZED: /**< Window has been maximized */
                    if ( m_subbedFunctions.contains( window_event::resize ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::resize ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    if ( m_subbedFunctions.contains( window_event::maximize ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::maximize ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_RESTORED: /**< Window has been restored to normal size and position */
                    if ( m_subbedFunctions.contains( window_event::resize ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::resize ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    if ( m_subbedFunctions.contains( window_event::exposed ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::exposed ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED: /**< Window has gained keyboard focus */
                case SDL_EVENT_WINDOW_TAKE_FOCUS:   /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
                    if ( m_subbedFunctions.contains( window_event::focus_gained ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::focus_gained ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST: /**< Window has lost keyboard focus */
                    if ( m_subbedFunctions.contains( window_event::focus_lost ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::focus_lost ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: /**< The window has entered fullscreen mode */
                    if ( m_subbedFunctions.contains( window_event::resize ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::resize ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    if ( m_subbedFunctions.contains( window_event::fullscreen_enter ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::fullscreen_enter ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: /**< The window has left fullscreen mode */
                    if ( m_subbedFunctions.contains( window_event::resize ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::resize ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    if ( m_subbedFunctions.contains( window_event::fullscreen_leave ) )
                    {
                        const std::vector<std::function<void()>>& functions { m_subbedFunctions.at( window_event::fullscreen_leave ) };
                        std::ranges::for_each( functions, []( const std::function<void()>& func )
                                               {
                                                   func();
                                               } );
                    }
                    break;
                default: break;
            }
        }
    }

    void WindowEventManager::RegisterCallback( const window_event event, const std::function<void()>& function )
    {
        m_subbedFunctions[ event ].push_back( function );
    }
}// namespace FawnVision
