#pragma once
#include "../deer_vulkan_core.hpp"
#include "device.hpp"

namespace deer_vulkan
{
struct SamplerCreateInfo
{
    bool useLinear{};
    bool mipLinear{};
    std::uint8_t repeatU{};
    std::uint8_t repeatV{};
    std::uint8_t repeatW{};
    bool anisotropyEnable{true};
    float maxAnisotropy{8.0f};
    float maxLod{1.0f};
};

struct Sampler
{
    vk::Sampler sampler{nullptr};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const SamplerCreateInfo& createInfo, Sampler& sampler) noexcept -> vk_status
{
    const vk::Filter filter{createInfo.useLinear ? vk::Filter::eLinear : vk::Filter::eNearest};
    const vk::SamplerMipmapMode mipmapMode{createInfo.mipLinear ? vk::SamplerMipmapMode::eLinear : vk::SamplerMipmapMode::eNearest};
    const auto addressModeU{static_cast<vk::SamplerAddressMode>(createInfo.repeatU)};
    const auto addressModeV{static_cast<vk::SamplerAddressMode>(createInfo.repeatV)};
    const auto addressModeW{static_cast<vk::SamplerAddressMode>(createInfo.repeatW)};

    const vk::SamplerCreateInfo samplerCI{
        .sType                   = vk::StructureType::eSamplerCreateInfo,
        .pNext                   = nullptr,
        .flags                   = {},
        .magFilter               = filter,
        .minFilter               = filter,
        .mipmapMode              = mipmapMode,
        .addressModeU            = addressModeU,
        .addressModeV            = addressModeV,
        .addressModeW            = addressModeW,
        .mipLodBias              = 0.0F,
        .anisotropyEnable        = static_cast<vk::Bool32>(createInfo.anisotropyEnable),
        .maxAnisotropy           = createInfo.maxAnisotropy,
        .compareEnable           = static_cast<vk::Bool32>(false),
        .compareOp               = vk::CompareOp::eNever,
        .minLod                  = 0,
        .maxLod                  = createInfo.maxLod,
        .borderColor             = vk::BorderColor::eFloatTransparentBlack,
        .unnormalizedCoordinates = static_cast<vk::Bool32>(true),
    };

    sampler.sampler = device.device.createSampler(samplerCI, nullptr, dispatch.dispatch);
    return vk_status::ok;
}

inline auto Cleanup(const Dispatch& dispatch, const Device& device, const Sampler& sampler) noexcept -> void
{
    device.device.destroySampler(sampler.sampler, nullptr, dispatch.dispatch);
}
} // namespace deer_vulkan
