//
// Created by joran on 06/01/2024.
//

#include "Instance.h"

#include <SDL3/SDL_vulkan.h>

#include "Presenter.h"
#include "Wrapper/VkDevice.h"
#include "Wrapper/VkInstance.h"
#include "Wrapper/VkSurface.h"

namespace DeerVulkan
{
    int Instance::Initialize( SDL_Window* pWindow, const char* appTitle, const uint32_t appVersion, const char* const* extensions, const uint32_t extensionsCount ) noexcept
    {
        m_instance = new CVkInstance {};
        if ( m_instance->Initialize( appTitle, appVersion, extensions, extensionsCount ) )
        {
            return -1;
        }
        VkSurfaceKHR surface;
        if ( SDL_Vulkan_CreateSurface( pWindow, m_instance->Handle(), VK_NULL_HANDLE, &surface ) != SDL_TRUE )
        {
            return -2;
        }
        m_surface = new CVkSurface {};
        m_surface->Initialize( surface );
        m_familyIndex = m_instance->FindBestPhysicalDeviceIndex( m_surface );
        if ( m_familyIndex < 0 )
        {
            return -3;
        }
        if ( m_instance->CreateDevice( m_device, m_familyIndex ) )
        {
            return -4;
        }
        return 0;
    }
    int Instance::Cleanup() noexcept
    {

        if ( m_device )
        {
            if ( m_device->Release() )
            {
                return -1;
            }
        }
        if ( m_instance )
        {
            if ( m_surface )
            {
                m_instance->FreeSurface( m_surface );
                m_surface = VK_NULL_HANDLE;
            }
            delete m_instance;
            m_instance = nullptr;
        }
        return 0;
    }
    int Instance::CreatePresentor( Presenter& presentor, const int32_t w, const int32_t h ) const noexcept
    {
        if ( presentor.Initialize( m_device, m_surface, m_familyIndex, w, h ) )
        {
            return -1;
        }
        return 0;
    }
    int Instance::RecreatePresentor( Presenter& presentor, const int32_t w, const int32_t h ) const noexcept
    {
        presentor.Cleanup();
        if ( presentor.Initialize( m_device, m_surface, m_familyIndex, w, h ) )
        {
            return -1;
        }
        return 0;
    }
}// namespace DeerVulkan
