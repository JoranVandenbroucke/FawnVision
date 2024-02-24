//
// Created by Joran on 21/02/2024.
//

#pragma once
#include "FawnVision_Core.hpp"
#include "FawnVision_WindowEvents.hpp"

#include <unordered_map>

namespace FawnVision
{
    class WindowEventManager
    {
      public:
        void HandleEvents( const Window& window ) const;
        void RegisterCallback( window_event event, const std::function<void()>& function );

      private:
        std::unordered_map<window_event, std::vector<std::function<void()>>> m_subbedFunctions;
    };
    inline WindowEventManager g_windowEventManager{};
}// namespace FawnVision
