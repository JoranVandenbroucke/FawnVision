//
// Created by Joran on 27/01/2024.
//
#include "FawnVision_Shader.hpp"

#include "Error.hpp"
#include "ObjectDefinitions.hpp"

namespace FawnVision
{
    int32_t CreateShader( const std::vector<ShaderCreateInfo>& createInfo, const Renderer& renderer, Shader& shader )
    {
        if ( shader != nullptr )
        {
            SetError( error_code::generic, "Shader is already created" );
            return -1;
        }
        if ( renderer == nullptr )
        {
            SetError( error_code::generic, "Renderer is not initialized" );
            return -1;
        }

        shader = new ( std::nothrow ) Shader_T {};
        if ( shader == nullptr )
        {
            SetError( error_code::out_of_memory, "" );
            return -1;
        }
        std::vector<std::vector<uint8_t>> shaderCode( createInfo.size() );
        std::vector<std::string_view> entryNames( createInfo.size() );
        std::vector<uint32_t> shaderStages( createInfo.size() );
        uint32_t areBinary {};
        for ( std::size_t index {}; index < createInfo.size(); ++index )
        {
            const auto& [ code, name, shaderType, isBinary ] { createInfo[ index ] };
            shaderCode[ index ]    = code;
            entryNames[ index ]    = name;
            shaderStages[ index ]  = static_cast<uint32_t>( shaderType );
            areBinary             |= static_cast<uint32_t>( isBinary ) & 1U << index;
        }

        if ( shader->shader.Initialize( renderer->presenter.GetCommandHandler()->Device(), shaderCode, entryNames, shaderStages, areBinary ) != 0 )
        {
            SetError( error_code::generic, "Shader failed to initialize" );
            return -1;
        }
        return 0;
    }
    int32_t ReleaseShader( Shader& shader )
    {
        if ( shader == nullptr )
        {
            SetError( error_code::generic, "Shader is not initialized" );
            return -1;
        }
        shader->shader.Cleanup();
        delete shader;
        shader = nullptr;
        return 0;
    }
}// namespace FawnVision
