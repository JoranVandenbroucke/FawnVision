//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "Presenter.hpp"
#include "Wrapper/VkDevice.hpp"
#include "Wrapper/VkInstance.hpp"
#include "Wrapper/VkSurface.hpp"

struct SDL_Window;

namespace DeerVulkan
{
class CInstance
{
public:
    constexpr CInstance() = default;
    ~CInstance() = default;
    CInstance(const CInstance& other) = delete;
    CInstance(CInstance&& other) noexcept = delete;
    auto operator=(const CInstance& other) -> CInstance& = delete;
    auto operator=(CInstance&& other) noexcept -> CInstance&
    {
        if(&other != this)
        {
            std::memcpy(this, &other, sizeof(CInstance));
        }
        return *this;
    }

    auto Initialize(SDL_Window* pWindow, const char* pAppTitle, uint32_t appVersion, const char* const* pExtensions, uint32_t extensionsCount) noexcept -> int32_t;
    auto Cleanup() noexcept -> int32_t;

    auto CreatePresentor(CPresenter& presenter, const int32_t width, const int32_t height) const -> int32_t
    {
        presenter = CPresenter{m_device, m_surface, m_familyIndex};
        return presenter.Initialize(width, height, m_familyIndex);
    }

    [[nodiscard]] auto Device() const -> const CVkDevice&
    {
        return m_device;
    }

private:
    CVkInstance m_instance;
    CVkDevice m_device;
    CVkSurface m_surface;
    SQueueFamily m_familyIndex{};
};
} // namespace DeerVulkan