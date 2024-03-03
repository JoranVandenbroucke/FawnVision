//
// Copyright (c) 2024.
// Author: Joran
//

#include "WindowEvents.hpp"
#include "ObjectDefinitions.hpp"

#include <SDL3/SDL.h>

namespace FawnVision
{
void RegisterWindowEventCallback(const window_event event, const std::function<void()>& function)
{
    g_windowEventManager.RegisterCallback(event, function);
}

void PollWindowEvent(const Window& window)
{
    g_windowEventManager.HandleEvents(window);
}
#pragma todo( "handle window swapping" )
void WaitForWindowEvent(const Window& /*unused*/, const window_event event)
{
    while (true)
    {
        SDL_Event sdlEvent;
        if (SDL_WaitEvent(&sdlEvent) != 0)
        {
            switch (sdlEvent.window.type)
            {
            case SDL_EVENT_WINDOW_SHOWN:           /**< Window has been shown */
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: /**< The window manager requests that the window be closed */
                if ((event & window_event::close) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_HIDDEN: /**< Window has been hidden */
                if ((event & window_event::hidden) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_EXPOSED: /**< Window has been exposed and should be redrawn */
                if ((event & window_event::exposed) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_MOVED: /**< Window has been moved to data1, data2 */
                if ((event & window_event::moved) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_RESIZED: /**< Window has been resized to data1xdata2 */
                if ((event & window_event::resize) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_MINIMIZED: /**< Window has been minimized */
                if ((event & window_event::resize) == window_event::close || (event & window_event::minimize) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_MAXIMIZED: /**< Window has been maximized */
                if ((event & window_event::resize) == window_event::close || (event & window_event::maximize) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_RESTORED: /**< Window has been restored to normal size and position */
                if ((event & window_event::resize) == window_event::close || (event & window_event::exposed) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_FOCUS_GAINED: /**< Window has gained keyboard focus */
            case SDL_EVENT_WINDOW_TAKE_FOCUS:   /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
                if ((event & window_event::focus_gained) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_FOCUS_LOST: /**< Window has lost keyboard focus */
                if ((event & window_event::focus_lost) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: /**< The window has entered fullscreen mode */
                if ((event & window_event::resize) == window_event::close || (event & window_event::fullscreen_enter) == window_event::close)
                {
                    return;
                }
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: /**< The window has left fullscreen mode */
                if ((event & window_event::resize) == window_event::close || (event & window_event::fullscreen_leave) == window_event::close)
                {
                    return;
                }
            default: break;
            }
        }
    }
}

void WindowEventManager::HandleEvents(const Window& /*unused*/) const
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.window.type)
        {
        case SDL_EVENT_WINDOW_SHOWN:           /**< Window has been shown */
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: /**< The window manager requests that the window be closed */
            if (m_subbedFunctions.contains(window_event::close))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::close)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_HIDDEN: /**< Window has been hidden */
            if (m_subbedFunctions.contains(window_event::hidden))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::hidden)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_EXPOSED: /**< Window has been exposed and should be redrawn */
            if (m_subbedFunctions.contains(window_event::exposed))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::exposed)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_MOVED: /**< Window has been moved to data1, data2 */
            if (m_subbedFunctions.contains(window_event::moved))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::moved)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;

        case SDL_EVENT_WINDOW_RESIZED: /**< Window has been resized to data1xdata2 */
            if (m_subbedFunctions.contains(window_event::resize))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::resize)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_MINIMIZED: /**< Window has been minimized */
            if (m_subbedFunctions.contains(window_event::resize))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::resize)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            if (m_subbedFunctions.contains(window_event::minimize))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::minimize)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_MAXIMIZED: /**< Window has been maximized */
            if (m_subbedFunctions.contains(window_event::resize))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::resize)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            if (m_subbedFunctions.contains(window_event::maximize))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::maximize)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_RESTORED: /**< Window has been restored to normal size and position */
            if (m_subbedFunctions.contains(window_event::resize))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::resize)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            if (m_subbedFunctions.contains(window_event::exposed))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::exposed)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED: /**< Window has gained keyboard focus */
        case SDL_EVENT_WINDOW_TAKE_FOCUS:   /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
            if (m_subbedFunctions.contains(window_event::focus_gained))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::focus_gained)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST: /**< Window has lost keyboard focus */
            if (m_subbedFunctions.contains(window_event::focus_lost))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::focus_lost)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: /**< The window has entered fullscreen mode */
            if (m_subbedFunctions.contains(window_event::resize))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::resize)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            if (m_subbedFunctions.contains(window_event::fullscreen_enter))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::fullscreen_enter)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: /**< The window has left fullscreen mode */
            if (m_subbedFunctions.contains(window_event::resize))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::resize)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            if (m_subbedFunctions.contains(window_event::fullscreen_leave))
            {
                const std::vector<std::function<void()>>& functions{m_subbedFunctions.at(window_event::fullscreen_leave)};
                std::ranges::for_each(functions,
                                      [](const std::function<void()>& func)
                                      {
                                          func();
                                      });
            }
            break;
        default: break;
        }
    }
}

void WindowEventManager::RegisterCallback(const window_event event, const std::function<void()>& function)
{
    if ((event & window_event::exposed) == window_event::exposed)
    {
        m_subbedFunctions[window_event::exposed].push_back(function);
    }
    if ((event & window_event::hidden) == window_event::hidden)
    {
        m_subbedFunctions[window_event::hidden].push_back(function);
    }
    if ((event & window_event::fullscreen_enter) == window_event::fullscreen_enter)
    {
        m_subbedFunctions[window_event::fullscreen_enter].push_back(function);
    }
    if ((event & window_event::fullscreen_leave) == window_event::fullscreen_leave)
    {
        m_subbedFunctions[window_event::fullscreen_leave].push_back(function);
    }
    if ((event & window_event::maximize) == window_event::maximize)
    {
        m_subbedFunctions[window_event::maximize].push_back(function);
    }
    if ((event & window_event::minimize) == window_event::minimize)
    {
        m_subbedFunctions[window_event::minimize].push_back(function);
    }
    if ((event & window_event::moved) == window_event::moved)
    {
        m_subbedFunctions[window_event::moved].push_back(function);
    }
    if ((event & window_event::resize) == window_event::resize)
    {
        m_subbedFunctions[window_event::resize].push_back(function);
    }
    if ((event & window_event::close) == window_event::close)
    {
        m_subbedFunctions[window_event::close].push_back(function);
    }
    if ((event & window_event::focus_gained) == window_event::focus_gained)
    {
        m_subbedFunctions[window_event::focus_gained].push_back(function);
    }
    if ((event & window_event::focus_lost) == window_event::focus_lost)
    {
        m_subbedFunctions[window_event::focus_lost].push_back(function);
    }
}
} // namespace FawnVision