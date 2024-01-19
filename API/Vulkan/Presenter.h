//
// Created by Joran on 08/01/2024.
//
#pragma once
#include "DeerVulkan_Core.h"

namespace DeerVulkan
{
    class CVkDevice;

    class CVkQueue;
    class CVkSwapChain;
    class CVkSurface;
    class CVkSemaphore;
    class CVkFence;
    class CVkCommandHandler;

    class Presenter
    {
    public:
        int Initialize( CVkDevice* pDevice, const CVkSurface* pSurface, int32_t familyIndex, int32_t w, int32_t h ) noexcept;
        void Cleanup() noexcept;
        [[nodiscard]] int BeginRender() const noexcept;
        [[nodiscard]] int EndRender() const noexcept;

    private:
        CVkQueue* m_queue { BALBINO_NULL };
        CVkSwapChain* m_swapChain { BALBINO_NULL };
        CVkSemaphore* m_renderSemaphore { BALBINO_NULL };
        CVkSemaphore* m_imageSemaphore { BALBINO_NULL };
        CVkFence* m_fence { BALBINO_NULL };
        CVkCommandHandler* m_commandHandler { BALBINO_NULL };
    };
}// namespace DeerVulkan
