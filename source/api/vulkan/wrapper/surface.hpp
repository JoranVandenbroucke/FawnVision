#pragma once
#include "../deer_vulkan_core.hpp"
#include "instance.hpp"

namespace deer_vulkan
{
struct Surface
{
    vk::SurfaceKHR surface{nullptr};
    vk::SurfaceCapabilitiesKHR capabilities{};
};

[[nodiscard]] inline auto Initialize(SDL_Window* pWindow, const Instance& instance, Surface& surface) noexcept -> vk_status
{
    vk::SurfaceKHR::CType cTypeSurface{nullptr};
    const auto result{SDL_Vulkan_CreateSurface(pWindow, static_cast<VkInstance>(static_cast<vk::Instance::CType>(instance.instance)), nullptr, &cTypeSurface) ? vk_status::ok : vk_status::init_failed};
    if (result == vk_status::init_failed)
    {
        std::cerr << "vkCreateSurfaceKHR failed: " << SDL_GetError() << std::endl;
    }
    surface.surface = cTypeSurface;
    return result;
}

inline auto Cleanup(Instance& instance, Surface& surface) noexcept -> void
{
    SDL_Vulkan_DestroySurface(static_cast<VkInstance>(static_cast<vk::Instance::CType>(instance.instance)), static_cast<VkSurfaceKHR>(static_cast<vk::SurfaceKHR::CType>(surface.surface)), nullptr);
    surface.surface = nullptr;
}
} // namespace deer_vulkan
