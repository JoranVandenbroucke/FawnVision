//
// Created by joran on 30/12/2023.
//

#include "FawnVision_Core.h"
#include "ObjectDefenitions.h"

namespace FawnVision
{

    error_code InitializeRenderer( const Window& window, const Renderer& renderer )
    {
        if ( renderer->instance.Initialize( window->pWindow, "Fixme", 0u, window->extensions, window->extensionCount ) )
        {
            return error_code::not_initializable;
        }
        if ( renderer->instance.CreatePresentor( renderer->presenter, window->width, window->height ) )
        {
            return error_code::not_initializable;
        }
        return error_code::oke;
    }

    error_code CreateRenderer( const Window& window, Renderer& renderer )
    {
        if ( renderer )
        {
            return error_code::already_initialized;
        }
        if ( !window )
        {
            return error_code::not_initialized;
        }
        renderer = new ( std::nothrow ) Renderer_T {};
        if ( !renderer )
        {
            return error_code::out_of_memory;
        }
        if ( const error_code errorCode = InitializeRenderer( window, renderer ); errorCode != error_code::oke )
        {
            return errorCode;
        }
        return error_code::oke;
    }
    error_code ReleaseRenderer( Renderer& renderer )
    {
        if ( !renderer )
        {
            return error_code::not_initialized;
        }
        renderer->presenter.Cleanup();
        renderer->instance.Cleanup();

        delete renderer;
        renderer = nullptr;
        return error_code::oke;
    }
    error_code ResizeRender( const Window& window, const Renderer& renderer )
    {
        if ( renderer->instance.RecreatePresentor( renderer->presenter, window->width, window->height ) )
        {
            return error_code::not_initializable;
        }
        return error_code::oke;
    }
    error_code StartRender( const Renderer& renderer )
    {
        if ( !renderer )
        {
            return error_code::not_initialized;
        }
        if ( renderer->presenter.BeginRender() )
        {
            return error_code::begin_render_failed;
        }
        return error_code::oke;
    }
    error_code StopRender( const Renderer& renderer )
    {
        if ( !renderer )
        {
            return error_code::not_initialized;
        }
        if ( renderer->presenter.EndRender() )
        {
            return error_code::end_render_failed;
        }
        return error_code::oke;
    }

    const char* GetRenderError()
    {
        return "no idea what happoned :p\n check the error code. I still have to implement saving the error so that I can process it here";
    }
}// namespace FawnVision
