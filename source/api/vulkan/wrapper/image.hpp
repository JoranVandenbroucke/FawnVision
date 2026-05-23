#pragma once
#include "../deer_vulkan_core.hpp"
#include "device.hpp"
#include "physical_device.hpp"

namespace deer_vulkan
{
struct ImageCreateInfo
{
    std::uint32_t format{};
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t depth{};
    std::uint32_t mipCount{};
    std::uint32_t arrayCount{};
    std::uint32_t sampleCount{};
    std::uint32_t usage{};
    std::uint32_t memoryProperty{};
    std::uint8_t imageType{};
    std::uint8_t tiling{};
};

struct Image
{
    vk::Image image{nullptr};
    vk::DeviceMemory memory{nullptr};
    vk::ImageLayout layout{};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const PhysicalDevice& physicalDevice, const ImageCreateInfo& createInfo, Image& image) noexcept -> vk_status
{
    const vk::ImageCreateInfo imageCI{
        .sType = vk::StructureType::eImageCreateInfo,
        .pNext=nullptr,
        .flags={},
        .imageType=                      static_cast<vk::ImageType>(createInfo.imageType),
        .format=                         static_cast<vk::Format>(createInfo.format),
        .extent=                  {
            .width=createInfo.width,
            .height=createInfo.height,
            .depth=createInfo.depth,
        },
        .mipLevels=createInfo.mipCount,
        .arrayLayers=createInfo.arrayCount,
        .samples=static_cast<vk::SampleCountFlagBits>(createInfo.sampleCount),
        .tiling=static_cast<vk::ImageTiling>(createInfo.tiling),
        .usage=static_cast<vk::ImageUsageFlagBits>(createInfo.usage),
        .sharingMode=vk::SharingMode::eExclusive,
        .queueFamilyIndexCount=0,
        .pQueueFamilyIndices=nullptr,
        .initialLayout=vk::ImageLayout::eUndefined,
    };
    image.image = device.device.createImage(imageCI, nullptr, dispatch.dispatch);

    const vk::MemoryRequirements memRequirements { device.device.getImageMemoryRequirements(image.image, dispatch.dispatch)};

    const vk::MemoryAllocateInfo allocInfo{
        .sType           = vk::StructureType::eMemoryAllocateInfo,
        .pNext           = nullptr,
        .allocationSize  = memRequirements.size,
        .memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, createInfo.memoryProperty),
    };
    image.memory = device.device.allocateMemory(allocInfo, nullptr, dispatch.dispatch);
    device.device.bindImageMemory(image.image, image.memory, 0, dispatch.dispatch);
    image.layout = imageCI.initialLayout;

    return vk_status::ok;
}

inline auto Cleanup(const Dispatch& dispatch, const Device& device, Image& image) noexcept -> void
{
    device.device.freeMemory(image.memory, nullptr, dispatch.dispatch);
    device.device.destroyImage(image.image, nullptr, dispatch.dispatch);
    image.image  = nullptr;
    image.memory = nullptr;
}

} // namespace deer_vulkan
