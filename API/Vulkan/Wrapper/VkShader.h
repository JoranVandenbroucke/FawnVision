//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "Base.h"
#include "VkDevice.h"

namespace DeerVulkan
{
struct SShaderData
{
    std::vector<uint8_t> code;
    const char* name{BALBINO_NULL};
    uint32_t stage;
    bool isBinary;
};

class CVkShader final : public CDeviceObject
{
public:
    explicit CVkShader(const CVkDevice* pDevice)
        : CDeviceObject(pDevice)
    {
    }

    ~CVkShader() override
    {
        for (const VkShaderEXT& shader : m_shaders)
        {
            vkDestroyShaderEXT(Device()->VkDevice(), shader, VK_NULL_HANDLE);
        }
    }

    auto Initialize(const std::vector<SShaderData>& shadersData) noexcept -> int32_t
    {
        vkCreateShadersEXT = std::bit_cast<PFN_vkCreateShadersEXT>(vkGetDeviceProcAddr(Device()->VkDevice(), "vkCreateShadersEXT"));
        vkDestroyShaderEXT = std::bit_cast<PFN_vkDestroyShaderEXT>(vkGetDeviceProcAddr(Device()->VkDevice(), "vkDestroyShaderEXT"));
        vkGetShaderBinaryDataEXT = std::bit_cast<PFN_vkGetShaderBinaryDataEXT>(vkGetDeviceProcAddr(Device()->VkDevice(), "vkGetShaderBinaryDataEXT"));

        std::vector<VkShaderCreateInfoEXT> createInfos{};
        createInfos.reserve(shadersData.size());
        m_stages.reserve(shadersData.size());
        m_shaders.resize(shadersData.size());
        for (std::size_t i{}; i < shadersData.size(); ++i)
        {
            const auto& [code, name, shaderType, isBinary]{shadersData[i]};
            const auto stage{static_cast<VkShaderStageFlagBits>(shaderType)};
            const VkShaderStageFlags nextStage{i + 1 < shadersData.size() ? shadersData[i + 1].stage : 0};
            createInfos.push_back(VkShaderCreateInfoEXT{
                    VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT,
                    VK_NULL_HANDLE,
                    VK_SHADER_CREATE_LINK_STAGE_BIT_EXT,
                    stage,
                    nextStage,
                    isBinary ? VK_SHADER_CODE_TYPE_BINARY_EXT : VK_SHADER_CODE_TYPE_SPIRV_EXT,
                    code.size(),
                    code.data(),
                    name,
                    0,
                    VK_NULL_HANDLE,
                    0,
                    VK_NULL_HANDLE,
                    VK_NULL_HANDLE}
                );
            m_stages.emplace_back(stage);
        }
        if (!CheckVkResult(vkCreateShadersEXT(Device()->VkDevice(), static_cast<uint32_t>(createInfos.size()), createInfos.data(), VK_NULL_HANDLE, m_shaders.data())))
        {
            return -1;
        }
        return 0;
    }

    auto GetBinaryData() const noexcept -> std::vector<std::vector<uint8_t>>
    {
        if (m_shaders.empty())
        {
            return {};
        }
        std::vector<std::vector<uint8_t>> binaryData{};
        binaryData.reserve(m_shaders.size());
        for (std::size_t i{}; i < m_shaders.size(); ++i)
        {
            binaryData.push_back(GetBinaryData(i));
        }
        return binaryData;
    }

    auto GetBinaryData(const std::size_t idx = 0) const noexcept -> std::vector<uint8_t>
    {
        if (idx >= m_shaders.size())
        {
            return {};
        }

        size_t dataSize{};
        std::vector<uint8_t> binaryData{};
        vkGetShaderBinaryDataEXT(Device()->VkDevice(), m_shaders[idx], &dataSize, nullptr);
        binaryData.resize(dataSize);
        vkGetShaderBinaryDataEXT(Device()->VkDevice(), m_shaders[idx], &dataSize, binaryData.data());
        return binaryData;
    }

    auto GetShaderCount() const noexcept -> uint32_t
    {
        return static_cast<uint32_t>(m_shaders.size());
    }

    auto GetStages() const noexcept -> const VkShaderStageFlagBits*
    {
        return m_shaders.empty() ? VK_NULL_HANDLE : m_stages.data();
    }

    auto Handle() const noexcept -> const VkShaderEXT*
    {
        return m_shaders.empty() ? VK_NULL_HANDLE : m_shaders.data();
    }

private:
    std::vector<VkShaderEXT> m_shaders;
    std::vector<VkShaderStageFlagBits> m_stages;

    PFN_vkCreateShadersEXT vkCreateShadersEXT{VK_NULL_HANDLE};
    PFN_vkDestroyShaderEXT vkDestroyShaderEXT{VK_NULL_HANDLE};
    PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT{VK_NULL_HANDLE};
};
} // namespace DeerVulkan