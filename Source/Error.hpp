//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once
#include "FawnVision_Core.hpp"
#include <format>

namespace FawnVision
{
enum class error_code : uint8_t
{
    oke,
    generic,
    out_of_memory,
    max,
};

struct SError
{
    std::array<char, MAX_STRING_SIZE> str;
    error_code error;
};

inline SError g_error{};

template <typename... Args>
void SetError(const error_code errorCode, const char* pFmt, const Args&... args)
{
    // Ensure the string is null-terminated and fits within the buffer
    if (const int len{std::format_to_n(g_error.str, MAX_STRING_SIZE, pFmt, args...)}; len >= 0 && static_cast<size_t>(len) < MAX_STRING_SIZE)
    {
        g_error.error = errorCode;
    }
    else
    {
        // Handle error, maybe truncate the string or set a default message
        constexpr std::array<char, 43> ErrorMessage{"Error message too long or formatting error"};
        std::copy(ErrorMessage.cbegin(), ErrorMessage.cend(), g_error.str.cbegin());
        g_error.error = error_code::generic;
    }
}
} // namespace FawnVision