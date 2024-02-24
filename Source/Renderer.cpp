//
// Created by joran on 30/12/2023.
//

#include "FawnVision_Renderer.hpp"

#include "Error.hpp"
#include "ObjectDefinitions.hpp"

namespace FawnVision
{
    int32_t InitializeRenderer( const Window& window, const Renderer& renderer )
    {
        if ( const int32_t returnValue { renderer->instance.Initialize( window->pWindow, "Fixme", 0U, window->extensions, window->extensionCount ) }; returnValue != 0 )
        {
            switch ( returnValue )
            {
                default:
                    SetError( error_code::generic, "The Renderer could not be initialized" );
                    break;
                case -1:
                    SetError( error_code::generic, "The Renderer Instance could not be initialized" );
                    break;
                case -2:
                    SetError( error_code::generic, "The Renderer Surface could not be created" );
                    break;
                case -3:
                    SetError( error_code::generic, "No valid family found" );
                    break;
                case -4:
                    SetError( error_code::generic, "Could not Create or Initialize the Device" );
                    break;
            }
            return -1;
        }
        if ( renderer->instance.CreatePresentor( renderer->presenter, window->width, window->height ) )
        {
            SetError( error_code::generic, "The Renderer could not create a presentor" );
            return -1;
        }
        return 0;
    }

    int32_t CreateRenderer( const Window& window, Renderer& renderer )
    {
        if ( renderer )
        {
            SetError( error_code::generic, "The Renderer could not be initialized" );
            return -1;
        }
        if ( !window )
        {
            SetError( error_code::generic, "The Renderer could not be initialized" );
            return -1;
        }
        renderer = new ( std::nothrow ) Renderer_T {};
        if ( !renderer )
        {
            SetError( error_code::out_of_memory, "" );
            return -1;
        }
        if ( InitializeRenderer( window, renderer ) )
        {
            return -1;
        }
        return 0;
    }
    int32_t ReleaseRenderer( Renderer& renderer )
    {
        if ( !renderer )
        {
            SetError( error_code::generic, "The Renderer is not be initialized" );
            return -1;
        }
        renderer->presenter.Cleanup();
        renderer->instance.Cleanup();

        delete renderer;
        renderer = nullptr;
        return 0;
    }
    int32_t ResizeRender( const Window& window, const Renderer& renderer )
    {
        if ( !renderer )
        {
            SetError( error_code::generic, "The Renderer is not be initialized" );
            return -1;
        }
        if ( renderer->instance.RecreatePresentor( renderer->presenter, window->width, window->height ) )
        {
            SetError( error_code::generic, "Failed to resize the renderer: %d x %d", window->width, window->height );
            return -1;
        }
        return 0;
    }
    int32_t StartRender( const Renderer& renderer )
    {
        if ( !renderer )
        {
            SetError( error_code::generic, "The Renderer is not be initialized" );
            return -1;
        }
        if ( renderer->presenter.BeginRender() )
        {
            SetError( error_code::generic, "Could not begin start the frame" );
            return -1;
        }
        renderer->currentRenderPassIndex = 0U;
        return 0;
    }
    int32_t StopRender( const Renderer& renderer )
    {
        if ( !renderer )
        {
            SetError( error_code::generic, "The Renderer is not be initialized" );
            return -1;
        }
        if ( renderer->presenter.EndRender() )
        {
            SetError( error_code::generic, "Could not present the frame" );
            return -1;
        }
        return 0;
    }
    int32_t RenderFullscreenEffect( const Renderer& renderer )
    {
        if ( !renderer )
        {
            SetError( error_code::generic, "The Renderer is not be initialized" );
            return -1;
        }
        renderer->presenter.FullscreenEffect( renderer->renderPasses[ renderer->currentRenderPassIndex ].material->shader->shader );
        return 0;
    }

    int32_t SetRenderPasses( const Renderer& renderer, const std::vector<RenderPassInfo>& renderPasses )
    {
        if ( !renderer )
        {
            SetError( error_code::generic, "The Renderer is not be initialized" );
            return -1;
        }
        renderer->renderPasses = renderPasses;
        return 0;
    }
    int32_t LoadRenderPass( const Renderer& renderer, bool& isFullscreen )
    {
        isFullscreen = false;
        if ( !renderer )
        {
            SetError( error_code::generic, "The Renderer is not be initialized" );
            return -1;
        }
        if ( renderer->renderPasses.empty() )
        {
            return 0;
        }

        const auto& [ viewport,
                      shader,
                      depthSettings,
                      renderPassType,
                      isFrontFaceClockWice ] {
            renderer->renderPasses[ renderer->currentRenderPassIndex ] };
        renderer->presenter.SetRenderPass( viewport.xPos, viewport.yPos, viewport.width, viewport.height, static_cast<uint8_t>( depthSettings.depthCompareOp ), depthSettings.enableDepthTest, depthSettings.enableWriteDepth, isFrontFaceClockWice );
        isFullscreen = renderer->renderPasses[ renderer->currentRenderPassIndex ].renderPassType == RenderPassInfo::render_pass_type::fullscreen_shader;
        return 0;
    }
    int32_t NextRenderPass( const Renderer& renderer, bool& isAtEnd )
    {
        isAtEnd = false;
        if ( !renderer )
        {
            SetError( error_code::generic, "The Renderer is not be initialized" );
            return -1;
        }
        if ( renderer->renderPasses.empty() )
        {
            isAtEnd = true;
            return 0;
        }
        if ( ++renderer->currentRenderPassIndex == renderer->renderPasses.size() )
        {
            isAtEnd = true;
        }
        return 0;
    }
}// namespace FawnVision
