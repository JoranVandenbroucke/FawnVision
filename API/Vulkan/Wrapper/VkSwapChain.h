//
// Created by joran on 04/01/2024.
//

#pragma once
#include "../DeerVulkan_Core.h"
#include "Base.h"
#include "VkFence.h"
#include "VkSemaphore.h"
#include "VkSurface.h"

namespace DeerVulkan
{
    struct SwapChainSupportDetails
    {
        std::vector<VkSurfaceFormatKHR> formats {};
        std::vector<VkPresentModeKHR> presentModes {};
        VkSurfaceCapabilitiesKHR capabilities {};
    };

    class CVkSwapChain final : public CDeviceObject
    {
      public:
        explicit CVkSwapChain( const CVkDevice* pDevice )
            : CDeviceObject { pDevice }
        {
        }
        ~CVkSwapChain() override
        {
            vkDestroySwapchainKHR( Device()->VkDevice(), m_swapChain, VK_NULL_HANDLE );
            for ( const auto imageView : m_imageViews )
            {
                vkDestroyImageView( Device()->VkDevice(), imageView, VK_NULL_HANDLE );
            }
            // for (const auto image : m_images )
            // {
            // vkDestroyImage( GetDevice()->GetVkDevice(), image, VK_NULL_HANDLE );
            // }
            m_imageViews.clear();
            m_images.clear();
        }

