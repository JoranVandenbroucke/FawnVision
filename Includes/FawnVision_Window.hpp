//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "FawnVision_Core.hpp"

namespace FawnVision
{
constexpr int32_t g_centerPosition{0x2FFF0000U};
/**
 * \brief Creates a new window based on the provided creation information.
 * \param[in] createInfo The WindowCreateInfo containing information for window creation (input).
 * \param[out] window The created Window object (output).
 * \return 0 when successful, -1 otherwise.
 */
auto CreateWindow(const WindowCreateInfo& createInfo, Window& window) noexcept -> int32_t;

/**
 * \brief Releases the resources associated with the given window.
 * \param[in,out] window The Window to be released (input and output).
 * \return 0 if successful, -1 otherwise.
 */
auto ReleaseWindow(Window& window) noexcept -> int32_t;

/**
 * \brief Sets the specified flags for the given window.
 * \param[in] window The target Window (input).
 * \param[in] flags The flags to be set (input).
 * \return 0 if successful, -1 otherwise.
 */
auto SetWindowFlags(const Window& window, uint32_t flags) noexcept -> int32_t;

/**
 * \brief Toggles the specified flags for the given window.
 * \param[in] window The target Window (input).
 * \param[in] flags The flags to be toggled (input).
 * \return 0 if successful, -1 otherwise.
 */
auto ToggleWindowFlags(const Window& window, uint32_t flags) noexcept -> int32_t;

/**
 * \brief Sets the size of the given window.
 * \param[in] window The target Window (input).
 * \param[in] width The width to set (input).
 * \param[in] height The height to set (input).
 * \return 0 if successful, -1 otherwise.
 */
auto SetWindowSize(const Window& window, int32_t width, int32_t height) noexcept -> int32_t;

/**
 * \brief Retrieves the size of the given window.
 * \param[in] window The target Window (input).
 * \param[out] width The width of the window (output).
 * \param[out] height The height of the window (output).
 * \return 0 if successful, -1 otherwise.
 */
auto GetWindowSize(const Window& window, int32_t& width, int32_t& height) noexcept -> int32_t;

/**
 * \brief Sets the position of the given window.
 * \param[in] window The target Window (input).
 * \param[in] xPosition The x-coordinate of the position (input).
 * \param[in] yPosition The y-coordinate of the position (input).
 * \return 0 if successful, -1 otherwise.
 */
auto SetWindowPosition(const Window& window, int32_t xPosition = g_centerPosition | 0, int32_t yPosition = g_centerPosition | 0) noexcept -> int32_t;

/**
 * \brief Retrieves the position of the given window.
 * \param[in] window The target Window (input).
 * \param[out] xPosition The x-coordinate of the position (output).
 * \param[out] yPosition The y-coordinate of the position (output).
 * \return 0 if successful, -1 otherwise.
 */
auto GetWindowPosition(const Window& window, int32_t& xPosition, int32_t& yPosition) noexcept -> int32_t;
} // namespace FawnVision
