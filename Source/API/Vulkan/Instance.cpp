//
// Copyright (c) 2024.
// Author: Joran
//

#include "Instance.hpp"

#include <SDL3/SDL_vulkan.h>

namespace DeerVulkan
{
auto CInstance::Initialize(SDL_Window* pWindow, const char* pAppTitle, const uint32_t appVersion, const char* const* pExtensions, const uint32_t extensionsCount) noexcept -> int
{
    if (m_instance.Initialize(pAppTitle, appVersion, pExtensions, extensionsCount) != 0)
    {
        return -1;
    }
    VkSurfaceKHR pSurface{VK_NULL_HANDLE};
    if (SDL_Vulkan_CreateSurface(pWindow, m_instance.Handle(), VK_NULL_HANDLE, &pSurface) != SDL_TRUE)
    {
        return -2;
    }
    m_surface.Surface(pSurface);
    m_familyIndex = m_instance.FindBestPhysicalDeviceIndex(m_surface);
    if (!m_familyIndex.IsComplete())
    {
        return -3;
    }
    if (m_instance.CreateDevice(m_device, m_familyIndex) != 0)
    {
        return -4;
    }
    return 0;
}

auto CInstance::Cleanup() noexcept -> int
{
    m_device.Cleanup();
    m_instance.FreeSurface(m_surface);
    m_surface.Surface(VK_NULL_HANDLE);
}
} // namespace DeerVulkan