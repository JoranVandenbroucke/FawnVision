//
// Copyright (c) 2024.
// Author: Joran.
//

#pragma once
#include "../DeerVulkan_Core.hpp"
#include "VkInstance.hpp"

#include <SDL3/SDL_vulkan.h>

namespace DeerVulkan
{
struct SVkSurface
{
    VkSurfaceKHR surface{VK_NULL_HANDLE};
};
inline auto InitializeSurface(SDL_Window* pWindow, const SVkInstance& instance, SVkSurface& surface)
{
    return SDL_Vulkan_CreateSurface(pWindow, instance.instance, nullptr, &surface.surface);
}
inline auto Cleanup(const SVkInstance& instance, const SVkSurface& surface)
{
    SDL_Vulkan_DestroySurface(instance.instance, surface.surface, nullptr);
}
} // namespace DeerVulkan