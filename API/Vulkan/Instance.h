//
// Created by joran on 06/01/2024.
//

#pragma once
#include "DeerVulkan_Core.h"

struct SDL_Window;

namespace DeerVulkan
{
    class Presenter;

    class CVkInstance;
    class CVkSurface;
    class CVkDevice;

    class Instance
    {
    public:
        Instance() = default;
        int32_t Initialize( SDL_Window* pWindow, const char* appTitle, uint32_t appVersion, const char* const* extensions, uint32_t extensionsCount ) noexcept;
        int32_t Cleanup() noexcept;

        int32_t CreatePresentor( Presenter& presentor, int32_t w, int32_t h ) const noexcept;
        int32_t RecreatePresentor( Presenter& presentor, int32_t w, int32_t h ) const noexcept;

    private:
        CVkInstance* m_instance;
        CVkDevice* m_device;
        CVkSurface* m_surface;
        int32_t m_familyIndex;
    };
}// namespace DeerVulkan
