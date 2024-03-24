//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "DeerVulkan_Core.hpp"
#include "Wrapper/VkQueue.hpp"

struct SDL_Window;

namespace DeerVulkan
{
class CPresenter;

class CVkInstance;
class CVkSurface;
class CVkDevice;

class CInstance
{
public:
    CInstance() = default;
    auto Initialize(SDL_Window* pWindow, const char* pAppTitle, uint32_t appVersion, const char* const* pExtensions, uint32_t extensionsCount) noexcept -> int32_t;
    auto Cleanup() noexcept -> int32_t;

    auto CreatePresentor(CPresenter& presentor, int32_t width, int32_t height) const noexcept -> int32_t;
    auto RecreatePresentor(CPresenter& presentor, int32_t width, int32_t height) const noexcept -> int32_t;

private:
    CVkInstance* m_pInstance;
    CVkDevice* m_device;
    CVkSurface* m_surface;
    SQueueFamily m_familyIndex;
};
} // namespace DeerVulkan