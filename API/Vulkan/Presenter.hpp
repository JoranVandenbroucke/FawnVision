//
// Copyright (c) 2024.
// Author: Joran
//
#pragma once
#include "Versions.hpp"
#include <array>

namespace DeerVulkan
{
struct SQueueFamily;
class CShader;

class CVkDevice;
class CVkCommandPool;
class CVkFence;
class CVkQueue;
class CVkSwapChain;
class CVkSurface;
class CVkSemaphore;

constexpr uint8_t g_graphicsQueueId{0};
constexpr uint8_t g_computeQueueId{1};
constexpr uint8_t g_presentQueueId{2};
constexpr uint8_t g_queueCount{3};

class CPresenter
{
public:
    auto Initialize(const CVkDevice* pDevice, const CVkSurface* pSurface, const SQueueFamily& queueFamily, int32_t width, int32_t height) noexcept -> int32_t;
    void Cleanup() const noexcept;
    [[nodiscard]] auto BeginRender() const noexcept -> int32_t;
    [[nodiscard]] auto EndRender() const noexcept -> int32_t;

    [[nodiscard]] auto GetCommandHandler() const noexcept -> CVkCommandPool*
    {
        return m_commandHandler;
    }

    void SetRenderPass(float viewX, float viewY, float viewW, float viewH, uint8_t compareOperator, bool enableDepthTest, bool enableWriteDepth, bool isFrontFaceClockWice) const;
    void FullscreenEffect(const CShader& shader) const;

private:
    std::array<CVkQueue*, g_queueCount> m_queue{BALBINO_NULL,BALBINO_NULL,BALBINO_NULL};
    CVkSwapChain* m_swapChain{BALBINO_NULL};
    CVkSemaphore* m_timelineSemaphore{BALBINO_NULL};
    CVkSemaphore* m_binarySemaphore{BALBINO_NULL};
    CVkFence* m_fence{BALBINO_NULL};
    CVkCommandPool* m_commandHandler{BALBINO_NULL};
};
} // namespace DeerVulkan