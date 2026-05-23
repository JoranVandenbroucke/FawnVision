#pragma once
#include "../deer_vulkan_core.hpp"
#include "device.hpp"
#include "dispatch.hpp"
#include "physical_device.hpp"

namespace deer_vulkan
{
struct BufferCreateInfo
{
    std::uint64_t size{};
    std::uint32_t usage{};
    std::uint32_t memoryProperty{};
};

struct Buffer
{
    vk::Buffer buffer{nullptr};
    vk::DeviceMemory memory{};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const PhysicalDevice& physicalDevice, const BufferCreateInfo& createInfo,
                                     Buffer& buffer) noexcept -> vk_status
{
    const vk::DeviceSize size{createInfo.size};
    const vk::BufferUsageFlags usage{static_cast<vk::BufferUsageFlagBits>(createInfo.usage)};
    const vk::BufferCreateInfo bufferCI{
        .sType                 = vk::StructureType::eBufferCreateInfo,
        .pNext                 = nullptr,
        .flags                 = {},
        .size                  = size,
        .usage                 = usage,
        .sharingMode           = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
    };

    buffer.buffer = device.device.createBuffer(bufferCI, nullptr, dispatch.dispatch);

    const vk::MemoryRequirements memoryRequirements{device.device.getBufferMemoryRequirements(buffer.buffer, dispatch.dispatch)};
    const vk::MemoryAllocateInfo allocInfo{
        .sType           = vk::StructureType::eMemoryAllocateInfo,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, createInfo.memoryProperty),
    };
    buffer.memory = device.device.allocateMemory(allocInfo, nullptr, dispatch.dispatch);

    device.device.bindBufferMemory(buffer.buffer, buffer.memory, 0, dispatch.dispatch);
    return vk_status::ok;
}

inline auto Cleanup(const Dispatch& dispatch, const Device& device, Buffer& buffer) noexcept -> void
{
    device.device.freeMemory(buffer.memory, nullptr, dispatch.dispatch);
    device.device.destroyBuffer(buffer.buffer, nullptr, dispatch.dispatch);
    buffer.memory = nullptr;
    buffer.buffer = nullptr;
}

template <typename T, auto Size = std::dynamic_extent>
auto CopyData(const Dispatch& dispatch, const Device& device, const Buffer& buffer, const std::span<const T, Size> data, const std::uint32_t offset = 0) noexcept -> void
{
    void* pData = device.device.mapMemory(buffer.memory, offset, static_cast<vk::DeviceSize>(data.size_bytes()), {}, dispatch.dispatch);
    memcpy(pData, std::bit_cast<const unsigned char*>(data.data()), data.size_bytes());
    device.device.unmapMemory(buffer.memory, dispatch.dispatch);
}

inline void Flush(const Dispatch& dispatch, const Device& device, const Buffer& buffer, const std::uint64_t size, const std::uint64_t offset) noexcept
{
    const vk::MappedMemoryRange mappedRange{
        .sType  = vk::StructureType::eMappedMemoryRange,
        .pNext  = nullptr,
        .memory = buffer.memory,
        .offset = offset,
        .size   = size,
    };

    device.device.flushMappedMemoryRanges({mappedRange}, dispatch.dispatch);
}
} // namespace deer_vulkan
