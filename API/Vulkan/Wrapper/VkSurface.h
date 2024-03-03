//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include <SDL3/SDL_vulkan.h>

#include "../DeerVulkan_Core.h"

namespace DeerVulkan
{
class CVkSurface
{
public:
    void Initialize(const VkSurfaceKHR& pSurface) noexcept
    {
        m_surface = pSurface;
    }

    [[nodiscard]] auto Handle() const noexcept -> VkSurfaceKHR
    {
        return m_surface;
    }

private:
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
};
} // namespace DeerVulkan