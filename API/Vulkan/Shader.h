//
// Created by Joran on 11/02/2024.
//

#pragma once
#include "Wrapper/VkShader.h"

namespace DeerVulkan
{
    class CShader
    {
    public:
        int32_t Initialize( const CVkDevice* pDevice, const std::vector<std::vector<uint8_t>>& shaderCode, const std::vector<std::string_view>& entryNames, const std::vector<uint32_t>& shaderStages, uint32_t areBinary );
        void Cleanup() const noexcept
        {
            m_shader->Release();
        }
        const CVkShader* Shader() const
        {
            return m_shader;
        }

    private:
        CVkShader* m_shader { BALBINO_NULL };
    };
}// namespace DeerVulkan
