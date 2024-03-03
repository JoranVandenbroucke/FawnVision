//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "FawnVision_Core.hpp"

namespace FawnVision
{
/**
 * \brief Creates a new renderer for the specified window.
 * \param[in] window The current Window (input).
 * \param[out] renderer The Renderer that needs to be created (output).
 * \return 0 when successful, -1 otherwise.
 */
auto CreateRenderer(const Window& window, Renderer& renderer) -> int32_t;

/**
 * \brief Releases the resources associated with the given renderer.
 * \param[in,out] renderer The Current Renderer (input and output).
 * \return 0 if successful, -1 otherwise.
 */
auto ReleaseRenderer(Renderer& renderer) -> int32_t;

/**
 * \brief Resizes the render target based on the provided window.
 * \param[in] window The current Window (input).
 * \param[in,out] renderer The Renderer whose target needs to be resized (input and output).
 * \return 0 if successful, -1 otherwise.
 */
auto ResizeRender(const Window& window, const Renderer& renderer) -> int32_t;

/**
 * \brief Initiates the rendering process using the specified renderer.
 * \param[in] renderer The Renderer for rendering (input).
 * \return 0 if successful, -1 otherwise.
 */
auto StartRender(const Renderer& renderer) -> int32_t;

/**
 * \brief Stops the rendering process for the given renderer.
 * \param[in] renderer The Renderer to stop rendering (input).
 * \return 0 if successful, -1 otherwise.
 */
auto StopRender(const Renderer& renderer) -> int32_t;

/**
 * \brief Applies a fullscreen effect during rendering using the specified renderer.
 * \param[in] renderer The Renderer for applying the effect (input).
 * \return 0 if successful, -1 otherwise.
 */
auto RenderFullscreenEffect(const Renderer& renderer) -> int32_t;

/**
 * \brief Sets the rendering passes for the specified renderer.
 * \param[in] renderer The Renderer for which render passes need to be set (input).
 * \param[in] renderPasses A vector containing RenderPassInfo for each rendering pass (input).
 * \return 0 if successful, -1 otherwise.
 */
auto SetRenderPasses(const Renderer& renderer, const std::vector<RenderPassInfo>& renderPasses) -> int32_t;

/**
 * \brief Loads a render pass for the given renderer and checks if it is fullscreen.
 * \param[in] renderer The Renderer for which the render pass is loaded (input).
 * \param[out] isFullscreen Boolean flag indicating whether the loaded render pass is fullscreen (output).
 * \return 0 if successful, -1 otherwise.
 */
auto LoadRenderPass(const Renderer& renderer, bool& isFullscreen) -> int32_t;

/**
 * \brief Moves to the next render pass in the sequence for the specified renderer.
 * \param[in] renderer The Renderer for which the next render pass is selected (input).
 * \param[out] isAtEnd Boolean flag indicating if the renderer is at the end of the render passes (out).
 * \return 0 if successful, -1 otherwise.
 */
auto NextRenderPass(const Renderer& renderer, bool& isAtEnd) -> int32_t;
} // namespace FawnVision
