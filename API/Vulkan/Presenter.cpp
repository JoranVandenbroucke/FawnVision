//
// Copyright (c) 2024.
// Author: Joran
//

#include "Presenter.h"

#include "FawnVision_Core.hpp"
#include "Wrapper/VkCommandHandler.h"
#include "Wrapper/VkQueue.h"
#include "Wrapper/VkSemaphore.h"
#include "Wrapper/VkSwapChain.h"

namespace DeerVulkan
{
auto CPresenter::Initialize(const CVkDevice* pDevice, const CVkSurface* pSurface, const int32_t familyIndex, const int32_t width, const int32_t height) noexcept -> int
{
    m_queue = new CVkQueue{pDevice};
    if (m_queue->Initialize(familyIndex) != 0)
    {
        return -1;
    }
    m_swapChain = new CVkSwapChain{pDevice};
    if (m_swapChain->Initialize(pSurface, width, height) != 0)
    {
        return -1;
    }
    m_timelineSemaphore = new CVkSemaphore{pDevice};
    if (m_timelineSemaphore->Initialize(true) != 0)
    {
        return -1;
    }
    m_binarySemaphore = new CVkSemaphore{pDevice};
    if (m_binarySemaphore->Initialize(false) != 0)
    {
        return -1;
    }
    m_commandHandler = new CVkCommandHandler{pDevice};
    if (m_commandHandler->Initialize(m_swapChain, familyIndex) != 0)
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
    if (m_queue != nullptr)
    {
        m_queue->Release();
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
    if (m_queue->Execute(m_timelineSemaphore, m_commandHandler) != 0)
    {
        return -1;
    }
    if (m_queue->Present(m_swapChain, m_binarySemaphore, m_swapChain->GetImageIndex()) != 0)
    {
        return -1;
    }
    m_swapChain->NextFrame();
    m_queue->WaitIdle();
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