//
// Created by joran on 02/01/2024.
//

#include "VkInstance.h"

#include <iostream>
#include <set>

#include "../DeerVulkan_Core.h"
#include "VkDevice.h"
#include "VkSurface.h"

namespace DeerVulkan
{
#if defined( _DEBUG )
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReport( const VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t code, const char* layerPrefix, const char* msg, void* userData )
    {
        const char* debugType { flags & VK_DEBUG_REPORT_ERROR_BIT_EXT                         ? "ERROR"
                                        : flags & VK_DEBUG_REPORT_WARNING_BIT_EXT             ? "WARNING"
                                        : flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT         ? "INFO"
                                        : flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ? "PERFORMANCE WARNING"
                                        : flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT               ? "DEBUG"
                                                                                              : "UNKNOWN" };

        std::cout << std::format( "[Vulkan DebugLayer, {}, {}, {}]: {}", debugType, layerPrefix, code, msg ) << std::endl;
        return VK_FALSE;
    }

    VkResult CVkInstance::InitializeDebugLayerCallback()
    {
        if ( const auto pCreateCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>( vkGetInstanceProcAddr( m_instanceHandle, "vkCreateDebugReportCallbackEXT" ) ) )
        {
            constexpr VkDebugReportCallbackCreateInfoEXT debugReportCi {
                    .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                    .flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
                    .pfnCallback = DebugReport,
                    .pUserData   = BALBINO_NULL,
            };
            if ( pCreateCallback( m_instanceHandle, &debugReportCi, nullptr, &m_debugReportCallback ) == VK_SUCCESS )
            {
                return VK_SUCCESS;
            }
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }
#endif//_DEBUG && !BL_EDITOR
    CVkInstance::~CVkInstance()
    {
#if defined( _DEBUG )
        if ( m_debugReportCallback )
        {
            if ( const auto pDestroyCallback { reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>( vkGetInstanceProcAddr( m_instanceHandle, "vkDestroyDebugReportCallbackEXT" ) ) } )
            {
                pDestroyCallback( m_instanceHandle, m_debugReportCallback, nullptr );
            }
        }
#endif
    }

    int CVkInstance::Initialize( const char* name, const uint32_t appVersion, const char* const* extensions, const uint32_t extensionsCount )
    {
        if ( m_instanceHandle )
        {
            return -1;
        }
        for ( int i = 0; i < extensionsCount; ++i )
        {
            if ( const char* const ext { extensions[ i ] }; std::ranges::find_if( m_instanceExtensions,
                                                                                  [ ext ]( const char* pExtension )
                                                                                  {
                                                                                      return std::strcmp( ext, pExtension ) == 0;
                                                                                  } )
                 == m_instanceExtensions.cend() )
            {
                m_instanceExtensions.emplace_back( ext );
            }
        }

        if ( !CheckVkResult( InitializeInstance( name, appVersion, "BalbinoEngine", VK_MAKE_VERSION( 0, 1, 0 ) ) ))
        {
            return -1;
        }
#ifdef _DEBUG
        InitializeDebugLayerCallback();
#endif
        if ( !CheckVkResult( InitializePhysicalDeviceInfos() ) )
        {
            return -1;
        }

        return 0;
    }

    int32_t CVkInstance::CreateDevice( CVkDevice*& device, const size_t physicalDeviceIndex ) const noexcept
    {
        const SPhysicalDeviceInfo& info { m_physicalDevices[ std::max( static_cast<size_t>( 0 ), std::min( physicalDeviceIndex, m_physicalDevices.size() ) ) ] };
        auto* temp = new CVkDevice {};

        temp->Initialize( info, m_physicalDeviceLayers, m_physicalDeviceExtensions );

        device = temp;
        return 0;
    }

    int32_t CVkInstance::FindBestPhysicalDeviceIndex( const CVkSurface* surface )
    {
        for ( auto& [ deviceProperties, deviceMemoryProperties, deviceFeatures, queueFamilyProperties, device ] : m_physicalDevices )
        {
            for ( uint32_t j = 0; j < static_cast<uint32_t>( queueFamilyProperties.size() ); ++j )
            {
                VkBool32 supportsPresent { VK_FALSE };
                vkGetPhysicalDeviceSurfaceSupportKHR( device, j, surface->GetHandle(), &supportsPresent );
                if ( supportsPresent && queueFamilyProperties[ j ].queueFlags & VK_QUEUE_GRAPHICS_BIT && deviceFeatures.samplerAnisotropy )
                {
                    return static_cast<int32_t>( j );
                }
            }
        }
        return -1;
    }

    VkResult CVkInstance::InitializeInstance( const char* appName, const uint32_t appVersion, const char* engineName, const uint32_t engineVersion )
    {
        const VkApplicationInfo applicationInfo {
                .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext              = VK_NULL_HANDLE,
                .pApplicationName   = appName,
                .applicationVersion = appVersion,
                .pEngineName        = engineName,
                .engineVersion      = engineVersion,
                .apiVersion         = VK_API_VERSION_1_3,
        };
        VkInstanceCreateInfo createInfo {
                .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext                   = VK_NULL_HANDLE,
                .flags                   = 0,
                .pApplicationInfo        = &applicationInfo,
                .enabledLayerCount       = static_cast<uint32_t>( m_instanceLayers.size() ),
                .ppEnabledLayerNames     = !m_instanceLayers.empty() ? m_instanceLayers.data() : VK_NULL_HANDLE,
                .enabledExtensionCount   = static_cast<uint32_t>( m_instanceExtensions.size() ),
                .ppEnabledExtensionNames = !m_instanceExtensions.empty() ? m_instanceExtensions.data() : VK_NULL_HANDLE,
        };
        VkResult result { vkCreateInstance( &createInfo, VK_NULL_HANDLE, &m_instanceHandle ) };
        if ( result == VK_ERROR_LAYER_NOT_PRESENT )
        {
            createInfo.enabledLayerCount   = 0;
            createInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
            result                         = vkCreateInstance( &createInfo, VK_NULL_HANDLE, &m_instanceHandle );
        }
        return result;
    }

    VkResult CVkInstance::InitializePhysicalDeviceInfos()
    {
        uint32_t deviceCount {};
        VkResult result { vkEnumeratePhysicalDevices( m_instanceHandle, &deviceCount, VK_NULL_HANDLE ) };
        if ( result != VK_SUCCESS )
        {
            return result;
        }
        std::vector<VkPhysicalDevice> devices( deviceCount, BALBINO_NULL );
        result = vkEnumeratePhysicalDevices( m_instanceHandle, &deviceCount, devices.data() );
        if ( result != VK_SUCCESS )
        {
            return result;
        }

        m_physicalDevices.reserve( deviceCount );
        for ( uint32_t i = 0; i < deviceCount; ++i )
        {
            uint32_t queueFamilyCount {};
            vkGetPhysicalDeviceQueueFamilyProperties( devices[ i ], &queueFamilyCount, nullptr );
            std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
            vkGetPhysicalDeviceQueueFamilyProperties( devices[ i ], &queueFamilyCount, queueFamilies.data() );

            uint32_t extensionCount;
            result = vkEnumerateDeviceExtensionProperties( devices[ i ], nullptr, &extensionCount, nullptr );
            if ( result != VK_SUCCESS )
            {
                return result;
            }
            std::vector<VkExtensionProperties> extensions( extensionCount );
            result = vkEnumerateDeviceExtensionProperties( devices[ i ], nullptr, &extensionCount, extensions.data() );
            if ( result != VK_SUCCESS )
            {
                return result;
            }

            std::set<std::string> requiredExtensions( m_physicalDeviceExtensions.begin(), m_physicalDeviceExtensions.end() );
            for ( const auto& extension : extensions )
            {
                requiredExtensions.erase( extension.extensionName );
            }

            if ( !requiredExtensions.empty() )
            {
                continue;
            }

            SPhysicalDeviceInfo deviceInfo {
                    .queueFamilyProperties = queueFamilies,
                    .device                = devices[ i ],
            };
            vkGetPhysicalDeviceProperties( devices[ i ], &deviceInfo.deviceProperties );
            vkGetPhysicalDeviceMemoryProperties( devices[ i ], &deviceInfo.deviceMemoryProperties );
            vkGetPhysicalDeviceFeatures( devices[ i ], &deviceInfo.deviceFeatures );

            m_physicalDevices.emplace_back( deviceInfo );
        }
        return VK_SUCCESS;
    }

    bool CVkInstance::SupportsLayers( const std::array<const char*, 1>& layers )
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

        std::vector<VkLayerProperties> availableLayers( layerCount );
        vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

        for ( const auto& [ layerName, specVersion, implementationVersion, description ] : availableLayers )
        {
            if ( strcmp( layerName, layers[ 0 ] ) == 0 )
            {
                return true;
            }
        }

        return false;
    }
}// namespace DeerVulkan
