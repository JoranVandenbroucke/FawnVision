//
// Copyright (c) 2024.
// Author: Joran
//
#include "FawnVision_Material.hpp"

#include "Error.hpp"
#include "ObjectDefinitions.hpp"

namespace FawnVision
{
auto CreateMaterial(const MaterialCreateInfo& createInfo, const Renderer& renderer, Material& material) -> int32_t
{
    if (material != nullptr)
    {
        SetError(error_code::generic, "Shader is already created");
        return -1;
    }
    if (renderer == nullptr)
    {
        SetError(error_code::generic, "Renderer is not initialized");
        return -1;
    }

    if (createInfo.shader == BALBINO_NULL)
    {
        SetError(error_code::generic, "Material does not have a shader to base on");
        return -1;
    }
    material = new(std::nothrow) Material_T{};
    if (material == nullptr)
    {
        SetError(error_code::out_of_memory, "");
        return -1;
    }
    material->shader = createInfo.shader;
    material->primitiveTopology = static_cast<uint32_t>(createInfo.primitiveTopology);

#pragma todo("Calculate Discriptor Set")
    return 0;
}

auto ReleaseMaterial(Material& material) -> int32_t
{
    if (material == nullptr)
    {
        SetError(error_code::generic, "Shader is not initialized");
        return -1;
    }
    // material->material.Cleanup();
    delete material;
    material = nullptr;
    return 0;
}
} // namespace FawnVision