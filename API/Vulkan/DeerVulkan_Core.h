//
// Created by joran on 02/01/2024.
//

#pragma once
#include <cstdint>
#include <iostream>
#include <source_location>
#include <vulkan/vulkan.hpp>
#include <Versions.hpp>

#ifndef BALBINO_NULL
    #define BALBINO_NULL nullptr
#endif//BALBINO_NULL

namespace DeerVulkan
{
    constexpr bool CheckVkResult( const VkResult err, const std::string_view failMessage = "", const VkResult exclusions = VK_SUCCESS, const std::source_location& location = std::source_location::current() )
    {
        if ( err == VK_SUCCESS || err == exclusions )
        {
            return true;
        }
        const std::string_view file = location.file_name();
        const std::string_view func = location.function_name();
        const uint32_t line         = location.line();

        std::cerr << "Vulkan Error: " << err << " at " << file << ": " << line << " (" << func << ")" << std::endl;
        if ( !failMessage.empty() )
        {
            // todo use the logging library
            std::cerr << "Message: " << failMessage << std::endl;
        }
        return false;
    }
}// namespace DeerVulkan
