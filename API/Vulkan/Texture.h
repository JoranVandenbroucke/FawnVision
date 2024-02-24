//
// Created by Joran on 08/01/2024.
//

#pragma once
#include "DeerVulkan_Core.h"

namespace DeerVulkan
{
    class CVkDevice;
    class CVkCommandHandler;
    class CVkImageView;
    class CVkSampler;

    class CTexture
    {
    public:
        CTexture() = default;

        int32_t Initialize() noexcept{return -1;}
        void Cleanup() noexcept;
        [[nodiscard]] uint32_t MipCount() const noexcept;

    private:
        CVkImageView* m_view{BALBINO_NULL};
        CVkSampler* m_sampler{BALBINO_NULL};
    };
}// namespace DeerVulkan
