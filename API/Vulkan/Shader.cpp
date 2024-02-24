//
// Created by Joran on 11/02/2024.
//

#include "Shader.h"

namespace DeerVulkan
{
    int32_t CShader::Initialize( const CVkDevice* pDevice, const std::vector<std::vector<uint8_t>>& shaderCode, const std::vector<std::string_view>& entryNames, const std::vector<uint32_t>& shaderStages, const uint32_t areBinary )
    {
        std::vector<ShaderData> shaderDatas { shaderCode.size() };
        for ( std::size_t i {}; i < shaderCode.size(); ++i )
        {
            auto& [ code, name, stage, isBinary ] { shaderDatas[ i ] };
            code     = shaderCode[ i ];
            name     = entryNames[ i ].data();
            stage    = shaderStages[ i ];
            isBinary = areBinary & 1 << i;
        }
        m_shader = new CVkShader { pDevice };
        return m_shader->Initialize( shaderDatas );
    }
}// namespace DeerVulkan
