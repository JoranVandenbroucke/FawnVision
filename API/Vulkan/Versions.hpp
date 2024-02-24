//
// Created by Joran on 30/01/2024.
//

#ifndef VERSIONS_HPP
#define VERSIONS_HPP

#include <cstdint>
#include <string_view>

#define BALBINO_NULL nullptr

#if defined( _WIN32 )
#    define BALBINO_PLATFORM_WINDOWS
#elif defined( __linux__ )
#    define BALBINO_PLATFORM_LINUX
#elif defined( __APPLE__ )
#    define BALBINO_PLATFORM_MAC
#endif

#ifdef __cplusplus
#    if defined( _MSVC_LANG ) && _MSVC_LANG > __cplusplus
#        define BALBINO_STL_LANG _MSVC_LANG
#    else// ^^^ language mode is _MSVC_LANG / language mode is __cplusplus vvv
#        define BALBINO_STL_LANG __cplusplus
#    endif// ^^^ language mode is larger of _MSVC_LANG and __cplusplus ^^^
#else     // ^^^ determine compiler's C++ mode / no C++ support vvv
#    define BALBINO_STL_LANG 0L
#endif// ^^^ no C++ support ^^^

#if BALBINO_STL_LANG <= 199711L
#    define BALBINO_STD_VERSION 98
#elif BALBINO_STL_LANG <= 201103L
#    define BALBINO_STD_VERSION 11
#elif BALBINO_STL_LANG <= 201402L
#    define BALBINO_STD_VERSION 14
#elif BALBINO_STL_LANG <= 201703L
#    define BALBINO_STD_VERSION 17
#elif BALBINO_STL_LANG <= 202002L
#    define BALBINO_STD_VERSION 20
#elif BALBINO_STL_LANG <= 202302L
#    define BALBINO_STD_VERSION 23
#else
// Expected release year of the next C++ standard
#    define BALBINO_STD_VERSION 26
#endif

#if BALBINO_STD_VERSION < 11
#    define BALBINO_NOEXCEPT_SINCE_CXX11
#else
#    define BALBINO_NOEXCEPT_SINCE_CXX11 noexcept
#endif

#if BALBINO_STD_VERSION < 14
#    define BALBINO_EXPLICIT_SINCE_CXX11
#    define BALBINO_CONSTEXPR_SINCE_CXX14
#else
#    define BALBINO_EXPLICIT_SINCE_CXX11 explicit
#    define BALBINO_CONSTEXPR_SINCE_CXX14 constexpr
#endif

#if BALBINO_STD_VERSION < 17
#    define BALBINO_CONSTEXPR_SINCE_CXX17
#    define BALBINO_NODISCARD_SINCE_CXX17
#    define BALBINO_INLINE_SINCE_CXX17
#else
#    define BALBINO_CONSTEXPR_SINCE_CXX17 constexpr
#    define BALBINO_NODISCARD_SINCE_CXX17 [[nodiscard]]
#    define BALBINO_INLINE_SINCE_CXX17 inline
#endif

#if BALBINO_STD_VERSION < 20
#    define BALBINO_CONSTEXPR_SINCE_CXX20
#else
#    define BALBINO_CONSTEXPR_SINCE_CXX20 constexpr
#endif

#if BALBINO_STD_VERSION < 20
#    define BALBINO_CONSTEXPR_SINCE_CXX23
#else
#    define BALBINO_CONSTEXPR_SINCE_CXX23 constexpr
#endif

#ifdef _DEBUG
#    define BALBINO_DEBUG
#    define BALBINO_ASSERT( expr )                                \
        if ( !( expr ) )                                          \
        {                                                         \
            std::cerr << "Assertion failed: " #expr << std::endl; \
            std::abort();                                         \
        }
#else
#    define BALBINO_ASSERT( expr ) ( (void) 0 )
#endif

