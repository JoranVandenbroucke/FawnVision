//
// Created by joran on 30/12/2023.
//

#pragma once
#include <cstdint>

#ifndef BALBINO_NULL
#define BALBINO_NULL nullptr
#endif

#ifndef STATIC_FUNCTION_NAMING
#define STATIC_FUNCTION_NAMING
#include <string_view>

#if defined __clang__ || defined __GNUC__
#define PRETTY_FUNCTION __PRETTY_FUNCTION__
#define PRETTY_FUNCTION_PREFIX '='
#define PRETTY_FUNCTION_SUFFIX ']'
#elif defined _MSC_VER
        #define PRETTY_FUNCTION __FUNCSIG__
        #define PRETTY_FUNCTION_PREFIX '<'
        #define PRETTY_FUNCTION_SUFFIX '>'
#endif
template <typename>
constexpr std::string_view FunctionName()
{
    constexpr std::string_view functionName{PRETTY_FUNCTION};
    constexpr uint64_t first = functionName.find_first_not_of(
        ' ', functionName.find_first_of(PRETTY_FUNCTION_PREFIX) + 1);
    return functionName.substr(first, functionName.find_last_of(PRETTY_FUNCTION_SUFFIX) - first);
}
template <typename T>
constexpr std::size_t Hash()
{
    return std::hash<std::string_view>{}(FunctionName<T>());
}
#endif// STATIC_FUNCTION_NAMING

namespace FawnVision
{
    enum class error_code : uint8_t
    {
        oke,
        not_initializable,
        not_initialized,
        already_initialized,
        no_display_found,
        window_not_created,
        bad_flag,
        max,
    };
}
