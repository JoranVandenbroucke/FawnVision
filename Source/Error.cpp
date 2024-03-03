//
// Copyright (c) 2024.
// Author: Joran
//

#include "Error.hpp"
#include "FawnVision_Error.hpp"

namespace FawnVision
{
auto GetError() -> const char*
{
    switch (g_error.error)
    {
    case error_code::generic: return g_error.str.data();
    case error_code::out_of_memory: return "Out Of Memory";
    default: break;
    }
    return "";
}
} // namespace FawnVision
