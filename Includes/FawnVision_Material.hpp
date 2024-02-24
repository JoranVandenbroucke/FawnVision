//
// Created by Joran on 22/02/2024.
//

#pragma once
#include "FawnVision_Core.hpp"

namespace FawnVision
{
    int32_t CreateMaterial( const MaterialCreateInfo& createInfo, const Renderer& renderer, Material& material);
    int32_t ReleaseMaterial( Material& material );
}