        int32_t Initialize( const CVkSurface* surface, const int32_t w, const int32_t h ) noexcept
        {
            m_surface = surface;
            const auto& [ formats, presentModes, capabilities ] { GenerateSupportDetail() };
            const auto [ format, colorSpace ] { ChooseSurfaceFormat( formats ) };
            const VkPresentModeKHR presentMode { ChoosePresentMode( presentModes ) };
            const VkExtent2D extent { GetExtent( capabilities, w, h ) };

            uint32_t imageCount { capabilities.minImageCount + 1 };
            if ( capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount )
            {
                imageCount = capabilities.maxImageCount;
            }

            const VkSwapchainCreateInfoKHR swapChainCreateInfo {
                .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .pNext                 = VK_NULL_HANDLE,
                .flags                 = 0,
                .surface               = surface->Handle(),
                .minImageCount         = imageCount,
                .imageFormat           = format,
                .imageColorSpace       = colorSpace,
                .imageExtent           = extent,
                .imageArrayLayers      = 1,// 2 for stereo
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
            if ( !CheckVkResult( vkCreateSwapchainKHR( Device()->VkDevice(), &swapChainCreateInfo, VK_NULL_HANDLE, &m_swapChain ) ) )
            {
                return -1;
            }

            if ( !CheckVkResult( vkGetSwapchainImagesKHR( Device()->VkDevice(), m_swapChain, &imageCount, VK_NULL_HANDLE ) ) )
            {
                return -1;
            }

            m_images.resize( imageCount );
            m_imageViews.resize( imageCount );
            if ( !CheckVkResult( vkGetSwapchainImagesKHR( Device()->VkDevice(), m_swapChain, &imageCount, m_images.data() ) ) )
            {
                return -1;
            }
            for ( size_t i = 0; i < m_images.size(); i++ )
            {
                VkImageViewCreateInfo createInfo {
                    .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .pNext      = VK_NULL_HANDLE,
                    .flags      = 0,
                    .image      = m_images[ i ],
                    .viewType   = VK_IMAGE_VIEW_TYPE_2D,
                    .format     = format,
                    .components = {
                        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                    },
                    .subresourceRange {
                        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel   = 0,
                        .levelCount     = 1,
                        .baseArrayLayer = 0,
                        .layerCount     = 1,
                    },
                };
                if ( !CheckVkResult( vkCreateImageView( Device()->VkDevice(), &createInfo, VK_NULL_HANDLE, &m_imageViews[ i ] ) ) )
                {
                    return -1;
                }
            }
            m_clearValue = { { { 0.7863f, 0.4386f, 0.426f, 1.0f } } };

            m_colorAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            m_colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
            m_colorAttachmentInfo.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
            m_colorAttachmentInfo.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
            m_colorAttachmentInfo.clearValue  = m_clearValue;

            m_renderInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                .pNext = VK_NULL_HANDLE,
                .flags = 0,
                .renderArea {
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
        int32_t NextImage( const CVkSemaphore* pSemaphore ) noexcept
        {
            const VkAcquireNextImageInfoKHR acquireNextImageInfo {
                .sType      = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
                .pNext      = VK_NULL_HANDLE,
                .swapchain  = m_swapChain,
                .timeout    = UINT64_MAX,
                .semaphore  = pSemaphore->Handle(),
                .fence      = VK_NULL_HANDLE,
                .deviceMask = 1U,
            };
            if ( !CheckVkResult( vkAcquireNextImage2KHR( Device()->VkDevice(), &acquireNextImageInfo, &m_currentImageIdx ) ) )
            {
                return -1;
            }
            m_colorAttachmentInfo.imageView = m_imageViews[ m_currentFrameIdx ];
            return 0;
        }

        const VkSwapchainKHR& Handle() const noexcept
        {
            return m_swapChain;
        }
        const VkRenderingInfo& GetRenderInfo() const noexcept
        {
            return m_renderInfo;
        }
        int32_t GetImageCount() const noexcept
        {
            return static_cast<int>( m_images.size() );
        }
        uint32_t GetImageIndex() const noexcept
        {
            return m_currentImageIdx;
        }
        uint32_t GetFrameIndex() const noexcept
        {
            return m_currentFrameIdx;
        }
        VkFormat GetFormat() const noexcept
        {
            return m_imageFormat;
        }
        VkImageMemoryBarrier GetImageRenderMemory() const
        {
            return {
                .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext               = VK_NULL_HANDLE,
                .srcAccessMask       = 0,
                .dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .srcQueueFamilyIndex = 0,
                .dstQueueFamilyIndex = 0,
                .image               = m_images[ m_currentFrameIdx ],
                .subresourceRange    = {
                       .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                       .baseMipLevel   = 0,
                       .levelCount     = 1,
                       .baseArrayLayer = 0,
                       .layerCount     = 1,
                },
            };
        }
        VkImageMemoryBarrier GetImagePresentMemory() const
        {
            return {
                .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext               = VK_NULL_HANDLE,
                .srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask       = 0,
                .oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = 0,
                .dstQueueFamilyIndex = 0,
                .image               = m_images[ m_currentFrameIdx ],
                .subresourceRange    = {
                       .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                       .baseMipLevel   = 0,
                       .levelCount     = 1,
                       .baseArrayLayer = 0,
                       .layerCount     = 1,
                },
            };
        }
        void NextFrame()
        {
            m_currentFrameIdx = ( m_currentFrameIdx + 1 ) % m_imageViews.size();
        };

      private:
        std::vector<VkImage> m_images {};
        std::vector<VkImageView> m_imageViews {};
        VkRenderingInfoKHR m_renderInfo {};
        VkRenderingAttachmentInfoKHR m_colorAttachmentInfo {};
        VkClearValue m_clearValue {};
        VkExtent2D m_extent {};
        VkSwapchainKHR m_swapChain { VK_NULL_HANDLE };
        const CVkSurface* m_surface {};
        VkFormat m_imageFormat {};
        uint32_t m_currentImageIdx {};
        uint32_t m_currentFrameIdx {};

        SwapChainSupportDetails GenerateSupportDetail() const
        {
            SwapChainSupportDetails details;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( Device()->PhysicalDeviceInfo().device, m_surface->Handle(), &details.capabilities );

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR( Device()->PhysicalDeviceInfo().device, m_surface->Handle(), &formatCount, nullptr );

            if ( formatCount != 0 )
            {
                details.formats.resize( formatCount );
                vkGetPhysicalDeviceSurfaceFormatsKHR( Device()->PhysicalDeviceInfo().device, m_surface->Handle(), &formatCount, details.formats.data() );
            }

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR( Device()->PhysicalDeviceInfo().device, m_surface->Handle(), &presentModeCount, nullptr );

            if ( presentModeCount != 0 )
            {
                details.presentModes.resize( presentModeCount );
                vkGetPhysicalDeviceSurfacePresentModesKHR( Device()->PhysicalDeviceInfo().device, m_surface->Handle(), &presentModeCount, details.presentModes.data() );
            }

            return details;
        }
        static VkSurfaceFormatKHR ChooseSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& formats ) noexcept
        {
            for ( const auto& format : formats )
            {
                if ( format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
                {
                    return format;
                }
            }
            return formats[ 0 ];
        }
        static VkPresentModeKHR ChoosePresentMode( const std::vector<VkPresentModeKHR>& presentModes ) noexcept
        {
            for ( const auto& presentMode : presentModes )
            {
                if ( presentMode == VK_PRESENT_MODE_MAILBOX_KHR )
                {
                    return presentMode;
                }
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        static VkExtent2D GetExtent( const VkSurfaceCapabilitiesKHR& capabilities, const int32_t w, const int32_t h ) noexcept
        {
            if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
            {
                return capabilities.currentExtent;
            }

            const VkExtent2D actualExtent { std::clamp( static_cast<uint32_t>( w ), capabilities.minImageExtent.width, capabilities.maxImageExtent.width ),
                                            std::clamp( static_cast<uint32_t>( h ), capabilities.minImageExtent.height, capabilities.maxImageExtent.height ) };

            return actualExtent;
        }
    };
}// namespace DeerVulkan
