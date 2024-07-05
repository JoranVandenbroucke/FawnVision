//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"

namespace DeerVulkan
{
class CVkSurface
{
  public:
    constexpr CVkSurface() = default;

    constexpr void Surface(const VkSurfaceKHR& surface) noexcept
    {
        m_surface = surface;
    }
    [[nodiscard]] constexpr auto Surface() const noexcept -> VkSurfaceKHR
    {
        return m_surface;
    }

  private:
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
};
} // namespace DeerVulkan