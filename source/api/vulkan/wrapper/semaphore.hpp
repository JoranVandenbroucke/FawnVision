#pragma once
#include "../deer_vulkan_core.hpp"
#include "device.hpp"

namespace deer_vulkan
{

struct Semaphore
{
    vk::Semaphore semaphore{nullptr};
    std::uint64_t value{};
    bool isTimeline{};
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const bool isTimeline, Semaphore& semaphore) noexcept -> vk_status
{
    semaphore.isTimeline = isTimeline;

    const vk::SemaphoreTypeCreateInfo typeInfo{
        .sType         = vk::StructureType::eSemaphoreTypeCreateInfo,
        .pNext         = nullptr,
        .semaphoreType = vk::SemaphoreType::eTimeline,
        .initialValue  = semaphore.value,
    };

    const vk::SemaphoreCreateInfo createInfo{
        .sType = vk::StructureType::eSemaphoreCreateInfo,
        .pNext = isTimeline ? &typeInfo : nullptr,
        .flags = {},
    };

    semaphore.semaphore = device.device.createSemaphore(createInfo, nullptr, dispatch.dispatch);
    return vk_status::ok;
}

inline auto Cleanup(const Dispatch& dispatch, const Device& device, const Semaphore& semaphore) noexcept -> void
{
    if (semaphore.semaphore == nullptr){return;}
    device.device.destroySemaphore(semaphore.semaphore, nullptr, dispatch.dispatch);
}

[[nodiscard]] inline auto Wait(const Dispatch& dispatch, const Device& device, const Semaphore& semaphore) noexcept -> vk_status
{
    const vk::SemaphoreWaitInfo semaphoreWaitInfo{
        .sType          = vk::StructureType::eSemaphoreWaitInfo,
        .pNext          = nullptr,
        .flags          = {},
        .semaphoreCount = 1,
        .pSemaphores    = &semaphore.semaphore,
        .pValues        = &semaphore.value,
    };

    return FromVkResult(device.device.waitSemaphores(semaphoreWaitInfo, ~0ULL, dispatch.dispatch));
}

[[nodiscard]] inline auto Signal(const Dispatch& dispatch, const Device& device, const Semaphore& semaphore, const std::uint64_t value) noexcept -> vk_status
{
    const vk::SemaphoreSignalInfo semaphoreSignalInfo{
        .sType     = vk::StructureType::eSemaphoreSignalInfo,
        .pNext     = nullptr,
        .semaphore = semaphore.semaphore,
        .value     = value,
    };

    device.device.signalSemaphore(semaphoreSignalInfo, dispatch.dispatch);
    return vk_status::ok;
}

inline auto GetValue(const Dispatch& dispatch, const Device& device, const Semaphore& semaphore, std::uint64_t& value)
{
    return FromVkResult(device.device.getSemaphoreCounterValue(semaphore.semaphore, &value, dispatch.dispatch));
}
} // namespace deer_vulkan
