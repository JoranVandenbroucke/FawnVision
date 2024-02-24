//
// Created by Joran on 19/01/2024.
//

#pragma once
#include "Base.h"

namespace DeerVulkan
{
    class CVkImageView final : public CDeviceObject
    {
    public:
        explicit CVkImageView( const CVkDevice* pDevice )
            : CDeviceObject( pDevice )
        {}
        ~CVkImageView() override
        {
            vkDestroyImageView( Device()->VkDevice(), m_imageView, VK_NULL_HANDLE );
        }
        VkImageView Handle() const noexcept
        {
            return m_imageView;
        }

    private:
        VkImageView m_imageView { VK_NULL_HANDLE };
    };
}// namespace DeerVulkan
