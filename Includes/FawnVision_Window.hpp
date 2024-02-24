//
// Created by Joran on 20/02/2024.
//

#pragma once
#include "FawnVision_Core.hpp"

namespace FawnVision
{
    /**
     * \brief Creates a new window based on the provided creation information.
     * \param[in] createInfo The WindowCreateInfo containing information for window creation (input).
     * \param[out] window The created Window object (output).
     * \return 0 when successful, -1 otherwise.
     */
    int32_t CreateWindow( const WindowCreateInfo& createInfo, Window& window ) noexcept;

    /**
     * \brief Releases the resources associated with the given window.
     * \param[in,out] window The Window to be released (input and output).
     * \return 0 if successful, -1 otherwise.
     */
    int32_t ReleaseWindow( Window& window ) noexcept;

    /**
     * \brief Sets the specified flags for the given window.
     * \param[in] window The target Window (input).
     * \param[in] flags The flags to be set (input).
     * \return 0 if successful, -1 otherwise.
     */
    int32_t SetWindowFlags( const Window& window, uint32_t flags ) noexcept;

    /**
     * \brief Toggles the specified flags for the given window.
     * \param[in] window The target Window (input).
     * \param[in] flags The flags to be toggled (input).
     * \return 0 if successful, -1 otherwise.
     */
    int32_t ToggleWindowFlags( const Window& window, uint32_t flags ) noexcept;

    /**
     * \brief Sets the size of the given window.
     * \param[in] window The target Window (input).
     * \param[in] width The width to set (input).
     * \param[in] height The height to set (input).
     * \return 0 if successful, -1 otherwise.
     */
    int32_t SetWindowSize( const Window& window, int32_t width, int32_t height ) noexcept;

    /**
     * \brief Retrieves the size of the given window.
     * \param[in] window The target Window (input).
     * \param[out] w The width of the window (output).
     * \param[out] h The height of the window (output).
     * \return 0 if successful, -1 otherwise.
     */
    int32_t GetWindowSize( const Window& window, int32_t& w, int32_t& h ) noexcept;

    /**
     * \brief Sets the position of the given window.
     * \param[in] window The target Window (input).
     * \param[in] x The x-coordinate of the position (input).
     * \param[in] y The y-coordinate of the position (input).
     * \return 0 if successful, -1 otherwise.
     */
    int32_t SetWindowPosition( const Window& window, int32_t x = 0x2FFF0000U | 0, int32_t y = 0x2FFF0000U | 0 ) noexcept;

    /**
     * \brief Retrieves the position of the given window.
     * \param[in] window The target Window (input).
     * \param[out] xPosition The x-coordinate of the position (output).
     * \param[out] yPosition The y-coordinate of the position (output).
     * \return 0 if successful, -1 otherwise.
     */
    int32_t GetWindowPosition( const Window& window, int32_t& xPosition, int32_t& yPosition ) noexcept;
}// namespace FawnVision
