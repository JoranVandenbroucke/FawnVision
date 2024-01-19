//
// Created by joran on 30/12/2023.
//

#pragma once
#include <cstdint>

#ifndef BALBINO_NULL
    #define BALBINO_NULL nullptr
#endif//BALBINO_NULL

#ifndef MAKE_VERSION
    #define MAKE_VERSION( ma, mi, pa ) uint32_t( ( uint8_t( ma ) << 24 ) | uint8_t( mi ) << 16 | uint16_t( pa ) )
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
template<typename>
constexpr std::string_view FunctionName()
{
    constexpr std::string_view functionName { PRETTY_FUNCTION };
    constexpr uint64_t first = functionName.find_first_not_of( ' ', functionName.find_first_of( PRETTY_FUNCTION_PREFIX ) + 1 );
    return functionName.substr( first, functionName.find_last_of( PRETTY_FUNCTION_SUFFIX ) - first );
}

template<typename T>
constexpr std::size_t Hash()
{
    return std::hash<std::string_view> {}( FunctionName<T>() );
}
#endif// STATIC_FUNCTION_NAMING
#define FV_DEFINE_HANDLE( object ) typedef struct object##_T* object

#ifdef BALBINO_VULKAN
    #include <DeerVulkan.h>
    #include <SDL_vulkan.h>
#endif

namespace FawnVision
{
    constexpr uint32_t VERSION { MAKE_VERSION( 1, 1, 0 ) };// Version 0.1
    constexpr uint32_t MAX_STRING_SIZE { 64 };

#ifdef BALBINO_VULKAN
    #define CInstance DeerVulkan::Instance
    #define CPresenter DeerVulkan::Presenter
    #define CRenderPipeline DeerVulkan::RenderPipeline
#else
    #define CInstance
    #define CPresentor
    #define CRenderPipeline
#endif

    enum class error_code : uint8_t
    {
        oke,
        not_initializable,
        not_initialized,
        already_initialized,
        no_display_found,
        window_not_created,
        bad_flag,
        out_of_memory,
        sdl_error,
        invalid_parameter,
        begin_render_failed,
        end_render_failed,
        max,
    };

    enum window_flags : uint32_t
    {

        fullscreen         = 0x00000001U,
        opengl             = 0x00000002U,
        occluded           = 0x00000004U,
        hidden             = 0x00000008U,
        borderless         = 0x00000010U,
        resizable          = 0x00000020U,
        minimized          = 0x00000040U,
        maximized          = 0x00000080U,
        mouse_grabbed      = 0x00000100U,
        input_focus        = 0x00000200U,
        mouse_focus        = 0x00000400U,
        external           = 0x00000800U,
        high_pixel_density = 0x00002000U,
        mouse_capture      = 0x00004000U,
        always_on_top      = 0x00008000U,
        utility            = 0x00020000U,
        tooltip            = 0x00040000U,
        popup_menu         = 0x00080000U,
        keyboard_grabbed   = 0x00100000U,
        vulkan             = 0x10000000U,
        metal              = 0x20000000U,
        transparent        = 0x40000000U,
        not_focusable      = 0x80000000U,
    };

    FV_DEFINE_HANDLE( Window );
    FV_DEFINE_HANDLE( Renderer );

    typedef struct WindowCreateInfo
    {
        const char* name { BALBINO_NULL };
        int32_t width {};
        int32_t height {};
        uint32_t flags {};
        int32_t screen { -1 };
    } WindowCreateInfo;

    typedef struct RendererCreateInfo
    {
        Window* window { BALBINO_NULL };
        uint32_t appVersion {};
    } RendererCreateInfo;

    error_code Initialize();

    error_code CreateWindow( const WindowCreateInfo& createInfo, Window& window ) noexcept;
    error_code SetWindowFlags( const Window& window, uint32_t flags ) noexcept;
    error_code ToggleWindowFlags( const Window& window, uint32_t flags ) noexcept;
    error_code SetWindowSize( const Window& window, int32_t w, int32_t h ) noexcept;
    error_code GetWindowSize( const Window& window, int32_t& w, int32_t& h ) noexcept;
    //  "( 0x2FFF0000u | ( 0 ) )" is the SDL value of SDL_WINDOWPOS_CENTERED, but I don't want to expose sdl stuff here
    error_code SetWindowPosition( const Window& window, int32_t x = 0x2FFF0000u | 0, int32_t y = 0x2FFF0000u | 0 ) noexcept;
    error_code GetWindowPosition( const Window& window, int32_t& x, int32_t& y ) noexcept;
    error_code ReleaseWindow( Window& window ) noexcept;

    void ProcessEvents( const Window& window, bool& loop ) noexcept;

    error_code CreateRenderer( const Window& window, Renderer& renderer );
    error_code ReleaseRenderer( Renderer& renderer );
    error_code ResizeRender( const Window& window, const Renderer& renderer);
    error_code StartRender( const Renderer& renderer );
    error_code StopRender( const Renderer& renderer );

    const char* GetWindowError();
    const char* GetRenderError();
}// namespace FawnVision
