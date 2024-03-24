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
    BALBINO_CONSTEXPR_SINCE_CXX20 BALBINO_EXPLICIT_SINCE_CXX11 CVkImageView(const CVkDevice* pDevice);
    BALBINO_CONSTEXPR_SINCE_CXX20 ~CVkImageView();

    CVkImageView(const CVkImageView& other)                    = delete;
    CVkImageView(CVkImageView&& other) noexcept                = delete;
    auto operator=(const CVkImageView& other) -> CVkImageView& = delete;
    auto operator=(CVkImageView&& other) -> CVkImageView&      = delete;

    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Initialize() BALBINO_NOEXCEPT_SINCE_CXX11-> int32_t;
    BALBINO_NODISCARD_SINCE_CXX17 BALBINO_CONSTEXPR_SINCE_CXX11 auto Handle() const BALBINO_NOEXCEPT_SINCE_CXX11 -> VkImageView;

  private:
    const CVkDevice* m_pDevice{BALBINO_NULL};
    VkImageView m_imageView{VK_NULL_HANDLE};
};
BALBINO_CONSTEXPR_SINCE_CXX20 CVkImageView::CVkImageView(const CVkDevice* pDevice)
    : m_pDevice{pDevice}
{
}
BALBINO_CONSTEXPR_SINCE_CXX20 CVkImageView::~CVkImageView()
{
    vkDestroyImageView(m_pDevice->Device(), m_imageView, VK_NULL_HANDLE);
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkImageView::Initialize() BALBINO_NOEXCEPT_SINCE_CXX11-> int32_t
{
    BALBINO_CONSTEXPR VkImageViewCreateInfo createInfo{.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                               .pNext            = VK_NULL_HANDLE,
                                               .flags            = 0,
                                               .image            = VK_NULL_HANDLE,
                                               .viewType         = VK_IMAGE_VIEW_TYPE_1D,
                                               .format           = VK_FORMAT_R8G8B8A8_SRGB,
                                               .components       = {},
                                               .subresourceRange = {}};
    if (!CheckVkResult(vkCreateImageView(m_pDevice->Device(), &createInfo, VK_NULL_HANDLE, &m_imageView)))
    {
        return -1;
    }
    return 0;
}
BALBINO_CONSTEXPR_SINCE_CXX11 auto CVkImageView::Handle() const BALBINO_NOEXCEPT_SINCE_CXX11 -> VkImageView
{
    return m_imageView;
}
} // namespace DeerVulkan