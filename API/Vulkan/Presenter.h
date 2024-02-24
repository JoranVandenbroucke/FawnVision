//
// Created by Joran on 08/01/2024.
//
#pragma once
#include "Wrapper/VkFence.h"

namespace DeerVulkan
{
    class CShader;

    class CVkDevice;
    class CVkCommandHandler;
    class CVkFence;
    class CVkQueue;
    class CVkSwapChain;
    class CVkSurface;
    class CVkSemaphore;

    class Presenter
    {
      public:
        int32_t Initialize( const CVkDevice* pDevice, const CVkSurface* pSurface, int32_t familyIndex, int32_t w, int32_t h ) noexcept;
        void Cleanup() const noexcept;
        [[nodiscard]] int32_t BeginRender() const noexcept;
        [[nodiscard]] int32_t EndRender() const noexcept;
        [[nodiscard]] CVkCommandHandler* GetCommandHandler() const noexcept
        {
            return m_commandHandler;
        }
        void SetRenderPass( float viewX, float viewY, float viewW, float viewH, uint8_t compareOperator, bool enableDepthTest, bool enableWriteDepth, bool isFrontFaceClockWice ) const;
        void FullscreenEffect( const CShader& shader ) const;

      private:
        CVkQueue* m_queue { BALBINO_NULL };
        CVkSwapChain* m_swapChain { BALBINO_NULL };
        CVkSemaphore* m_timelineSemaphore { BALBINO_NULL };
        CVkSemaphore* m_binarySemaphore { BALBINO_NULL };
        CVkFence* m_fence { BALBINO_NULL };
        CVkCommandHandler* m_commandHandler { BALBINO_NULL };
    };
}// namespace DeerVulkan
