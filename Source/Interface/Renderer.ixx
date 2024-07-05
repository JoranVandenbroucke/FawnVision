//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "API/Vulkan/Instance.hpp"
#include "API/Vulkan/Presenter.hpp"

#include <cstdint>
#include <cstring>
#include <unordered_map>

export module FawnVision.Renderer;
import FawnVision.Window;

#if defined __clang__ || defined __GNUC__
#    define PRETTY_FUNCTION __PRETTY_FUNCTION__
#    define PRETTY_FUNCTION_PREFIX '='
#    define PRETTY_FUNCTION_SUFFIX ']'
#elif defined _MSC_VER
#    define PRETTY_FUNCTION __FUNCSIG__
#    define PRETTY_FUNCTION_PREFIX '<'
#    define PRETTY_FUNCTION_SUFFIX '>'
#endif

template <typename T>
constexpr auto ToString() -> std::string_view
{
    constexpr std::string_view functionName{PRETTY_FUNCTION};
    constexpr uint64_t first{functionName.find_first_not_of(' ', functionName.find_first_of(PRETTY_FUNCTION_PREFIX) + 1)};
    return functionName.substr(first, functionName.find_last_of(PRETTY_FUNCTION_SUFFIX) - first);
}

template <typename T>
constexpr auto Hash() -> std::size_t
{
    return std::hash<std::string_view>{}(ToString<T>());
}


namespace FawnVision
{
export struct SRenderer
{
    DeerVulkan::CVkInstance m_instance;
    DeerVulkan::CVkDevice m_device;
    DeerVulkan::CVkSurface m_surface;
    DeerVulkan::SQueueFamily m_familyIndex{};

    DeerVulkan::CPresenter presenter{m_device, m_surface, m_familyIndex};
    std::array<DeerVulkan::CVkQueue, DeerVulkan::g_queueCount> m_queue;
    DeerVulkan::CVkSwapChain m_swapChain{m_device, m_surface};
    DeerVulkan::CVkSemaphore m_timelineSemaphore{m_device, true};
    DeerVulkan::CVkSemaphore m_binarySemaphore{m_device, false};
    DeerVulkan::CVkCommandPool m_commandPool{m_device, m_swapChain};
    DeerVulkan::CVkCommandBuffer* m_commandBuffer{nullptr};
};

export struct SRendererCreateInfo{};

auto InitializeRenderer(const SWindow& window, SRenderer& renderer) -> int32_t
{
    if (const int32_t returnValue{renderer.instance.Initialize(window.pWindow, "Fixme", 0U, window.extensions, window.extensionCount)}; returnValue != 0)
    {
        return -1;
    }
    if (renderer.instance.CreatePresentor(renderer.presenter, window.width, window.height) != 0)
    {
        return -1;
    }
    return 0;
}

export auto CreateRenderer(const SWindow& window, const SRendererCreateInfo& createInfo, SRenderer& renderer) -> int32_t
{
    constexpr SWindow emptyWindow{};
    constexpr SRenderer empty{};
    if (std::memcmp(&window, &emptyWindow, sizeof(SWindow)) == 0 || std::memcmp(&renderer, &empty, sizeof(SRenderer)) != 0)
    {
        return -1;
    }

    renderer = SRenderer{};
    if (InitializeRenderer(window, renderer) != 0)
    {
        return -1;
    }
    return 0;
}

export auto ReleaseRenderer(SRenderer& renderer) -> int32_t
{
    constexpr SRenderer empty{};
    if (std::memcmp(&renderer, &empty, sizeof(SRenderer)) == 0)
    {
        return -1;
    }
    renderer.presenter.Cleanup();
    renderer.instance.Cleanup();

    std::memset(&renderer, sizeof(SRenderer), 0);
    return 0;
}

// todo: implement
export auto ResizeRender(const SWindow& window, const SRenderer& renderer) -> int32_t
{
    constexpr SRenderer empty{};
    if (std::memcmp(&renderer, &empty, sizeof(SRenderer)) == 0)
    {
        return -1;
    }
    return 0;
}

export auto StartRender(SRenderer& renderer) -> int32_t
{
    constexpr SRenderer empty{};
    if (std::memcmp(&renderer, &empty, sizeof(SRenderer)) == 0)
    {
        return -1;
    }
    renderer.currentRenderPassIndex = 0U;
    return 0;
}

export template <typename PassData>
auto AddCompuyePass(const SRenderer& renderer, const std::string_view name, SComputePass& computePass, PassData& passData) -> int32_t
{
    constexpr SRenderer empty{};
    if (std::memcmp(&renderer, &empty, sizeof(SRenderer)) == 0)
    {
        return -1;
    }

    return 0;
}

export template <typename PassData>
auto AddRasterePass(const SRenderer& renderer, const std::string_view name, SRasterPass& rasterRenderPass, PassData& passData) -> int32_t
{
    constexpr SRenderer empty{};
    if (std::memcmp(&renderer, &empty, sizeof(SRenderer)) == 0)
    {
        return -1;
    }

    return 0;
}

export template <typename PassData>
auto AddRenderPass(const SRenderer& renderer, const std::string_view name, SRenderPass& rasterRenderPass, PassData& passData) -> int32_t
{
    constexpr SRenderer empty{};
    if (std::memcmp(&renderer, &empty, sizeof(SRenderer)) == 0)
    {
        return -1;
    }
    return 0;
}

export auto CleanupPasses(const SRenderer& renderer)
{
    constexpr SRenderer empty{};
    if (std::memcmp(&renderer, &empty, sizeof(SRenderer)) == 0)
    {
        return -1;
    }
    return 0;
}
} // namespace FawnVision