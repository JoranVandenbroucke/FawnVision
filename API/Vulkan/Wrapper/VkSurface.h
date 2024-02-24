//
// Created by Joran on 08/01/2024.
//

#pragma once
#include <SDL_vulkan.h>
#include "../DeerVulkan_Core.h"

namespace DeerVulkan
{
    class CVkSurface
    {
    public:
        void Initialize( const VkSurfaceKHR surface ) noexcept
        {
            m_surface = surface;
        };
        [[nodiscard]] VkSurfaceKHR Handle() const noexcept
        {
            return m_surface;
        }
    private:
        VkSurfaceKHR m_surface { VK_NULL_HANDLE };
    };
}// namespace DeerVulkan
