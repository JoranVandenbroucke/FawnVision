/*
 * MIT License
 *
 * Copyright (c) 2026 Joran Vandenbroucke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include <SDL3/SDL_vulkan.h>

import std;
import vulkan;

namespace deer_vulkan
{

// ---------------------------------------------------------------------------
// Minimal source_location replacement
// ---------------------------------------------------------------------------
struct SourceLocation
{
    const char* file{nullptr};
    const char* func{nullptr};
    std::uint32_t line{};

    [[nodiscard]] constexpr auto FileName() const noexcept -> std::string_view
    {
        return file;
    }
    [[nodiscard]] constexpr auto FunctionName() const noexcept -> std::string_view
    {
        return func;
    }
    [[nodiscard]] constexpr auto LineNumber() const noexcept -> std::uint32_t
    {
        return line;
    }
};

#define DEER_SOURCE_LOCATION                                                                                                                                                       \
    ::deer_vulkan::SourceLocation                                                                                                                                                  \
    {                                                                                                                                                                              \
        __FILE__, __func__, static_cast<std::uint32_t>(__LINE__)                                                                                                                   \
    }
#define DEER_SRC DEER_SOURCE_LOCATION

// ---------------------------------------------------------------------------
// vk_status
// ---------------------------------------------------------------------------
enum class vk_status : std::int8_t
{
    ok                  = 0,
    not_ready           = 1,
    timeout             = 2,
    incomplete          = 3,
    suboptimal          = 4,
    already_initialized = 5,

    unknown                   = -1,
    out_of_host_memory        = -2,
    out_of_device_memory      = -3,
    init_failed               = -4,
    device_lost               = -5,
    memory_map_failed         = -6,
    layer_not_present         = -7,
    extension_not_present     = -8,
    feature_not_present       = -9,
    incompatible_driver       = -10,
    too_many_objects          = -11,
    format_not_supported      = -12,
    fragmented_pool           = -13,
    out_of_pool_memory        = -14,
    out_of_date               = -15,
    surface_lost              = -16,
    pipeline_compile_required = -17,
    no_suitable_queue_family  = -18,
    no_presentation_support   = -19,
    null_value                = -20,
};

// ---------------------------------------------------------------------------
// Error info
// ---------------------------------------------------------------------------
struct VkErrorInfo
{
    vk_status code{vk_status::ok};
    std::string_view message{};
    std::string_view hint{};
};

[[nodiscard]] constexpr auto GetError(const vk_status status) noexcept -> VkErrorInfo
{
    using enum vk_status;
    switch (status)
    {
    case ok: return {.code = status, .message = "Success.", .hint = ""};
    case not_ready:
        return {.code = status, .message = "Operation not yet complete.", .hint = "Poll again or insert a pipeline barrier / vkQueueSubmit with the appropriate wait stage."};
    case timeout:
        return {.code    = status,
                .message = "Wait timed out before the object was signalled.",
                .hint    = "Increase the timeout value, check for GPU hangs, or verify fence/semaphore signal order."};
    case incomplete: return {.code = status, .message = "Enumeration was truncated; not all items were returned.", .hint = "Increase the output array size and call again."};
    case suboptimal:
        return {.code = status, .message = "Swapchain is suboptimal for the current surface.", .hint = "Recreate the swapchain at the next convenient frame boundary."};
    case already_initialized: return {.code = status, .message = "Resource was already initialised.", .hint = "Guard initialisation with a flag or use a lazy-init pattern."};

    case unknown:
        return {.code = status, .message = "An unknown or internal driver error occurred.", .hint = "Enable VK_LAYER_KHRONOS_validation and inspect the debug messenger output."};
    case out_of_host_memory:
        return {.code    = status,
                .message = "Host (CPU) memory allocation failed.",
                .hint    = "Reduce allocation sizes, audit for leaks, or handle OOM gracefully with a fallback allocator."};
    case out_of_device_memory:
        return {.code    = status,
                .message = "Device (GPU) memory allocation failed.",
                .hint    = "Free unused GPU resources, reduce texture/buffer sizes, or use a defragmenting allocator (e.g. VMA)."};
    case init_failed:
        return {.code    = status,
                .message = "Initialisation of a Vulkan object failed.",
                .hint    = "Verify driver installation, required extensions, and minimum feature/version support."};
    case device_lost:
        return {.code    = status,
                .message = "The logical device has been lost (GPU reset or driver crash).",
                .hint    = "Destroy all Vulkan objects, recreate the device, and reload all resources. Check for GPU timeout / TDR events."};
    case memory_map_failed:
        return {.code    = status,
                .message = "Failed to map device memory into host address space.",
                .hint    = "Ensure the memory type has VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT and that no other mapping is currently active on the allocation."};
    case layer_not_present:
        return {.code    = status,
                .message = "A requested validation or API layer is not installed.",
                .hint    = "Check vkEnumerateInstanceLayerProperties(); install the Vulkan SDK or update GPU drivers."};
    case extension_not_present:
        return {.code    = status,
                .message = "A required instance or device extension is not available.",
                .hint    = "Query supported extensions with vkEnumerateInstanceExtensionProperties / vkEnumerateDeviceExtensionProperties before requesting them."};
    case feature_not_present:
        return {.code    = status,
                .message = "A required device feature is not supported by this GPU.",
                .hint    = "Query VkPhysicalDeviceFeatures (and feature-chain structs) before enabling; provide a fallback path or abort with a clear minimum-spec message."};
    case incompatible_driver:
        return {.code    = status,
                .message = "The installed driver is incompatible with the requested Vulkan version.",
                .hint    = "Update GPU drivers; lower the requested API version via VkApplicationInfo::apiVersion as a fallback."};
    case too_many_objects:
        return {.code    = status,
                .message = "Too many objects of this type have already been created.",
                .hint    = "Pool and reuse Vulkan objects; destroy unused resources aggressively."};
    case format_not_supported:
        return {.code    = status,
                .message = "The requested texture or surface format is not supported by this device.",
                .hint    = "Query format support with vkGetPhysicalDeviceFormatProperties and select a supported fallback."};
    case fragmented_pool:
        return {.code    = status,
                .message = "The descriptor or memory pool is too fragmented to satisfy the request.",
                .hint    = "Reset or recreate the pool; consider sub-allocating via a custom allocator or enabling VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT."};
    case out_of_pool_memory:
        return {.code    = status,
                .message = "The descriptor pool has no remaining capacity.",
                .hint    = "Increase pool sizes at creation time or use VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT to allow individual frees."};
    case out_of_date:
        return {.code    = status,
                .message = "The swapchain is out of date and must be recreated immediately.",
                .hint    = "Call RecreateRenderer() / vkDestroySwapchainKHR before the next present. Triggered by window resize or display mode change."};
    case surface_lost:
        return {.code    = status,
                .message = "The presentation surface is no longer available.",
                .hint    = "Destroy the surface and swapchain, then recreate them (e.g. after a window move to a different monitor)."};
    case pipeline_compile_required:
        return {.code    = status,
                .message = "Pipeline compilation was deferred and is now required before this draw.",
                .hint
                = "Ensure pipeline cache warm-up is complete before the first draw call using this pipeline, or disable VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT."};
    case no_suitable_queue_family: return {.code = status, .message = "No suitable queue family found.", .hint = "Make sure the correct limits are set"};
    case no_presentation_support: return {.code = status, .message = "no presentation support.", .hint = "IDK"};
    case null_value:
        return {.code    = status,
                .message = "A null or invalid handle was passed to a Vulkan function.",
                .hint    = "Verify all preceding vkCreate*/vkAllocate* calls succeeded and returned non-null handles before use."};
    }
    return {.code = status, .message = "Unrecognised vk_status code.", .hint = "File a bug — a new status value may not have a get_error entry yet."};
}

