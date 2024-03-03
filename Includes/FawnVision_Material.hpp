//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "FawnVision_Core.hpp"

namespace FawnVision
{
/**
 * \brief
 * \param[in] createInfo createInfo contains information on how to create the material (input).
 * \param[in] renderer Current Renderer (input).
 * \param[out] material The material that needs to be created (output).
 * \return 0 when successful, -1 otherwise.
 */
auto CreateMaterial(const MaterialCreateInfo& createInfo, const Renderer& renderer, Material& material) -> int32_t;
auto ReleaseMaterial(Material& material) -> int32_t;
} // namespace FawnVision
