//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "../DeerVulkan_Core.hpp"

#include "VkDevice.hpp"

namespace DeerVulkan
{
class CVkImageView final
{
public:
    constexpr explicit CVkImageView(const CVkDevice* pDevice)
        : m_pDevice{pDevice}
    {
    }

    ~CVkImageView()
    {
        vkDestroyImageView(m_pDevice->Device(), m_imageView, VK_NULL_HANDLE);
    }


    CVkImageView(const CVkImageView& other) = delete;
    CVkImageView(CVkImageView&& other) noexcept = delete;
    auto operator=(const CVkImageView& other) -> CVkImageView& = delete;
    auto operator=(CVkImageView&& other) -> CVkImageView& = delete;

    [[nodiscard]] auto Initialize() noexcept -> int32_t
    {
        constexpr VkImageViewCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                                   .pNext = VK_NULL_HANDLE,
                                                   .flags = 0,
                                                   .image = VK_NULL_HANDLE,
                                                   .viewType = VK_IMAGE_VIEW_TYPE_1D,
                                                   .format = VK_FORMAT_R8G8B8A8_SRGB,
                                                   .components = {},
                                                   .subresourceRange = {}};
        if (!CheckVkResult(vkCreateImageView(m_pDevice->Device(), &createInfo, VK_NULL_HANDLE, &m_imageView)))
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr auto Handle() const noexcept -> VkImageView
    {
        return m_imageView;
    }

private:
    const CVkDevice* m_pDevice{nullptr};
    VkImageView m_imageView{VK_NULL_HANDLE};
};

} // namespace DeerVulkan