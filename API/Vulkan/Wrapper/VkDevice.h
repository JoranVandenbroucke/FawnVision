//
// Created by joran on 02/01/2024.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

#include "Base.h"
#include "VkInstance.h"

namespace DeerVulkan
{
    class CDeviceHolder
    {
    public:
        CDeviceHolder() = default;

        virtual ~CDeviceHolder()
        {
            if ( m_device != VK_NULL_HANDLE )
            {
                vkDestroyDevice( m_device, VK_NULL_HANDLE );
            }
        }

    protected:
        VkDevice m_device{VK_NULL_HANDLE};
    };

    class CVkDevice final : public CDeviceHolder, public CRefCounted
    {
    public:
        CVkDevice()
            : CDeviceHolder{}
              , CRefCounted{}
        {}

        ~CVkDevice() override
        {
            vkDeviceWaitIdle( m_device );
        }

        int32_t Initialize( SPhysicalDeviceInfo deviceInfo, const std::vector<const char*>& layersToEnable, const std::vector<const char*>& extensionsToEnable );

        void WaitIdle() const
        {
            vkDeviceWaitIdle( m_device );
        }

        [[nodiscard]] const VkDevice& GetVkDevice() const noexcept        {
            return m_device;
        }

        [[nodiscard]] const SPhysicalDeviceInfo& GetPhysicalDeviceInfo() const noexcept        {
            return m_deviceInfo;
        }

    private:
        SPhysicalDeviceInfo m_deviceInfo{};
    };
}// namespace DeerVulkan
