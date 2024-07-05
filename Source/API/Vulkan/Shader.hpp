//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "Wrapper/VkShader.hpp"

namespace DeerVulkan
{
class CShader
{
public:
    explicit CShader(const CVkDevice& pDevice): m_shader{pDevice}{}
    auto Initialize(const std::vector<std::vector<uint8_t>>& shaderCode, const std::vector<std::string_view>& entryNames, const std::vector<uint32_t>& shaderStages, uint32_t areBinary) -> int32_t;


    [[nodiscard]] auto Shader() const -> const CVkShader&
    {
        return m_shader;
    }

private:
    CVkShader m_shader;
};
} // namespace DeerVulkan