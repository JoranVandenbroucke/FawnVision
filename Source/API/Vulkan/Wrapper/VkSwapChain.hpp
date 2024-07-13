//
// Copyright (c) 2024.
// Author: Joran.
//

#pragma once
#include "../DeerVulkan_Core.hpp"
#include "VkDevice.hpp"
#include "VkSemaphore.hpp"
#include "VkSurface.hpp"

#include <vector>

namespace DeerVulkan
{
struct SSwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct SVkSwapChain
{
    uint32_t currentFrameIdx;
    VkFormat imageFormat;
    VkSwapchainKHR swapchain;
    VkExtent2D extent;
    VkClearValue clearValue;
    VkRenderingAttachmentInfo colorAttachmentInfo;
    VkRenderingInfo renderingInfo;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};

inline auto ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) noexcept -> VkSurfaceFormatKHR
{
    for (const auto& format : formats)
    {
        if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }
    return formats[0];
}

inline auto ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) noexcept -> VkPresentModeKHR
{
    for (const auto& presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

inline auto GetExtent(const VkSurfaceCapabilitiesKHR& capabilities, const int32_t width, const int32_t height) noexcept -> VkExtent2D
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    const VkExtent2D actualExtent{std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                                  std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};

    return actualExtent;
}

inline auto GenerateSupportDetail(const SVkDevice& device, const SVkSurface& surface) -> SSwapChainSupportDetails
{
    SSwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.deviceInfo.device, surface.surface, &details.capabilities);

    uint32_t formatCount{};
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.deviceInfo.device, surface.surface, &formatCount, VK_NULL_HANDLE);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.deviceInfo.device, surface.surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount{};
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.deviceInfo.device, surface.surface, &presentModeCount, VK_NULL_HANDLE);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.deviceInfo.device, surface.surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

inline auto Initialize(SVkSwapChain& swapChain, const SVkDevice& device, const SVkSurface& surface, const int32_t width, const int32_t height) -> int32_t
{
    const auto& [capabilities, formats, presentModes]{GenerateSupportDetail(device, surface)};
    const auto [format, colorSpace]{ChooseSurfaceFormat(formats)};
    const VkPresentModeKHR presentMode{ChoosePresentMode(presentModes)};
    const VkExtent2D extent{GetExtent(capabilities, width, height)};

    uint32_t imageCount{capabilities.minImageCount + 1};
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    if (const VkSwapchainCreateInfoKHR swapChainCreateInfo{
            .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext                 = VK_NULL_HANDLE,
            .flags                 = 0,
            .surface               = surface.surface,
            .minImageCount         = imageCount,
            .imageFormat           = format,
            .imageColorSpace       = colorSpace,
            .imageExtent           = extent,
            .imageArrayLayers      = 1, // 2 for stereo
            .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0U,
            .pQueueFamilyIndices   = VK_NULL_HANDLE,
            .preTransform          = capabilities.currentTransform,
            .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode           = presentMode,
            .clipped               = VK_TRUE,
            .oldSwapchain          = swapChain.swapchain,
        };
        !CheckVkResult(vkCreateSwapchainKHR(device.device, &swapChainCreateInfo, VK_NULL_HANDLE, &swapChain.swapchain)))
    {
        return -1;
    }

    if (!CheckVkResult(vkGetSwapchainImagesKHR(device.device, swapChain.swapchain, &imageCount, VK_NULL_HANDLE)))
    {
        return -1;
    }

    swapChain.images.resize(imageCount);
    swapChain.imageViews.resize(imageCount);
    if (!CheckVkResult(vkGetSwapchainImagesKHR(device.device, swapChain.swapchain, &imageCount, swapChain.images.data())))
    {
        return -1;
    }
    for (size_t i = 0; i < swapChain.images.size(); i++)
    {
        VkImageViewCreateInfo const createInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = VK_NULL_HANDLE,
                .flags = 0,
                .image = swapChain.images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = format,
                .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
                .subresourceRange{
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };
        if (!CheckVkResult(vkCreateImageView(device.device, &createInfo, VK_NULL_HANDLE, &swapChain.imageViews[i])))
        {
            return -1;
        }
    }
    swapChain.clearValue = {{{0.7863F, 0.4386F, 0.426F, 1.0F}}};

    swapChain.colorAttachmentInfo = {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext              = VK_NULL_HANDLE,
        .imageView          = VK_NULL_HANDLE,
        .imageLayout        = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
        .resolveMode        = VK_RESOLVE_MODE_NONE,
        .resolveImageView   = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue         = swapChain.clearValue,
    };

    swapChain.renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .renderArea{
            .offset = {},
            .extent = extent,
        },
        .layerCount           = 1,
        .viewMask             = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &swapChain.colorAttachmentInfo,
        .pDepthAttachment     = VK_NULL_HANDLE,
        .pStencilAttachment   = VK_NULL_HANDLE,
    };

    swapChain.imageFormat = format;
    swapChain.extent      = extent;
    return 0;
}
inline void Cleanup(const SVkDevice& device, SVkSwapChain& swapChain)
{
    vkDestroySwapchainKHR(device.device, swapChain.swapchain, nullptr);
    std::ranges::for_each(swapChain.imageViews,
                          [&vkdevice = device.device](const VkImageView& imageView)
                          {
                              vkDestroyImageView(vkdevice, imageView, nullptr);
                          });
    swapChain.swapchain = VK_NULL_HANDLE;
    swapChain.images.clear();
    swapChain.imageViews.clear();
}

inline auto NextImage(const SVkDevice& device, SVkSwapChain& swapChain, const SVkSemaphore& semaphore) -> int32_t
{
    if (const VkAcquireNextImageInfoKHR acquireNextImageInfo{
            .sType      = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
            .pNext      = VK_NULL_HANDLE,
            .swapchain  = swapChain.swapchain,
            .timeout    = UINT64_MAX,
            .semaphore  = semaphore.semaphore,
            .fence      = VK_NULL_HANDLE,
            .deviceMask = 1U,
        };
        !CheckVkResult(vkAcquireNextImage2KHR(device.device, &acquireNextImageInfo, &swapChain.currentFrameIdx)))
    {
        return -1;
    }
    swapChain.colorAttachmentInfo.imageView = swapChain.imageViews[swapChain.currentFrameIdx];
    return 0;
}

inline auto GetImageRenderMemory(const SVkSwapChain& swapChain) -> VkImageMemoryBarrier
{
    return {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = VK_NULL_HANDLE,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = 0,
            .dstQueueFamilyIndex = 0,
            .image = swapChain.images[swapChain.currentFrameIdx],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
}
inline void NextFrame(SVkSwapChain& swapChain)
{
    swapChain.currentFrameIdx = (swapChain.currentFrameIdx + 1) % swapChain.images.size();
}
inline auto GetImagePresentMemory(const SVkSwapChain& swapChain) -> VkImageMemoryBarrier
{
    return {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = VK_NULL_HANDLE,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = 0,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = 0,
            .dstQueueFamilyIndex = 0,
            .image = swapChain.images[swapChain.currentFrameIdx],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
}
} // namespace DeerVulkan