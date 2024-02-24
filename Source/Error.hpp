//
// Created by Joran on 20/02/2024.
//

#pragma once
#include "FawnVision_Core.hpp"
#include <cstdio>

namespace FawnVision
{
    enum class error_code : uint8_t
    {
        oke,
        generic,
        out_of_memory,
        max,
    };

    inline struct Error
    {
        char str[ MAX_STRING_SIZE ];
        error_code error;
    } g_error {};

    template<typename... Args>
    void SetError( const error_code errorCode, const char* fmt, const Args&... args )
    {
        // Using std::format to safely format the string
        int len = std::snprintf( g_error.str, MAX_STRING_SIZE, fmt, args... );

        // Ensure the string is null-terminated and fits within the buffer
        if ( len >= 0 && static_cast<size_t>( len ) < MAX_STRING_SIZE )
        {
            g_error.error = errorCode;
        }
        else
        {
            // Handle error, maybe truncate the string or set a default message
            std::snprintf( g_error.str, MAX_STRING_SIZE, "Error message too long or formatting error" );
            g_error.error = error_code::generic;
        }
    }
}// namespace FawnVision
