//
// Copyright (c) 2024.
// Author: Joran.
//

module;

#include "API/Vulkan/DeerVulkan.hpp"

#include <cstdint>
#include <vector>

export module FawnVision.Shader;
import FawnVision.RendererTypes;

namespace FawnVision
{
struct SShaderData
{
    uint32_t stage;
    std::vector<uint8_t> code;
};
export using ShaderCreateInfo = std::vector<SShaderData>;
export struct SShader
{
    DeerVulkan::SVKShader shader;
};

export auto CreateShader(const SRenderer& renderer, const ShaderCreateInfo& shaderCreateinfo, SShader& shader)
{
    std::vector<uint32_t> stages(shaderCreateinfo.size(), 0);
    std::vector<std::vector<uint8_t>> codes(shaderCreateinfo.size());

    for (std::size_t i = 0; i < shaderCreateinfo.size(); ++i)
    {
        stages[i] = shaderCreateinfo[i].stage;
        codes[i] = shaderCreateinfo[i].code;
    }

    if (InitializeShader(renderer.device, stages, codes, shader.shader) != 0)
    {
        return -1;
    }
    return 0;
}
inline void Cleanup(const SRenderer& renderer, SShader& shader)
{
    Cleanup(renderer.device, shader.shader);
}
} // namespace FawnVision