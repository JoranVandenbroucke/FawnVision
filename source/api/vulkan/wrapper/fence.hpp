#pragma once
#include "../deer_vulkan_core.hpp"
#include "device.hpp"
#include "dispatch.hpp"

namespace deer_vulkan
{

struct Fence
{
    vk::Fence fence{nullptr};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, Fence& fence) noexcept -> vk_status
{
    constexpr vk::FenceCreateInfo createInfo{
        .sType = vk::StructureType::eFenceCreateInfo,
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled,
    };

    fence.fence = device.device.createFence(createInfo, nullptr, dispatch.dispatch);
    return vk_status::ok;
}

inline auto Cleanup(const Dispatch& dispatch, const Device& device, const Fence& fence) noexcept -> void
{
    device.device.destroyFence(fence.fence, nullptr, dispatch.dispatch);
}

[[nodiscard]] inline auto WaitAndReset(const Dispatch& dispatch, const Device& dev, const Fence& fence) noexcept -> vk_status
{
    return FromVkResult(dev.device.resetFences(1, &fence.fence, dispatch.dispatch));
}
} // namespace deer_vulkan
