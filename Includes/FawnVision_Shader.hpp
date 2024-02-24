//
// Created by Joran on 20/02/2024.
//

#pragma once
#include "FawnVision_Core.hpp"

namespace FawnVision
{
    int32_t CreateShader( const std::vector<ShaderCreateInfo>& createInfo, const Renderer& renderer, Shader& shader );
    int32_t ReleaseShader( Shader& shader );
}
