//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "DeerVulkan_Core.hpp"

namespace DeerVulkan
{
class CVkDevice;
class CVkCommandPool;
class CVkImageView;
class CVkSampler;

class CTexture
{
public:
    CTexture() = default;

    int32_t Initialize() noexcept
    {
        return -1;
    }

    void Cleanup() noexcept;
    [[nodiscard]] uint32_t MipCount() const noexcept;

private:
    CVkImageView* m_view{BALBINO_NULL};
    CVkSampler* m_sampler{BALBINO_NULL};
};
} // namespace DeerVulkan