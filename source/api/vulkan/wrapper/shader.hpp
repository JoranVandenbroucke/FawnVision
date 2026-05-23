#pragma once
#include "../deer_vulkan_core.hpp"
#include "descriptor.hpp"
#include "device.hpp"

namespace deer_vulkan
{
struct Shader
{
    std::vector<vk::ShaderEXT> shaders{};
    std::vector<vk::ShaderStageFlagBits> stages{};
};

template <std::size_t N = std::dynamic_extent>
[[nodiscard]] auto Initialize(const Dispatch& dispatch, const Device& device, const std::span<const std::uint32_t, N> stages,
                              const std::span<const std::vector<std::uint8_t>, N> codes, const Descriptor& descriptor, Shader& shader) noexcept -> vk_status
{
    const auto count = static_cast<std::uint32_t>(stages.size());

    // Descriptor layout — null handle means no layout (push constants only etc.)
    const bool hasLayout                   = descriptor.descriptorSetLayout != nullptr;
    const std::uint32_t layoutCount        = hasLayout ? 1U : 0U;
    const vk::DescriptorSetLayout* pLayout = hasLayout ? &descriptor.descriptorSetLayout : nullptr;

    std::vector<vk::ShaderCreateInfoEXT> createInfos;
    createInfos.reserve(count);
    shader.stages.reserve(count);
    shader.shaders.resize(count, nullptr);

    for (std::size_t idx = 0; idx < count; ++idx)
    {
        const auto stage     = static_cast<vk::ShaderStageFlagBits>(stages[idx]);
        const auto nextStage = static_cast<vk::ShaderStageFlags>(idx + 1U < stages.size() ? stages[idx + 1U] : 0U);

        createInfos.push_back(vk::ShaderCreateInfoEXT{
            .sType                  = vk::StructureType::eShaderCreateInfoEXT,
            .pNext                  = nullptr,
            .flags                  = vk::ShaderCreateFlagBitsEXT::eLinkStage,
            .stage                  = stage,
            .nextStage              = nextStage,
            .codeType               = vk::ShaderCodeTypeEXT::eSpirv, // todo : have a binary version of this
            .codeSize               = codes[idx].size(),
            .pCode                  = codes[idx].data(),
            .pName                  = "main",
            .setLayoutCount         = layoutCount,
            .pSetLayouts            = pLayout,
            .pushConstantRangeCount = 0U,
            .pPushConstantRanges    = nullptr,
            .pSpecializationInfo    = nullptr,
        });

        shader.stages.push_back(stage);
    }

    const vk_status status = FromVkResult(device.device.createShadersEXT(count, createInfos.data(), nullptr, shader.shaders.data(), dispatch.dispatch));
    if (IsError(status)) [[unlikely]]
    {
        for (const vk::ShaderEXT shaderExt : shader.shaders)
        {
            if (shaderExt)
            {
                device.device.destroyShaderEXT(shaderExt, nullptr, dispatch.dispatch);
            }
        }
        shader.shaders.clear();
        shader.stages.clear();
    }

    return status;
}

inline auto Cleanup(const Dispatch& dispatch, const Device& device, Shader& shader) noexcept -> void
{
    for (const vk::ShaderEXT pShader : shader.shaders)
    {
        device.device.destroyShaderEXT(pShader, nullptr, dispatch.dispatch);
    }

    shader.shaders.clear();
    shader.stages.clear();
}
} // namespace deer_vulkan
