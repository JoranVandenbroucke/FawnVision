//
// Created by Joran on 09/01/2024.
//

#pragma once
#include "../DeerVulkan_Core.h"
#include "Base.h"

namespace DeerVulkan
{
    class CVkCommandPool;
    class CVkSwapChain;

    class CVkCommandHandler final : public CDeviceObject
    {
    public:
        explicit CVkCommandHandler( CVkDevice* pDevice )
            : CDeviceObject( pDevice )
        {}
        ~CVkCommandHandler() override;
        int32_t Initialize( CVkSwapChain* pSwapChain, uint32_t queueFamilyIndex ) noexcept;
        int32_t BeginCommand() const noexcept;
        int32_t EndCommand() const noexcept;
        void BeginRender() const noexcept;
        void EndRender() const noexcept;

        VkCommandBuffer GetHandle() const noexcept
        {
            return m_commandBuffer;
        }

    private:
        VkCommandPool m_commandPool { VK_NULL_HANDLE };
        VkCommandBuffer m_commandBuffer { VK_NULL_HANDLE };
        PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR { VK_NULL_HANDLE };
        PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR { VK_NULL_HANDLE };
        CVkSwapChain* m_swapChain { BALBINO_NULL };
        uint32_t m_commandBufferCount { 1 };
    };
}// namespace DeerVulkan