// ---------------------------------------------------------------------------
// Status predicates
// ---------------------------------------------------------------------------
[[nodiscard]] constexpr auto IsSuccess(const vk_status status) noexcept -> bool
{
    return static_cast<std::int32_t>(status) >= 0;
}

[[nodiscard]] constexpr auto IsError(const vk_status status) noexcept -> bool
{
    return static_cast<std::int32_t>(status) < 0;
}

[[nodiscard]] constexpr auto NeedsSwapchainRecreate(const vk_status status) noexcept -> bool
{
    return status == vk_status::out_of_date || status == vk_status::surface_lost;
}

// ---------------------------------------------------------------------------
// VkResult -> vk_status
// ---------------------------------------------------------------------------
[[nodiscard]] constexpr auto FromVkResult(const vk::Result result) noexcept -> vk_status
{
    switch (result)
    {
    case vk::Result::eSuccess: return vk_status::ok;
    case vk::Result::eNotReady: return vk_status::not_ready;
    case vk::Result::eTimeout: return vk_status::timeout;
    case vk::Result::eIncomplete: return vk_status::incomplete;
    case vk::Result::eSuboptimalKHR: return vk_status::suboptimal;
    case vk::Result::eErrorOutOfHostMemory: return vk_status::out_of_host_memory;
    case vk::Result::eErrorOutOfDeviceMemory: return vk_status::out_of_device_memory;
    case vk::Result::eErrorInitializationFailed: return vk_status::init_failed;
    case vk::Result::eErrorDeviceLost: return vk_status::device_lost;
    case vk::Result::eErrorMemoryMapFailed: return vk_status::memory_map_failed;
    case vk::Result::eErrorLayerNotPresent: return vk_status::layer_not_present;
    case vk::Result::eErrorExtensionNotPresent: return vk_status::extension_not_present;
    case vk::Result::eErrorFeatureNotPresent: return vk_status::feature_not_present;
    case vk::Result::eErrorIncompatibleDriver: return vk_status::incompatible_driver;
    case vk::Result::eErrorTooManyObjects: return vk_status::too_many_objects;
    case vk::Result::eErrorFormatNotSupported: return vk_status::format_not_supported;
    case vk::Result::eErrorFragmentedPool: return vk_status::fragmented_pool;
    case vk::Result::eErrorOutOfPoolMemory: return vk_status::out_of_pool_memory;
    case vk::Result::eErrorOutOfDateKHR: return vk_status::out_of_date;
    case vk::Result::eErrorSurfaceLostKHR: return vk_status::surface_lost;
    case vk::Result::ePipelineCompileRequired: return vk_status::pipeline_compile_required;
    default: return vk_status::unknown;
    }
}

enum class vk_check_severity : std::uint8_t
{
    warning,
    error,
};

inline constexpr std::uint8_t maxFramesInFlight{2U};
} // namespace deer_vulkan
