//
// Created by Joran on 20/02/2024.
//

#include "FawnVision_Error.hpp"
#include "Error.hpp"

namespace FawnVision
{
    const char* GetError()
    {
        switch ( g_error.error )
        {
            case error_code::generic: return g_error.str;
            case error_code::out_of_memory: return "Out Of Memory";
            default: break;
        }
        return "";
    }
}// namespace FawnVision
