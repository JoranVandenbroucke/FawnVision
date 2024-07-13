//
// Copyright (c) 2024.
// Author: Joran.
//

// todo: rename this file (no utis, no helpers ...)
#pragma once
#include "compiler.hpp"

#include <string_view>

namespace Balbino
{
#if defined __GNUC__
#    define PRETTY_FUNCTION __PRETTY_FUNCTION__
constexpr char PRETTY_FUNCTION_PREFIX{'='};
constexpr char PRETTY_FUNCTION_SUFFIX{']'};
#    define PRETTY_FUNCTION_SUFFIX ']'
#elif defined __clang__
#    define PRETTY_FUNCTION __FUNCSIG__
constexpr char PRETTY_FUNCTION_PREFIX{'='};
constexpr char PRETTY_FUNCTION_SUFFIX{']'};
#elif defined _MSC_VER
#    define PRETTY_FUNCTION __FUNCSIG__
constexpr char PRETTY_FUNCTION_PREFIX{'<'};
constexpr char PRETTY_FUNCTION_SUFFIX{'>'};
#endif

template <typename T>
BALBINO_CONSTEXPR_SINCE_CXX14 auto Name() BALBINO_NOEXCEPT_SINCE_CXX11->std::string_view
{
    BALBINO_CONSTEXPR std::string_view functionName{PRETTY_FUNCTION};
    BALBINO_CONSTEXPR uint64_t first{functionName.find_first_not_of(' ', functionName.find_first_of(PRETTY_FUNCTION_PREFIX) + 1)};
    return functionName.substr(first, functionName.find_last_of(PRETTY_FUNCTION_SUFFIX) - first);
}

template <typename T>
BALBINO_CONSTEXPR_SINCE_CXX14 auto Hash() BALBINO_NOEXCEPT_SINCE_CXX11->std::size_t
{
    BALBINO_CONSTEXPR std::string_view name{Name<T>()};
    BALBINO_CONSTEXPR std::size_t prime{1099511628211ULL};
    std::size_t value{};
    for (auto&& curr : name)
    {
        value = (value ^ static_cast<size_t>(curr)) * prime;
    }
    return value;
}
} // namespace Balbino