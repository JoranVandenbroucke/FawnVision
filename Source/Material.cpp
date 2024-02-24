//
// Created by Joran on 22/02/2024.
//
#include "FawnVision_Material.hpp"

#include "Error.hpp"
#include "ObjectDefinitions.hpp"

namespace FawnVision
{
    int32_t CreateMaterial( const MaterialCreateInfo& createInfo, const Renderer& renderer, Material& material)
    {
        if ( material != nullptr )
        {
            SetError( error_code::generic, "Shader is already created" );
            return -1;
        }
        if ( renderer == nullptr )
        {
            SetError( error_code::generic, "Renderer is not initialized" );
            return -1;
        }

        if ( createInfo.shader == BALBINO_NULL )
        {
            SetError( error_code::generic, "Material does not have a shader to base on" );
            return -1;
        }
        material = new ( std::nothrow ) Material_T{};
        if ( material == nullptr )
        {
            SetError( error_code::out_of_memory, "" );
            return -1;
        }
        material->shader = createInfo.shader;
        material->primitiveTopology = static_cast<uint32_t>( createInfo.primitiveTopology );

#pragma todo("Calculate Discriptor Set")
        return 0;
    }
    int32_t ReleaseMaterial( Material& material )
    {
        if ( material == nullptr )
        {
            SetError( error_code::generic, "Shader is not initialized" );
            return -1;
        }
        // material->material.Cleanup();
        delete material;
        material = nullptr;
        return 0;
    }
}
