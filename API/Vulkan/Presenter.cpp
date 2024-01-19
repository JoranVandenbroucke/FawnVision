//
// Created by Joran on 08/01/2024.
//

#include "Presenter.h"

#include "Wrapper/VkCommandHandler.h"
#include "Wrapper/VkFence.h"
#include "Wrapper/VkQueue.h"
#include "Wrapper/VkSemaphore.h"
#include "Wrapper/VkSwapChain.h"

namespace DeerVulkan
{
    int Presenter::Initialize( CVkDevice* pDevice, const CVkSurface* pSurface, const int32_t familyIndex, const int32_t w, const int32_t h ) noexcept
    {
        m_queue = new CVkQueue { pDevice };
        if ( m_queue->Initialize( familyIndex ) )
        {
            return -1;
        }
        m_swapChain = new CVkSwapChain { pDevice };
        if ( m_swapChain->Initialize( pSurface, w, h ) )
        {
            return -1;
        }
        const int imageCount { m_swapChain->GetImageCount() };
        m_renderSemaphore = new CVkSemaphore { pDevice };
        if ( m_renderSemaphore->Initialize( imageCount ) )
        {
            return -1;
        }
        m_imageSemaphore = new CVkSemaphore { pDevice };
        if ( m_imageSemaphore->Initialize( imageCount ) )
        {
            return -1;
        }
        m_fence = new CVkFence { pDevice };
        if ( m_fence->Initialize( imageCount ) )
        {
            return -1;
        }
        m_commandHandler = new CVkCommandHandler { pDevice };
        if ( m_commandHandler->Initialize( m_swapChain, familyIndex ) )
        {
            return -1;
        }
        return 0;
    }

    void Presenter::Cleanup() noexcept
    {
        if ( m_commandHandler )
        {
            m_commandHandler->Release();
        }
        if ( m_fence )
        {
            m_fence->Release();
        }
        if ( m_renderSemaphore )
        {
            m_renderSemaphore->Release();
        }
        if ( m_imageSemaphore )
        {
            m_imageSemaphore->Release();
        }
        if ( m_swapChain )
        {
            m_swapChain->Release();
        }
        if ( m_queue )
        {
            m_queue->Release();
        }
        m_fence = BALBINO_NULL;
    }

    int Presenter::BeginRender() const noexcept
    {
        const uint32_t frameIndex { m_swapChain->GetFrameIndex() };
        if ( m_fence->Wait( frameIndex ) )
        {
            return -1;
        }
        if ( m_swapChain->NextImage( m_imageSemaphore ) )
        {
            return -1;
        }
        if ( m_commandHandler->BeginCommand() )
        {
            return -1;
        }
        m_swapChain->MakeReadyToRender( m_commandHandler );
        m_commandHandler->BeginRender();
        return 0;
    }

    int Presenter::EndRender() const noexcept
    {
        const uint32_t frameIndex { m_swapChain->GetFrameIndex() };
        if ( m_fence->Wait( frameIndex ) )
        {
            return -1;
        }
        if ( m_fence->Reset( frameIndex ) )
        {
            return -1;
        }
        m_commandHandler->EndRender();
        m_swapChain->MakeReadyToPresent( m_commandHandler );
        if ( m_commandHandler->EndCommand() )
        {
            return -1;
        }
        uint32_t flags { 0x00000400 };
        if ( m_queue->Execute( m_commandHandler, m_imageSemaphore, m_renderSemaphore, m_fence, frameIndex, &flags ) )
        {
            return -1;
        }
        if ( m_queue->Present( m_swapChain, m_renderSemaphore, frameIndex ) )
        {
            return -1;
        }
        m_queue->WaitIdle();
        return 0;
    }
}// namespace DeerVulkan
