//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"

#include "VkDevice.hpp"
#include "VkSurface.hpp"
#include "VkSemaphore.hpp"

#include <vector>
namespace DeerVulkan
{
struct SSwapChainSupportDetails
{
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    VkSurfaceCapabilitiesKHR capabilities{};
};

class CVkSwapChain final
{
  public:
    BALBINO_CONSTEXPR_SINCE_CXX20 BALBINO_EXPLICIT_SINCE_CXX11 CVkSwapChain(const CVkDevice* pDevice)
        : m_pDevice{pDevice}
    {
    }

    ~CVkSwapChain()
    {
        vkDestroySwapchainKHR(m_pDevice->Device(), m_swapChain, VK_NULL_HANDLE);
        for (const auto& imageView : m_imageViews)
        {
            vkDestroyImageView(m_pDevice->Device(), imageView, VK_NULL_HANDLE);
        }
        // for (const auto image : m_images )
        // {
        // vkDestroyImage( GetDevice()->GetVkDevice(), image, VK_NULL_HANDLE );
        // }
        m_imageViews.clear();
        m_images.clear();
    }
    CVkSwapChain(const CVkSwapChain& other)                        = delete;
    CVkSwapChain(CVkSwapChain&& other) noexcept                    = delete;
    auto operator=(const CVkSwapChain& other) -> CVkSwapChain&     = delete;
    auto operator=(CVkSwapChain&& other) noexcept -> CVkSwapChain& = delete;

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Initialize(const CVkSurface* pSurface, const int32_t width, const int32_t height) BALBINO_NOEXCEPT_SINCE_CXX11 -> int32_t
    {
        m_surface = pSurface;
        const auto& [formats, presentModes, capabilities]{GenerateSupportDetail()};
        const auto [format, colorSpace]{ChooseSurfaceFormat(formats)};
        const VkPresentModeKHR presentMode{ChoosePresentMode(presentModes)};
        const VkExtent2D extent{GetExtent(capabilities, width, height)};

        uint32_t imageCount{capabilities.minImageCount + 1};
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        const VkSwapchainCreateInfoKHR swapChainCreateInfo{
            .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext                 = VK_NULL_HANDLE,
            .flags                 = 0,
            .surface               = pSurface->Handle(),
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
            .oldSwapchain          = m_swapChain,
        };
        if (!CheckVkResult(vkCreateSwapchainKHR(m_pDevice->Device(), &swapChainCreateInfo, VK_NULL_HANDLE, &m_swapChain)))
        {
            return -1;
        }

        if (!CheckVkResult(vkGetSwapchainImagesKHR(m_pDevice->Device(), m_swapChain, &imageCount, VK_NULL_HANDLE)))
        {
            return -1;
        }

        m_images.resize(imageCount);
        m_imageViews.resize(imageCount);
        if (!CheckVkResult(vkGetSwapchainImagesKHR(m_pDevice->Device(), m_swapChain, &imageCount, m_images.data())))
        {
            return -1;
        }
        for (size_t i = 0; i < m_images.size(); i++)
        {
            VkImageViewCreateInfo const createInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = VK_NULL_HANDLE,
                .flags = 0,
                .image = m_images[i],
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
            if (!CheckVkResult(vkCreateImageView(m_pDevice->Device(), &createInfo, VK_NULL_HANDLE, &m_imageViews[i])))
            {
                return -1;
            }
        }
        m_clearValue = {{{0.7863F, 0.4386F, 0.426F, 1.0F}}};

        m_colorAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        m_colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        m_colorAttachmentInfo.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_colorAttachmentInfo.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
        m_colorAttachmentInfo.clearValue  = m_clearValue;

        m_renderInfo = {
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
            .pColorAttachments    = &m_colorAttachmentInfo,
            .pDepthAttachment     = VK_NULL_HANDLE,
            .pStencilAttachment   = VK_NULL_HANDLE,
        };

        m_imageFormat = format;
        m_extent      = extent;
        return 0;
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto NextImage(const CVkSemaphore* pSemaphore) BALBINO_NOEXCEPT_SINCE_CXX11 -> int32_t
    {
        const VkAcquireNextImageInfoKHR acquireNextImageInfo{
            .sType      = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
            .pNext      = VK_NULL_HANDLE,
            .swapchain  = m_swapChain,
            .timeout    = UINT64_MAX,
            .semaphore  = pSemaphore->Handle(),
            .fence      = VK_NULL_HANDLE,
            .deviceMask = 1U,
        };
        if (!CheckVkResult(vkAcquireNextImage2KHR(m_pDevice->Device(), &acquireNextImageInfo, &m_currentImageIdx)))
        {
            return -1;
        }
        m_colorAttachmentInfo.imageView = m_imageViews[m_currentFrameIdx];
        return 0;
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Handle() const BALBINO_NOEXCEPT_SINCE_CXX11 -> const VkSwapchainKHR&
    {
        return m_swapChain;
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetRenderInfo() const BALBINO_NOEXCEPT_SINCE_CXX11 -> const VkRenderingInfo&
    {
        return m_renderInfo;
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetImageCount() const BALBINO_NOEXCEPT_SINCE_CXX11 -> int32_t
    {
        return static_cast<int>(m_images.size());
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetImageIndex() const BALBINO_NOEXCEPT_SINCE_CXX11 -> uint32_t
    {
        return m_currentImageIdx;
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetFrameIndex() const BALBINO_NOEXCEPT_SINCE_CXX11 -> uint32_t
    {
        return m_currentFrameIdx;
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetFormat() const BALBINO_NOEXCEPT_SINCE_CXX11 -> VkFormat
    {
        return m_imageFormat;
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetImageRenderMemory() const BALBINO_NOEXCEPT_SINCE_CXX11-> VkImageMemoryBarrier
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
            .image = m_images[m_currentFrameIdx],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
    }

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetImagePresentMemory() const BALBINO_NOEXCEPT_SINCE_CXX11-> VkImageMemoryBarrier
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
            .image = m_images[m_currentFrameIdx],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
    }

    BALBINO_CONSTEXPR_SINCE_CXX23 void NextFrame()BALBINO_NOEXCEPT_SINCE_CXX11
    {
        m_currentFrameIdx = (m_currentFrameIdx + 1) % m_imageViews.size();
    }

  private:
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    const CVkSurface* m_surface{BALBINO_NULL};
    const CVkDevice* m_pDevice{BALBINO_NULL};
    VkRenderingInfoKHR m_renderInfo{};
    VkRenderingAttachmentInfoKHR m_colorAttachmentInfo{};
    VkClearValue m_clearValue{};
    VkExtent2D m_extent{};
    VkSwapchainKHR m_swapChain{VK_NULL_HANDLE};
    VkFormat m_imageFormat{};
    uint32_t m_currentImageIdx{};
    uint32_t m_currentFrameIdx{};const

    auto GenerateSupportDetail() const -> SSwapChainSupportDetails
    {
        SSwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pDevice->PhysicalDeviceInfo().device, m_surface->Handle(), &details.capabilities);

        uint32_t formatCount{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_pDevice->PhysicalDeviceInfo().device, m_surface->Handle(), &formatCount, VK_NULL_HANDLE);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(m_pDevice->PhysicalDeviceInfo().device, m_surface->Handle(), &formatCount, details.formats.data());
        }

        uint32_t presentModeCount{};
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_pDevice->PhysicalDeviceInfo().device, m_surface->Handle(), &presentModeCount, VK_NULL_HANDLE);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(m_pDevice->PhysicalDeviceInfo().device, m_surface->Handle(), &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    static auto ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) noexcept -> VkSurfaceFormatKHR
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

    static auto ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) noexcept -> VkPresentModeKHR
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

    static auto GetExtent(const VkSurfaceCapabilitiesKHR& capabilities, const int32_t width, const int32_t height) noexcept -> VkExtent2D
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        const VkExtent2D actualExtent{std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                                      std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};

        return actualExtent;
    }
};
} // namespace DeerVulkan