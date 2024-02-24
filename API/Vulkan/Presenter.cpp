//
// Created by Joran on 08/01/2024.
//

#include "Presenter.h"

#include "FawnVision_Core.hpp"
#include "Wrapper/VkCommandHandler.h"
#include "Wrapper/VkQueue.h"
#include "Wrapper/VkSemaphore.h"
#include "Wrapper/VkSwapChain.h"

namespace DeerVulkan
{
    int Presenter::Initialize( const CVkDevice* pDevice, const CVkSurface* pSurface, const int32_t familyIndex, const int32_t w, const int32_t h ) noexcept
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
        m_timelineSemaphore = new CVkSemaphore { pDevice };
        if ( m_timelineSemaphore->Initialize( true ) )
        {
            return -1;
        }
        m_binarySemaphore = new CVkSemaphore { pDevice };
        if ( m_binarySemaphore->Initialize( false ) )
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

    void Presenter::Cleanup() const noexcept
    {
        if ( m_commandHandler )
        {
            m_commandHandler->Release();
        }
        if ( m_timelineSemaphore )
        {
            m_timelineSemaphore->Release();
        }
        if ( m_binarySemaphore )
        {
            m_binarySemaphore->Release();
        }
        if ( m_swapChain )
        {
            m_swapChain->Release();
        }
        if ( m_queue )
        {
            m_queue->Release();
        }
    }

    int Presenter::BeginRender() const noexcept
    {
        uint64_t currentSemaphoreValue;
        if ( m_timelineSemaphore->Value( currentSemaphoreValue ) )
        {
            return -1;
        }
        if ( m_timelineSemaphore->Wait( currentSemaphoreValue ) )
        {
            return -1;
        }
        if ( m_swapChain->NextImage( m_binarySemaphore ) )
        {
            return -1;
        }
        if ( m_commandHandler->BeginCommand() )
        {
            return -1;
        }
        m_commandHandler->MakeReadyToRender( m_swapChain );
        m_commandHandler->BeginRender();
        return 0;
    }

    int Presenter::EndRender() const noexcept
    {

        m_commandHandler->EndRender();
        m_commandHandler->MakeReadyToPresent( m_swapChain );
        if ( m_commandHandler->EndCommand() )
        {
            return -1;
        }
        if ( m_queue->Execute( m_timelineSemaphore, m_commandHandler ) )
        {
            return -1;
        }
        if ( m_queue->Present( m_swapChain, m_binarySemaphore, m_swapChain->GetImageIndex() ) )
        {
            return -1;
        }
        m_swapChain->NextFrame();
        m_queue->WaitIdle();
        return 0;
    }
    void Presenter::SetRenderPass(const float viewX, const float viewY, const float viewW, const float viewH, const uint8_t compareOperator, const bool enableDepthTest, const bool enableWriteDepth, const bool isFrontFaceClockWice ) const
    {
        m_commandHandler->SetViewport(viewX, viewY, viewW, viewH);
        m_commandHandler->SetDepthCompareOp( compareOperator );
        m_commandHandler->SetDepthTestEnable( enableDepthTest );
        m_commandHandler->SetDepthWriteEnable( enableWriteDepth );
        m_commandHandler->SetFrontFace( isFrontFaceClockWice );
    }
    void Presenter::FullscreenEffect( const CShader& shader ) const
    {
        m_commandHandler->BindShader( shader.Shader() );
        m_commandHandler->SetRasterizerDiscardEnable( true );
        m_commandHandler->DrawFullscreen();
    }
}// namespace DeerVulkan
