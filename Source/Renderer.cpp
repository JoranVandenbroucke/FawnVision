//
// Copyright (c) 2023.
// Author: Joran
//

#include "FawnVision_Renderer.hpp"

#include "Error.hpp"
#include "ObjectDefinitions.hpp"

namespace FawnVision
{
auto InitializeRenderer(const Window& window, const Renderer& renderer) -> int32_t
{
    if (const int32_t returnValue{renderer->instance.Initialize(window->pWindow, "Fixme", 0U, window->extensions, window->extensionCount)}; returnValue != 0)
    {
        switch (returnValue)
        {
        default: SetError(error_code::generic, "The Renderer could not be initialized");
            break;
        case -1: SetError(error_code::generic, "The Renderer Instance could not be initialized");
            break;
        case -2: SetError(error_code::generic, "The Renderer Surface could not be created");
            break;
        case -3: SetError(error_code::generic, "No valid family found");
            break;
        case -4: SetError(error_code::generic, "Could not Create or Initialize the Device");
            break;
        }
        return -1;
    }
    if (renderer->instance.CreatePresentor(renderer->presenter, window->width, window->height) != 0)
    {
        SetError(error_code::generic, "The Renderer could not create a presentor");
        return -1;
    }
    return 0;
}

auto CreateRenderer(const Window& window, Renderer& renderer) -> int32_t
{
    if (renderer != nullptr)
    {
        SetError(error_code::generic, "The Renderer could not be initialized");
        return -1;
    }
    if (window == nullptr)
    {
        SetError(error_code::generic, "The Renderer could not be initialized");
        return -1;
    }
    renderer = new(std::nothrow) Renderer_T{};
    if (renderer == nullptr)
    {
        SetError(error_code::out_of_memory, "");
        return -1;
    }
    if (InitializeRenderer(window, renderer) != 0)
    {
        return -1;
    }
    return 0;
}

auto ReleaseRenderer(Renderer& renderer) -> int32_t
{
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "The Renderer is not be initialized");
        return -1;
    }
    renderer->presenter.Cleanup();
    renderer->instance.Cleanup();

    delete renderer;
    renderer = nullptr;
    return 0;
}

auto ResizeRender(const Window& window, const Renderer& renderer) -> int32_t
{
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "The Renderer is not be initialized");
        return -1;
    }
    if (renderer->instance.RecreatePresentor(renderer->presenter, window->width, window->height) != 0)
    {
        SetError(error_code::generic, "Failed to resize the renderer: %d x %d", window->width, window->height);
        return -1;
    }
    return 0;
}

auto StartRender(const Renderer& renderer) -> int32_t
{
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "The Renderer is not be initialized");
        return -1;
    }
    if (renderer->presenter.BeginRender() != 0)
    {
        SetError(error_code::generic, "Could not begin start the frame");
        return -1;
    }
    renderer->currentRenderPassIndex = 0U;
    return 0;
}

auto StopRender(const Renderer& renderer) -> int32_t
{
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "The Renderer is not be initialized");
        return -1;
    }
    if (renderer->presenter.EndRender() != 0)
    {
        SetError(error_code::generic, "Could not present the frame");
        return -1;
    }
    return 0;
}

auto RenderFullscreenEffect(const Renderer& renderer) -> int32_t
{
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "The Renderer is not be initialized");
        return -1;
    }
    renderer->presenter.FullscreenEffect(renderer->renderPasses[renderer->currentRenderPassIndex].material->shader->shader);
    return 0;
}

auto SetRenderPasses(const Renderer& renderer, const std::vector<RenderPassInfo>& renderPasses) -> int32_t
{
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "The Renderer is not be initialized");
        return -1;
    }
    renderer->renderPasses = renderPasses;
    return 0;
}

auto LoadRenderPass(const Renderer& renderer, bool& isFullscreen) -> int32_t
{
    isFullscreen = false;
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "The Renderer is not be initialized");
        return -1;
    }
    if (renderer->renderPasses.empty())
    {
        return 0;
    }

    const auto& [viewport,
        shader,
        depthSettings,
        renderPassType,
        isFrontFaceClockWice]{
        renderer->renderPasses[renderer->currentRenderPassIndex]};
    renderer->presenter.SetRenderPass(viewport.xPos, viewport.yPos, viewport.width, viewport.height, static_cast<uint8_t>(depthSettings.depthCompareOp), depthSettings.enableDepthTest, depthSettings.enableWriteDepth, isFrontFaceClockWice);
    isFullscreen = renderer->renderPasses[renderer->currentRenderPassIndex].renderPassType == RenderPassInfo::render_pass_type::fullscreen_shader;
    return 0;
}

auto NextRenderPass(const Renderer& renderer, bool& isAtEnd) -> int32_t
{
    isAtEnd = false;
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "The Renderer is not be initialized");
        return -1;
    }
    if (renderer->renderPasses.empty())
    {
        isAtEnd = true;
        return 0;
    }
    if (++renderer->currentRenderPassIndex == renderer->renderPasses.size())
    {
        isAtEnd = true;
    }
    return 0;
}
} // namespace FawnVision