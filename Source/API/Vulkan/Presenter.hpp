//
// Copyright (c) 2024.
// Author: Joran
//
#pragma once
#include "Wrapper/VkCommandPool.hpp"
#include "Wrapper/VkQueue.hpp"
#include "Wrapper/VkSemaphore.hpp"
#include "Wrapper/VkSwapChain.hpp"

namespace DeerVulkan
{
class CVkFence;
struct SQueueFamily;
class CShader;

constexpr uint8_t g_graphicsQueueId{0};
constexpr uint8_t g_computeQueueId{1};
constexpr uint8_t g_presentQueueId{2};
constexpr uint8_t g_queueCount{3};

class CPresenter
{
public:
    constexpr explicit CPresenter(const CVkDevice& device, const CVkSurface& surface, const SQueueFamily& queueFamily)
        : m_queue{
              CVkQueue{device, static_cast<uint32_t>(queueFamily.graphicsFamily), 0U},
              CVkQueue{device, static_cast<uint32_t>(queueFamily.graphicsFamily), 1U},
              CVkQueue{device, static_cast<uint32_t>(queueFamily.presentFamily), queueFamily.graphicsFamily == queueFamily.presentFamily ? 2U : 0U}
          }
          , m_swapChain{device, surface}
          , m_timelineSemaphore{device, true}
          , m_binarySemaphore{device, false}
          , m_commandPool{device, m_swapChain}
    {
    }

    ~CPresenter()
    {
        Cleanup();
    }

    CPresenter(const CPresenter& other) = delete;
    CPresenter(CPresenter&& other) noexcept = delete;
    auto operator=(const CPresenter& other) -> CPresenter& = delete;

    auto operator=(CPresenter&& other) noexcept -> CPresenter&
    {
        if(&other != this)
        {
            std::memcpy(this, &other, sizeof(CPresenter));
        }
        return *this;
    }

    auto Initialize(int32_t width, int32_t height, const SQueueFamily& familyQueue) noexcept -> int32_t;
    void Cleanup() noexcept;

    [[nodiscard]] auto BeginRender() noexcept -> int32_t;
    [[nodiscard]] auto EndRender() noexcept -> int32_t;

private:
    std::array<CVkQueue, g_queueCount> m_queue;
    CVkSwapChain m_swapChain;
    CVkSemaphore m_timelineSemaphore;
    CVkSemaphore m_binarySemaphore;
    CVkCommandPool m_commandPool;
    CVkCommandBuffer* m_commandBuffer{nullptr};
};
} // namespace DeerVulkan