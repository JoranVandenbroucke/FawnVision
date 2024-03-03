//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "Base.h"

namespace DeerVulkan
{
class CVkImageView final : public CDeviceObject
{
public:
    explicit CVkImageView(const CVkDevice* pDevice)
        : CDeviceObject(pDevice)
    {
    }

    ~CVkImageView() override
    {
        vkDestroyImageView(Device()->VkDevice(), m_imageView, VK_NULL_HANDLE);
    }

    auto Handle() const noexcept -> VkImageView
    {
        return m_imageView;
    }

private:
    VkImageView m_imageView{VK_NULL_HANDLE};
};
} // namespace DeerVulkan