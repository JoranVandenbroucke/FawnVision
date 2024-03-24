//
// Copyright (c) 2024.
// Author: Joran
//

#include "Presenter.hpp"

#include "FawnVision_Core.hpp"
#include "Wrapper/VkCommandPool.hpp"
#include "Wrapper/VkQueue.hpp"
#include "Wrapper/VkSemaphore.hpp"
#include "Wrapper/VkSwapChain.hpp"

namespace DeerVulkan
{
auto CPresenter::Initialize(const CVkDevice* pDevice, const CVkSurface* pSurface, const SQueueFamily& queueFamily, const int32_t width, const int32_t height) noexcept -> int
{
    m_queue[g_graphicsQueueId] = new CVkQueue{pDevice, static_cast<uint32_t>(queueFamily.graphicsFamily), 0U};
    if (m_queue[g_graphicsQueueId]->Initialize() != 0)
    {
        return -1;
    }
    m_queue[g_computeQueueId] = new CVkQueue{pDevice, static_cast<uint32_t>(queueFamily.graphicsFamily), 1U};
    if (m_queue[g_computeQueueId]->Initialize() != 0)
    {
        return -1;
    }
    m_queue[g_presentQueueId] = new CVkQueue{pDevice, static_cast<uint32_t>(queueFamily.presentFamily), queueFamily.graphicsFamily == queueFamily.presentFamily? 2U:0U};
    if (m_queue[g_presentQueueId]->Initialize() != 0)
    {
        return -1;
    }
    m_swapChain = new CVkSwapChain{pDevice};
    if (m_swapChain->Initialize(pSurface, width, height) != 0)
    {
        return -1;
    }
    m_timelineSemaphore = new CVkSemaphore{pDevice,true};
    if (m_timelineSemaphore->Initialize() != 0)
    {
        return -1;
    }
    m_binarySemaphore = new CVkSemaphore{pDevice,false};
    if (m_binarySemaphore->Initialize() != 0)
    {
        return -1;
    }
    m_commandHandler = new CVkCommandPool{pDevice, m_swapChain};
    if (m_commandHandler->Initialize(familyIndex) != 0)
    {
        return -1;
    }
    return 0;
}

void CPresenter::Cleanup() const noexcept
{
    if (m_commandHandler != nullptr)
    {
        m_commandHandler->Release();
    }
    if (m_timelineSemaphore != nullptr)
    {
        m_timelineSemaphore->Release();
    }
    if (m_binarySemaphore != nullptr)
    {
        m_binarySemaphore->Release();
    }
    if (m_swapChain != nullptr)
    {
        m_swapChain->Release();
    }
    if (m_queue[g_graphicsQueueId] != nullptr)
    {
        m_queue[g_graphicsQueueId]->Release();
    }
    if (m_queue[g_computeQueueId] != nullptr)
    {
        m_queue[g_computeQueueId]->Release();
    }
    if (m_queue[g_presentQueueId] != nullptr)
    {
        m_queue[g_presentQueueId]->Release();
    }
}

auto CPresenter::BeginRender() const noexcept -> int
{
    uint64_t currentSemaphoreValue{};
    if (m_timelineSemaphore->Value(currentSemaphoreValue) != 0)
    {
        return -1;
    }
    if (m_timelineSemaphore->Wait(currentSemaphoreValue) != 0)
    {
        return -1;
    }
    if (m_swapChain->NextImage(m_binarySemaphore) != 0)
    {
        return -1;
    }
    if (m_commandHandler->BeginCommand() != 0)
    {
        return -1;
    }
    m_commandHandler->MakeReadyToRender(m_swapChain);
    m_commandHandler->BeginRender();
    return 0;
}

auto CPresenter::EndRender() const noexcept -> int
{
    m_commandHandler->EndRender();
    m_commandHandler->MakeReadyToPresent(m_swapChain);
    if (m_commandHandler->EndCommand() != 0)
    {
        return -1;
    }
    if (m_queue[g_presentQueueId]->Execute(m_timelineSemaphore, m_commandHandler) != 0)
    {
        return -1;
    }
    if (m_queue[g_presentQueueId]->Present(m_swapChain, m_binarySemaphore, m_swapChain->GetImageIndex()) != 0)
    {
        return -1;
    }
    m_swapChain->NextFrame();
    m_queue[g_presentQueueId]->WaitIdle();
    return 0;
}

void CPresenter::SetRenderPass(const float viewX, const float viewY, const float viewW, const float viewH, const uint8_t compareOperator, const bool enableDepthTest, const bool enableWriteDepth, const bool isFrontFaceClockWice) const
{
    m_commandHandler->SetViewport(viewX, viewY, viewW, viewH);
    m_commandHandler->SetDepthCompareOp(compareOperator);
    m_commandHandler->SetDepthTestEnable(enableDepthTest);
    m_commandHandler->SetDepthWriteEnable(enableWriteDepth);
    m_commandHandler->SetFrontFace(isFrontFaceClockWice);
}

void CPresenter::FullscreenEffect(const CShader& shader) const
{
    m_commandHandler->BindShader(shader.Shader());
    m_commandHandler->SetRasterizerDiscardEnable(true);
    m_commandHandler->DrawFullscreen();
}
} // namespace DeerVulkan