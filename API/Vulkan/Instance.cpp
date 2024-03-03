//
// Copyright (c) 2024.
// Author: Joran
//

#include "Instance.h"

#include <SDL3/SDL_vulkan.h>

#include "Presenter.h"
#include "Wrapper/VkDevice.h"
#include "Wrapper/VkInstance.h"
#include "Wrapper/VkSurface.h"

namespace DeerVulkan
{
auto CInstance::Initialize(SDL_Window* pWindow, const char* pAppTitle, const uint32_t appVersion, const char* const* pExtensions, const uint32_t extensionsCount) noexcept -> int
{
    m_pInstance = new CVkInstance{};
    if (m_pInstance->Initialize(pAppTitle, appVersion, pExtensions, extensionsCount) != 0)
    {
        return -1;
    }
    VkSurfaceKHR pSurface{VK_NULL_HANDLE};
    if (SDL_Vulkan_CreateSurface(pWindow, m_pInstance->Handle(), VK_NULL_HANDLE, &pSurface) != SDL_TRUE)
    {
        return -2;
    }
    m_surface = new CVkSurface{};
    m_surface->Initialize(pSurface);
    m_familyIndex = m_pInstance->FindBestPhysicalDeviceIndex(m_surface);
    if (m_familyIndex < 0)
    {
        return -3;
    }
    if (m_pInstance->CreateDevice(m_device, m_familyIndex) != 0)
    {
        return -4;
    }
    return 0;
}

auto CInstance::Cleanup() noexcept -> int
{
    if (m_device != nullptr)
    {
        if (m_device->Release() != 0U)
        {
            return -1;
        }
    }
    if (m_pInstance != nullptr)
    {
        if (m_surface != nullptr)
        {
            m_pInstance->FreeSurface(m_surface);
            m_surface = VK_NULL_HANDLE;
        }
        delete m_pInstance;
        m_pInstance = nullptr;
    }
    return 0;
}

auto CInstance::CreatePresentor(Presenter& presentor, const int32_t width, const int32_t height) const noexcept -> int
{
    if (presentor.Initialize(m_device, m_surface, m_familyIndex, width, height) != 0)
    {
        return -1;
    }
    return 0;
}

auto CInstance::RecreatePresentor(Presenter& presentor, const int32_t width, const int32_t height) const noexcept -> int
{
    presentor.Cleanup();
    if (presentor.Initialize(m_device, m_surface, m_familyIndex, width, height) != 0)
    {
        return -1;
    }
    return 0;
}
} // namespace DeerVulkan