//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "FawnVision_Core.hpp"

namespace FawnVision
{
auto CreateShader(const std::vector<ShaderCreateInfo>& createInfo, const Renderer& renderer, Shader& shader) -> int32_t;
auto ReleaseShader(Shader& shader) -> int32_t;
} // namespace FawnVision
