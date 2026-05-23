#pragma once
#include "../deer_vulkan_core.hpp"

#include "dispatch.hpp"
#include "instance.hpp"
#include "surface.hpp"

namespace deer_vulkan
{
struct PhysicalDevice
{
    vk::PhysicalDevice physicalDevice{};
    std::uint32_t graphicsQueueFamily{0};
    std::uint32_t computeQueueFamily{0};
    std::uint32_t presentQueueFamily{0};
    std::uint32_t graphicsQueueIdx{0};
    std::uint32_t computeQueueIdx{0};
    std::uint32_t presentQueueIdx{0};
    vk::Format depthFormat{vk::Format::eUndefined};

    vk::PhysicalDeviceProperties2 deviceProperties{};
    vk::PhysicalDeviceMemoryProperties2 deviceMemoryProperties{};
    vk::PhysicalDeviceFeatures2 deviceFeatures{};
    std::vector<vk::QueueFamilyProperties2> queueFamilyProperties{};
};

[[nodiscard]] inline auto FindMemoryType(const PhysicalDevice& physicalDevice, const std::uint32_t memoryType, const std::uint32_t memoryProperty) noexcept -> std::uint32_t
{
    for (std::uint32_t i{}; i < physicalDevice.deviceMemoryProperties.memoryProperties.memoryTypeCount; ++i)
    {
        if ((memoryType & (1 << i))
            && (static_cast<std::uint32_t>(physicalDevice.deviceMemoryProperties.memoryProperties.memoryTypes[i].propertyFlags) & memoryProperty) == memoryProperty)
        {
            return i;
        }
    }

    return 0;
}

[[nodiscard]] inline auto FindDepthFormat(const Dispatch& dispatch, const PhysicalDevice& physicalDevice) noexcept -> vk::Format
{
    for (constexpr std::array candidates{vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}; const vk::Format format : candidates)
    {
        const vk::FormatProperties2 props{physicalDevice.physicalDevice.getFormatProperties2(format, dispatch.dispatch)};
        if (constexpr vk::FormatFeatureFlags formatFeatureFlags{vk::FormatFeatureFlagBits::eDepthStencilAttachment};
            props.formatProperties.optimalTilingFeatures & formatFeatureFlags)
        {
            return format;
        }
    }
    return vk::Format::eUndefined;
}

[[nodiscard]] inline auto SelectPhysicalDevice(const Dispatch& dispatch, const Instance& instance, PhysicalDevice& physicalDevice) noexcept -> vk_status
{
    const std::vector<vk::PhysicalDevice> devices = instance.instance.enumeratePhysicalDevices(dispatch.dispatch);

    // todo : filter out devices that do not fit wanted things or use setting one
    const vk::PhysicalDevice device = devices.front();
    const std::vector queueFamilies(device.getQueueFamilyProperties(dispatch.dispatch));

    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;
    std::optional<std::uint32_t> computeFamily;
    std::optional<std::uint32_t> sharedFamily; // supports graphics + present + has queueCount >= 2

    for (std::uint32_t i = 0; i < queueFamilies.size(); ++i)
    {
        const auto& family     = queueFamilies[i];
        const bool hasGraphics = static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eGraphics);
        const bool hasCompute  = static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eCompute);
        const bool hasPresent  = static_cast<bool>(SDL_Vulkan_GetPresentationSupport(static_cast<VkInstance>(instance.instance), device, i));

        // Prefer a single family that does graphics+present with room for 2 queues
        if (hasGraphics && hasPresent && family.queueCount >= 2U)
        {
            sharedFamily = i;
        }

        if (!graphicsFamily.has_value() && hasGraphics)
        {
            graphicsFamily = i;
        }

        if (!presentFamily.has_value() && hasPresent)
        {
            presentFamily = i;
        }

        // Prefer a dedicated compute family (no graphics) to avoid contention
        if (hasCompute && !hasGraphics && !computeFamily.has_value())
        {
            computeFamily = i;
        }
    }

    // Resolve: shared family beats separate graphics+present
    if (sharedFamily.has_value())
    {
        physicalDevice.graphicsQueueFamily = sharedFamily.value();
        physicalDevice.presentQueueFamily  = sharedFamily.value();
        physicalDevice.computeQueueFamily  = sharedFamily.value();

        physicalDevice.graphicsQueueIdx = 0U;
        physicalDevice.computeQueueIdx  = 1U;
        physicalDevice.presentQueueIdx  = 1U;
    }
    else
    {
        if (!graphicsFamily.has_value())
        {
            return vk_status::no_suitable_queue_family;
        }

        if (!presentFamily.has_value())
        {
            std::println("{}", SDL_GetError());
            return vk_status::format_not_supported;
        }

        physicalDevice.graphicsQueueFamily = graphicsFamily.value();
        physicalDevice.presentQueueFamily  = presentFamily.value();
    }

    // Compute: dedicated family > fallback to graphics family
    physicalDevice.physicalDevice     = device;
    physicalDevice.computeQueueFamily = computeFamily.value_or(physicalDevice.graphicsQueueFamily);
    physicalDevice.depthFormat        = FindDepthFormat(dispatch, physicalDevice);
    if (physicalDevice.depthFormat == vk::Format::eUndefined)
    {
        return vk_status::format_not_supported;
    }

    physicalDevice.deviceProperties       = physicalDevice.physicalDevice.getProperties2(dispatch.dispatch);
    physicalDevice.deviceMemoryProperties = physicalDevice.physicalDevice.getMemoryProperties2(dispatch.dispatch);
    physicalDevice.deviceFeatures         = physicalDevice.physicalDevice.getFeatures2(dispatch.dispatch);
    physicalDevice.queueFamilyProperties  = physicalDevice.physicalDevice.getQueueFamilyProperties2(dispatch.dispatch);
    return vk_status::ok;
}

inline auto GetSurfaceCapabilities(const Dispatch& dispatch, const PhysicalDevice& physicalDevice, Surface& surface) noexcept -> vk_status
{
    return FromVkResult(physicalDevice.physicalDevice.getSurfaceCapabilitiesKHR(surface.surface, &surface.capabilities, dispatch.dispatch));
}
} // namespace deer_vulkan
