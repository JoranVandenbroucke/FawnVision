#pragma once
#include "../deer_vulkan_core.hpp"

#include "device.hpp"
#include "image_view.hpp"
#include "semaphore.hpp"
#include "surface.hpp"

namespace deer_vulkan
{

struct SwapChain
{
    std::vector<Image> images{};
    std::vector<ImageView> imageViews{};
    vk::SwapchainKHR swapChain{nullptr};
    vk::Format imageFormat{vk::Format::eB8G8R8A8Srgb};
    vk::Extent2D extent{};
    std::uint32_t currentFrameIdx{};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const Surface& surface, const std::int32_t width, const std::int32_t height,
                                     SwapChain& swapChain) noexcept -> vk_status
{
    swapChain.extent = surface.capabilities.currentExtent;

    vk::Extent2D swapChainExtent{
        .width  = surface.capabilities.currentExtent.width,
        .height = surface.capabilities.currentExtent.height,
    };
    if (surface.capabilities.currentExtent.width == 0xFFFFFFFF)
    {
        swapChainExtent = vk::Extent2D{static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
    }

    constexpr auto imageFormat{vk::Format::eB8G8R8A8Srgb};
    const vk::SwapchainCreateInfoKHR swapchainCI{
        .sType                 = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext                 = nullptr,
        .flags                 = {},
        .surface               = surface.surface,
        .minImageCount         = surface.capabilities.minImageCount,
        .imageFormat           = imageFormat,
        .imageColorSpace       = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent           = vk::Extent2D{swapChainExtent.width, swapChainExtent.height},
        .imageArrayLayers      = 1,
        .imageUsage            = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode      = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = surface.capabilities.currentTransform,
        .compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode           = vk::PresentModeKHR::eFifo,
        .clipped               = vk::True,
        .oldSwapchain          = swapChain.swapChain,
    };

    swapChain.swapChain = device.device.createSwapchainKHR(swapchainCI, nullptr, dispatch.dispatch);

    const std::vector images{device.device.getSwapchainImagesKHR(swapChain.swapChain, dispatch.dispatch)};
    const std::uint32_t imageCount{static_cast<std::uint32_t>(images.size())};

    const ImageViewCreateInfo viewCI{
        .format     = static_cast<std::uint32_t>(swapChain.imageFormat),
        .aspectFlag = static_cast<std::uint32_t>(vk::ImageAspectFlagBits::eColor),
        .mipCount   = 1,
        .layerCount = 1,
        .imageType  = static_cast<std::uint8_t>(vk::ImageViewType::e2D),
    };

    swapChain.images.resize(imageCount);
    swapChain.imageViews.resize(imageCount);
    for (std::uint32_t i = 0; i < imageCount; i++)
    {
        swapChain.images[i].image = images[i];
        if (const vk_status status{Initialize(dispatch, device, swapChain.images[i], viewCI, swapChain.imageViews[i])}; IsError(status))
        {
            return status;
        }
    }
    return vk_status::ok;
}

[[nodiscard]] inline auto Recreate(const Dispatch& dispatch, const Device& device, const Surface& surface, SwapChain& source, const std::int32_t width,
                                   const std::int32_t height) noexcept -> vk_status
{
    for (auto& view : source.imageViews)
    {
        Cleanup(dispatch, device, view);
    }
    source.imageViews.clear();
    source.images.clear();

    const vk::SwapchainKHR old{source.swapChain};
    const vk_status status = Initialize(dispatch, device, surface, width, height, source);
    device.device.destroySwapchainKHR(old, nullptr, dispatch.dispatch);
    return status;
}

inline auto Cleanup(const Dispatch& dispatch, const Device& device, SwapChain& swapChain) noexcept -> void
{
    if (swapChain.swapChain == nullptr)
    {
        return;
    }
    for (auto& view : swapChain.imageViews)
    {
        Cleanup(dispatch, device, view);
    }
    swapChain.imageViews.clear();
    swapChain.images.clear();

    device.device.destroySwapchainKHR(swapChain.swapChain, nullptr, dispatch.dispatch);
}

[[nodiscard]] inline auto NextImage(const Dispatch& dispatch, const Device& device, SwapChain& swapChain, const Semaphore& semaphore) noexcept -> vk_status
{
    const vk::AcquireNextImageInfoKHR acquireInfo{.sType      = vk::StructureType::eAcquireNextImageInfoKHR,
                                                  .pNext      = nullptr,
                                                  .swapchain  = swapChain.swapChain,
                                                  .timeout    = ~0ULL,
                                                  .semaphore  = semaphore.semaphore,
                                                  .fence      = nullptr,
                                                  .deviceMask = 1U};
    return FromVkResult(device.device.acquireNextImage2KHR(&acquireInfo, &swapChain.currentFrameIdx, dispatch.dispatch));
}

[[nodiscard]] constexpr auto CurrentImage(const SwapChain& swapChain) noexcept -> const Image&
{
    return swapChain.images[swapChain.currentFrameIdx];
}
[[nodiscard]] constexpr auto CurrentImage(SwapChain& swapChain) noexcept -> Image&
{
    return swapChain.images[swapChain.currentFrameIdx];
}
[[nodiscard]] constexpr auto CurrentImageView(const SwapChain& swapChain) noexcept -> const ImageView&
{
    return swapChain.imageViews[swapChain.currentFrameIdx];
}

inline auto NextFrame(SwapChain& swapChain) noexcept -> void
{
    swapChain.currentFrameIdx = (swapChain.currentFrameIdx + 1U) % static_cast<std::uint32_t>(swapChain.images.size());
}
} // namespace deer_vulkan
