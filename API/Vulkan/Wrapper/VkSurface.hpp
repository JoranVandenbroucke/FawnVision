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
    BALBINO_CONSTEXPR_SINCE_CXX20 BALBINO_EXPLICIT_SINCE_CXX11 CVkSurface(const VkSurfaceKHR& pSurface);

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Handle() const BALBINO_NOEXCEPT_SINCE_CXX11->VkSurfaceKHR;

  private:
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
};
BALBINO_CONSTEXPR_SINCE_CXX20 CVkSurface::CVkSurface(const VkSurfaceKHR& pSurface)
    : m_surface{pSurface}
{
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkSurface::Handle() const BALBINO_NOEXCEPT_SINCE_CXX11 -> VkSurfaceKHR
{
    return m_surface;
}
} // namespace DeerVulkan