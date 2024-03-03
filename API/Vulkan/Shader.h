//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "Versions.hpp"
#include "Wrapper/VkShader.h"

namespace DeerVulkan
{
class CShader
{
public:
    auto Initialize(const CVkDevice* pDevice, const std::vector<std::vector<uint8_t>>& shaderCode, const std::vector<std::string_view>& entryNames, const std::vector<uint32_t>& shaderStages, uint32_t areBinary) -> int32_t;

    void Cleanup() const noexcept
    {
        m_shader->Release();
    }

    [[nodiscard]] auto Shader() const -> const CVkShader*
    {
        return m_shader;
    }

private:
    CVkShader* m_shader{BALBINO_NULL};
};
} // namespace DeerVulkan