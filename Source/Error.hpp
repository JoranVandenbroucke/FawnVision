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
constexpr void SetError(const error_code errorCode, const std::format_string<Args...> pFmt, Args&&... args)
{
    std::format_to_n(g_error.str.data(), MAX_STRING_SIZE-1, pFmt, std::forward<decltype(args)>(args)...);
    g_error.error = errorCode;
}
} // namespace FawnVision