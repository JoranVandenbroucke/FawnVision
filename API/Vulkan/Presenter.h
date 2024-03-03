//
// Copyright (c) 2024.
// Author: Joran
//
#pragma once
#include "Versions.hpp"

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

class CPresenter
{
public:
    auto Initialize(const CVkDevice* pDevice, const CVkSurface* pSurface, int32_t familyIndex, int32_t width, int32_t height) noexcept -> int32_t;
    void Cleanup() const noexcept;
    [[nodiscard]] auto BeginRender() const noexcept -> int32_t;
    [[nodiscard]] auto EndRender() const noexcept -> int32_t;

    [[nodiscard]] auto GetCommandHandler() const noexcept -> CVkCommandHandler*
    {
        return m_commandHandler;
    }

    void SetRenderPass(float viewX, float viewY, float viewW, float viewH, uint8_t compareOperator, bool enableDepthTest, bool enableWriteDepth, bool isFrontFaceClockWice) const;
    void FullscreenEffect(const CShader& shader) const;

private:
    CVkQueue* m_queue{BALBINO_NULL};
    CVkSwapChain* m_swapChain{BALBINO_NULL};
    CVkSemaphore* m_timelineSemaphore{BALBINO_NULL};
    CVkSemaphore* m_binarySemaphore{BALBINO_NULL};
    CVkFence* m_fence{BALBINO_NULL};
    CVkCommandHandler* m_commandHandler{BALBINO_NULL};
};
} // namespace DeerVulkan