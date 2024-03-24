//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "VkDevice.hpp"

namespace DeerVulkan
{
struct SShaderData
{
    std::vector<uint8_t> code;
    const char* name{BALBINO_NULL};
    uint32_t stage;
    bool isBinary;
};

class CVkShader final
{
  public:
    BALBINO_CONSTEXPR_SINCE_CXX20 BALBINO_EXPLICIT_SINCE_CXX11 CVkShader(const CVkDevice* pDevice);
    BALBINO_CONSTEXPR_SINCE_CXX20 ~CVkShader();
    CVkShader(const CVkShader& other)                    = delete;
    CVkShader(CVkShader&& other) noexcept                = delete;
    auto operator=(const CVkShader& other) -> CVkShader& = delete;
    auto operator=(CVkShader&& other) -> CVkShader&      = delete;

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Initialize(const std::vector<SShaderData>& shadersData) BALBINO_NOEXCEPT_SINCE_CXX11->int32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetBinaryData() const BALBINO_NOEXCEPT_SINCE_CXX11->std::vector<std::vector<uint8_t>>;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetBinaryData(std::size_t idx = 0) const BALBINO_NOEXCEPT_SINCE_CXX11->std::vector<uint8_t>;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetShaderCount() const BALBINO_NOEXCEPT_SINCE_CXX11->uint32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto GetStages() const BALBINO_NOEXCEPT_SINCE_CXX11->const VkShaderStageFlagBits*;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Handle() const BALBINO_NOEXCEPT_SINCE_CXX11->const VkShaderEXT*;

  private:
    std::vector<VkShaderEXT> m_shaders;
    std::vector<VkShaderStageFlagBits> m_stages;
    const CVkDevice* m_pDevice;

    PFN_vkCreateShadersEXT vkCreateShadersEXT{VK_NULL_HANDLE};
    PFN_vkDestroyShaderEXT vkDestroyShaderEXT{VK_NULL_HANDLE};
    PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT{VK_NULL_HANDLE};
};
BALBINO_CONSTEXPR_SINCE_CXX20 CVkShader::CVkShader(const CVkDevice* pDevice)
    : m_pDevice{pDevice}
{
}
BALBINO_CONSTEXPR_SINCE_CXX20 CVkShader::~CVkShader()
{
    for (const VkShaderEXT& shader : m_shaders)
    {
        vkDestroyShaderEXT(m_pDevice->Device(), shader, VK_NULL_HANDLE);
    }
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkShader::Initialize(const std::vector<SShaderData>& shadersData) BALBINO_NOEXCEPT_SINCE_CXX11 -> int32_t
{
    vkCreateShadersEXT       = std::bit_cast<PFN_vkCreateShadersEXT>(vkGetDeviceProcAddr(m_pDevice->Device(), "vkCreateShadersEXT"));
    vkDestroyShaderEXT       = std::bit_cast<PFN_vkDestroyShaderEXT>(vkGetDeviceProcAddr(m_pDevice->Device(), "vkDestroyShaderEXT"));
    vkGetShaderBinaryDataEXT = std::bit_cast<PFN_vkGetShaderBinaryDataEXT>(vkGetDeviceProcAddr(m_pDevice->Device(), "vkGetShaderBinaryDataEXT"));

    std::vector<VkShaderCreateInfoEXT> createInfos{};
    createInfos.reserve(shadersData.size());
    m_stages.reserve(shadersData.size());
    m_shaders.resize(shadersData.size());
    for (std::size_t i{}; i < shadersData.size(); ++i)
    {
        const auto& [code, name, shaderType, isBinary]{shadersData[i]};
        const auto stage{static_cast<VkShaderStageFlagBits>(shaderType)};
        const VkShaderStageFlags nextStage{i + 1 < shadersData.size() ? shadersData[i + 1].stage : 0};
        createInfos.push_back(VkShaderCreateInfoEXT{VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT, VK_NULL_HANDLE, VK_SHADER_CREATE_LINK_STAGE_BIT_EXT, stage, nextStage, isBinary ? VK_SHADER_CODE_TYPE_BINARY_EXT : VK_SHADER_CODE_TYPE_SPIRV_EXT,
                                                    code.size(), code.data(), name, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_NULL_HANDLE});
        m_stages.emplace_back(stage);
    }
    if (!CheckVkResult(vkCreateShadersEXT(m_pDevice->Device(), static_cast<uint32_t>(createInfos.size()), createInfos.data(), VK_NULL_HANDLE, m_shaders.data())))
    {
        return -1;
    }
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkShader::GetBinaryData() const BALBINO_NOEXCEPT_SINCE_CXX11 -> std::vector<std::vector<uint8_t>>
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
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkShader::GetBinaryData(const std::size_t idx) const BALBINO_NOEXCEPT_SINCE_CXX11 -> std::vector<uint8_t>
{
    if (idx >= m_shaders.size())
    {
        return {};
    }

    std::size_t dataSize{};
    std::vector<uint8_t> binaryData{};
    vkGetShaderBinaryDataEXT(m_pDevice->Device(), m_shaders[idx], &dataSize, nullptr);
    binaryData.resize(dataSize);
    vkGetShaderBinaryDataEXT(m_pDevice->Device(), m_shaders[idx], &dataSize, binaryData.data());
    return binaryData;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkShader::GetShaderCount() const BALBINO_NOEXCEPT_SINCE_CXX11 -> uint32_t
{
    return static_cast<uint32_t>(m_shaders.size());
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkShader::GetStages() const BALBINO_NOEXCEPT_SINCE_CXX11 -> const VkShaderStageFlagBits*
{
    return m_shaders.empty() ? VK_NULL_HANDLE : m_stages.data();
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkShader::Handle() const BALBINO_NOEXCEPT_SINCE_CXX11 -> const VkShaderEXT*
{
    return m_shaders.empty() ? VK_NULL_HANDLE : m_shaders.data();
}
} // namespace DeerVulkan