#if defined( __GNUC__ ) || defined( __clang__ )
#    define BALBINO_LIKELY( expr ) __builtin_expect( !!( expr ), 1 )
#    define BALBINO_UNLIKELY( expr ) __builtin_expect( !!( expr ), 0 )
#else
#    define BALBINO_LIKELY( expr ) ( expr )
#    define BALBINO_UNLIKELY( expr ) ( expr )
#endif

#define BALBINO_ERROR( message )                        \
    do                                                  \
    {                                                   \
        std::cerr << "Error: " << message << std::endl; \
        std::abort();                                   \
    } while ( 0 )

#define QUOTE_( x ) #x
#define QUOTE( x ) QUOTE_( x )
#define FILE_LINE_ __FILE__ "(" QUOTE( __LINE__ ) ") : "

#define NOTE( x ) message( x )
#define FILE_LINE message( FILE_LINE_ )

#define TODO( x )                                                             \
    message( FILE_LINE_ "\n"                                                  \
                        " ------------------------------------------------\n" \
                        "|  TODO :   " #x "\n"                                \
                        " -------------------------------------------------\n" )
#define FIXME( x )                                                            \
    message( FILE_LINE_ "\n"                                                  \
                        " ------------------------------------------------\n" \
                        "|  FIXME :  " #x "\n"                                \
                        " -------------------------------------------------\n" )
#define todo( x ) message( FILE_LINE_ " TODO :   " #x "\n" )
#define fixme( x ) message( FILE_LINE_ " FIXME:   " #x "\n" )

namespace Balbino
{
#if defined __clang__ || defined __GNUC__
#    define PRETTY_FUNCTION __PRETTY_FUNCTION__
#    define PRETTY_FUNCTION_PREFIX '='
#    define PRETTY_FUNCTION_SUFFIX ']'
#elif defined _MSC_VER
#    define PRETTY_FUNCTION __FUNCSIG__
#    define PRETTY_FUNCTION_PREFIX '<'
#    define PRETTY_FUNCTION_SUFFIX '>'
#endif
    template<typename T>
    constexpr std::string_view Name()
    {
        constexpr std::string_view functionName { PRETTY_FUNCTION };
        constexpr uint64_t first { functionName.find_first_not_of( ' ', functionName.find_first_of( PRETTY_FUNCTION_PREFIX ) + 1 ) };
        return functionName.substr( first, functionName.find_last_of( PRETTY_FUNCTION_SUFFIX ) - first );
    }

    template<typename T>
    constexpr std::size_t Hash()
    {
        return std::hash<std::string_view>( Name<T>() );
    }

#define MAKE_VERSION( ma, mi, pa ) uint32_t( ( uint8_t( ma ) << 24 ) | uint8_t( mi ) << 16 | uint16_t( pa ) )
}// namespace Balbino
#include <type_traits>

template<typename TEnumType>
struct is_enum_flag : std::false_type
{
    using EnumNonFlagType = void;
};

template<typename TEnumType>
concept EnumType = std::is_enum_v<TEnumType>;
template<typename TEnumType>
concept EnumFlagType = is_enum_flag<TEnumType>::value;

template<EnumType TEnumType>
constexpr TEnumType operator|( TEnumType a_Lhs, TEnumType a_Rhs )
    requires is_enum_flag<TEnumType>::value
{
    using underlyingType = std::underlying_type_t<TEnumType>;
    return TEnumType( static_cast<underlyingType>( a_Lhs ) | static_cast<underlyingType>( a_Rhs ) );
}
template<EnumType TEnumType>
constexpr TEnumType operator&( TEnumType a_Lhs, TEnumType a_Rhs )
    requires is_enum_flag<TEnumType>::value
{
    using underlyingType = std::underlying_type_t<TEnumType>;
    return TEnumType( static_cast<underlyingType>( a_Lhs ) & static_cast<underlyingType>( a_Rhs ) );
}

#define ENABLE_ENUM_BITWISE_OPERATORS(enumClassFlagType) template<> struct is_enum_flag<enumClassFlagType> : std::true_type{};

#endif
