//
// Created by joran on 04/01/2024.
//

#pragma once
#include "../DeerVulkan_Core.h"
#include "Base.h"

namespace DeerVulkan
{
    class CVkCommandHandler;
    class CVkSemaphore;
    class CVkSurface;
    struct SwapChainSupportDetails
    {
        std::vector<VkSurfaceFormatKHR> formats {};
        std::vector<VkPresentModeKHR> presentModes {};
        VkSurfaceCapabilitiesKHR capabilities {};
    };

    class CVkSwapChain final : public CDeviceObject
    {
    public:
        explicit CVkSwapChain( CVkDevice* pDevice )
            : CDeviceObject { pDevice }
        {}
        ~CVkSwapChain() override;

        int Initialize( const CVkSurface* surface, int32_t w, int32_t h ) noexcept;
        int NextImage( const CVkSemaphore* pSemaphore ) noexcept;
        void MakeReadyToRender( const CVkCommandHandler* pCommandHandler ) const noexcept;
        void MakeReadyToPresent( const CVkCommandHandler* pCommandHandler ) noexcept;

        const VkRenderingInfo& GetRenderInfo() const noexcept
        {
            return m_renderInfo;
        }
        int GetImageCount() const noexcept
        {
            return static_cast<int>( m_images.size() );
        }
        VkSwapchainKHR GetHandle() const noexcept
        {
            return m_swapChain;
        }
        uint32_t GetImageIndex() const noexcept
        {
            return m_currentImageIdx;
        }
        uint32_t GetFrameIndex() const noexcept
        {
            return m_currentFrameIdx;
        }

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

        SwapChainSupportDetails GenerateSupportDetail() const;
        static VkSurfaceFormatKHR ChooseSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& formats ) noexcept;
        static VkPresentModeKHR ChoosePresentMode( const std::vector<VkPresentModeKHR>& presentModes ) noexcept;
        static VkExtent2D GetExtent( const VkSurfaceCapabilitiesKHR& capabilities, int32_t w, int32_t h ) noexcept;
    };
}// namespace DeerVulkan
