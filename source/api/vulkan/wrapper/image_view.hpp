#pragma once
#include "../deer_vulkan_core.hpp"
#include "device.hpp"
#include "dispatch.hpp"
#include "image.hpp"

namespace deer_vulkan
{
struct ImageViewCreateInfo
{
    std::uint32_t format{};
    std::uint32_t aspectFlag{};
    std::uint32_t mipOffset{};
    std::uint32_t mipCount{};
    std::uint32_t layerOffset{};
    std::uint32_t layerCount{};
    std::uint8_t imageType{};
    std::uint8_t rSwizzle{};
    std::uint8_t gSwizzle{};
    std::uint8_t bSwizzle{};
    std::uint8_t aSwizzle{};
};

struct ImageView
{
    vk::ImageView imageView{nullptr};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const Image& image, const ImageViewCreateInfo& createInfo, ImageView& imageView) noexcept
    -> vk_status
{
    const vk::ImageViewCreateInfo imageViewCreateInfo{
        .sType            = vk::StructureType::eImageViewCreateInfo,
        .pNext            = nullptr,
        .flags            = {},
        .image            = image.image,
        .viewType         = static_cast<vk::ImageViewType>(createInfo.imageType),
        .format           = static_cast<vk::Format>(createInfo.format),
        .components       = vk::ComponentMapping{static_cast<vk::ComponentSwizzle>(createInfo.rSwizzle), static_cast<vk::ComponentSwizzle>(createInfo.gSwizzle),
                                           static_cast<vk::ComponentSwizzle>(createInfo.bSwizzle), static_cast<vk::ComponentSwizzle>(createInfo.aSwizzle)},
        .subresourceRange = vk::ImageSubresourceRange{
            .aspectMask     = static_cast<vk::ImageAspectFlagBits>(createInfo.aspectFlag),
            .baseMipLevel   = createInfo.mipOffset,
            .levelCount     = createInfo.mipCount,
            .baseArrayLayer = createInfo.layerOffset,
            .layerCount     = createInfo.layerCount,
        }};

    imageView.imageView = device.device.createImageView(imageViewCreateInfo, nullptr, dispatch.dispatch);
    return vk_status::ok;
}

inline auto Cleanup(const Dispatch& dispatch, const Device& device, const ImageView& imageView) noexcept -> void
{
    device.device.destroyImageView(imageView.imageView, nullptr, dispatch.dispatch);
}
} // namespace deer_vulkan
