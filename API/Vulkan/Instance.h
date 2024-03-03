//
// Copyright (c) 2024.
// Author: Joran
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

class CInstance
{
public:
    CInstance() = default;
    auto Initialize(SDL_Window* pWindow, const char* pAppTitle, uint32_t appVersion, const char* const* pExtensions, uint32_t extensionsCount) noexcept -> int32_t;
    auto Cleanup() noexcept -> int32_t;

    auto CreatePresentor(Presenter& presentor, int32_t width, int32_t height) const noexcept -> int32_t;
    auto RecreatePresentor(Presenter& presentor, int32_t width, int32_t height) const noexcept -> int32_t;

private:
    CVkInstance* m_pInstance;
    CVkDevice* m_device;
    CVkSurface* m_surface;
    int32_t m_familyIndex;
};
} // namespace DeerVulkan