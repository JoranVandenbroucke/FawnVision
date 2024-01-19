//
// Created by joran on 04/01/2024.
//

#include "VkSwapChain.h"

#include "VkCommandHandler.h"
#include "VkDevice.h"
#include "VkSemaphore.h"

namespace DeerVulkan
{
    CVkSwapChain::~CVkSwapChain()
    {
        vkDestroySwapchainKHR( GetDevice()->GetVkDevice(), m_swapChain, VK_NULL_HANDLE );
        for ( const auto imageView : m_imageViews )
        {
            vkDestroyImageView( GetDevice()->GetVkDevice(), imageView, VK_NULL_HANDLE );
        }
        // for (const auto image : m_images )
        // {
        // vkDestroyImage( GetDevice()->GetVkDevice(), image, VK_NULL_HANDLE );
        // }
        m_imageViews.clear();
        m_images.clear();
    }

    int CVkSwapChain::Initialize( const CVkSurface* surface, const int32_t w, const int32_t h ) noexcept
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
                .surface               = surface->GetHandle(),
                .minImageCount         = imageCount,
                .imageFormat           = format,
                .imageColorSpace       = colorSpace,
                .imageExtent           = extent,
                .imageArrayLayers      = 1,// 2 for stereo
                .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 0u,
                .pQueueFamilyIndices   = VK_NULL_HANDLE,
                .preTransform          = capabilities.currentTransform,
                .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode           = presentMode,
                .clipped               = VK_TRUE,
                .oldSwapchain          = m_swapChain,
        };
        if ( !CheckVkResult( vkCreateSwapchainKHR( GetDevice()->GetVkDevice(), &swapChainCreateInfo, VK_NULL_HANDLE, &m_swapChain ) ) )
        {
            return -1;
        }

        if ( !CheckVkResult( vkGetSwapchainImagesKHR( GetDevice()->GetVkDevice(), m_swapChain, &imageCount, VK_NULL_HANDLE ) ) )
        {
            return -1;
        }

        m_images.resize( imageCount );
        m_imageViews.resize( imageCount );
        if ( !CheckVkResult( vkGetSwapchainImagesKHR( GetDevice()->GetVkDevice(), m_swapChain, &imageCount, m_images.data() ) ) )
        {
            return -1;
        }
        for ( size_t i = 0; i < m_images.size(); i++ )
        {
            VkImageViewCreateInfo createInfo {
                    .sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image                           = m_images[ i ],
                    .viewType                        = VK_IMAGE_VIEW_TYPE_2D,
                    .format                          = format,
                    .components = {
                        .r                    = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g                    = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b                    = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a                    = VK_COMPONENT_SWIZZLE_IDENTITY,
                    },
                    .subresourceRange{
                        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel   = 0,
                        .levelCount     = 1,
                        .baseArrayLayer = 0,
                        .layerCount     = 1,
                    },
            };
            if ( !CheckVkResult( vkCreateImageView( GetDevice()->GetVkDevice(), &createInfo, VK_NULL_HANDLE, &m_imageViews[ i ] ) ) )
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
                .renderArea {
                        .extent = extent,
                },
                .layerCount           = 1,
                .colorAttachmentCount = 1,
                .pColorAttachments    = &m_colorAttachmentInfo,
        };

        m_imageFormat = format;
        m_extent      = extent;
        return 0;
    }
    int CVkSwapChain::NextImage( const CVkSemaphore* pSemaphore ) noexcept
    {
        if ( !CheckVkResult( vkAcquireNextImageKHR( GetDevice()->GetVkDevice(), m_swapChain, UINT64_MAX, pSemaphore->GetHandle( m_currentFrameIdx ), VK_NULL_HANDLE, &m_currentImageIdx ) ) )
        {
            return -1;
        }
        m_colorAttachmentInfo.imageView = m_imageViews[m_currentFrameIdx];
        return 0;
    }
    void CVkSwapChain::MakeReadyToRender( const CVkCommandHandler* pCommandHandler ) const noexcept
    {
        const VkImageMemoryBarrier imageMemoryBarrier { .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                        .dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                        .oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED,
                                                        .newLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                        .image            = m_images[ m_currentFrameIdx ],
                                                        .subresourceRange = {
                                                                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                                                                .baseMipLevel   = 0,
                                                                .levelCount     = 1,
                                                                .baseArrayLayer = 0,
                                                                .layerCount     = 1,
                                                        } };

        vkCmdPipelineBarrier( pCommandHandler->GetHandle(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier );
    }
    void CVkSwapChain::MakeReadyToPresent( const CVkCommandHandler* pCommandHandler ) noexcept
    {
        const VkImageMemoryBarrier imageMemoryBarrier { .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                        .srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                        .oldLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                        .newLayout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                        .image            = m_images[ m_currentFrameIdx ],
                                                        .subresourceRange = {
                                                                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                                                                .baseMipLevel   = 0,
                                                                .levelCount     = 1,
                                                                .baseArrayLayer = 0,
                                                                .layerCount     = 1,
                                                        } };

        vkCmdPipelineBarrier( pCommandHandler->GetHandle(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier );
        m_currentFrameIdx = (m_currentFrameIdx + 1) % m_imageViews.size();
    }

    SwapChainSupportDetails CVkSwapChain::GenerateSupportDetail() const
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( GetDevice()->GetPhysicalDeviceInfo().device, m_surface->GetHandle(), &details.capabilities );

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR( GetDevice()->GetPhysicalDeviceInfo().device, m_surface->GetHandle(), &formatCount, nullptr );

        if ( formatCount != 0 )
        {
            details.formats.resize( formatCount );
            vkGetPhysicalDeviceSurfaceFormatsKHR( GetDevice()->GetPhysicalDeviceInfo().device, m_surface->GetHandle(), &formatCount, details.formats.data() );
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR( GetDevice()->GetPhysicalDeviceInfo().device, m_surface->GetHandle(), &presentModeCount, nullptr );

        if ( presentModeCount != 0 )
        {
            details.presentModes.resize( presentModeCount );
            vkGetPhysicalDeviceSurfacePresentModesKHR( GetDevice()->GetPhysicalDeviceInfo().device, m_surface->GetHandle(), &presentModeCount, details.presentModes.data() );
        }

        return details;
    }

    VkSurfaceFormatKHR CVkSwapChain::ChooseSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& formats ) noexcept
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

    VkPresentModeKHR CVkSwapChain::ChoosePresentMode( const std::vector<VkPresentModeKHR>& presentModes ) noexcept
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

    VkExtent2D CVkSwapChain::GetExtent( const VkSurfaceCapabilitiesKHR& capabilities, const int32_t w, const int32_t h ) noexcept
    {
        if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
        {
            return capabilities.currentExtent;
        }

        const VkExtent2D actualExtent { std::clamp( static_cast<uint32_t>( w ), capabilities.minImageExtent.width, capabilities.maxImageExtent.width ),
                                        std::clamp( static_cast<uint32_t>( h ), capabilities.minImageExtent.height, capabilities.maxImageExtent.height ) };

        return actualExtent;
    }
}// namespace DeerVulkan
