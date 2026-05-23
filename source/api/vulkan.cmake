# =============================================================================
# vulkan.cmake  –  Vulkan + Vulkan-Hpp C++23 named module setup
# SDK >= 1.4.344  |  CMake >= 4.3  |  GCC 14+ / Clang 17+ / MSVC 19.36+
# =============================================================================

cmake_minimum_required(VERSION 4.3)   # stable import-std support

set(VULKAN_SOURCE_FILES
        vulkan/wrapper/instance.cpp
)
set(VULKAN_HEADER_FILES
        vulkan/wrapper/buffer.hpp
        vulkan/wrapper/command.hpp
        vulkan/wrapper/descriptor.hpp
        vulkan/wrapper/device.hpp
        vulkan/wrapper/fence.hpp
        vulkan/wrapper/instance.hpp
        vulkan/wrapper/image.hpp
        vulkan/wrapper/image_view.hpp
        vulkan/wrapper/physical_device.hpp
        vulkan/wrapper/queue.hpp
        vulkan/wrapper/sampler.hpp
        vulkan/wrapper/semaphore.hpp
        vulkan/wrapper/shader.hpp
        vulkan/wrapper/surface.hpp
        vulkan/wrapper/swap_chain.hpp
        vulkan/deer_vulkan_core.hpp
)
list(TRANSFORM VULKAN_SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/source/api/")
list(TRANSFORM VULKAN_HEADER_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/source/api/")

# -----------------------------------------------------------------------------
# Internal helper: build the Vulkan::HppModule interface once
# Follows the pattern from Vulkan-Headers/CMakeLists.txt exactly.
# -----------------------------------------------------------------------------
function(_deer_ensure_vulkan_hpp_module)
    if (TARGET Vulkan::HppModule)
        return()
    endif ()

    # ── Compiler guards ───────────────────────────────────────────────────────
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
            CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14")
        message(FATAL_ERROR
                "[Vulkan] GCC 14+ required for C++23 / import std "
                "(you have GCC ${CMAKE_CXX_COMPILER_VERSION}). "
                "Disable with -DENABLE_VK_MODULE=OFF.")
    endif ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
            CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17")
        message(FATAL_ERROR
                "[Vulkan] Clang 17+ required for C++23 / import std "
                "(you have Clang ${CMAKE_CXX_COMPILER_VERSION}). "
                "Disable with -DENABLE_VK_MODULE=OFF.")
    endif ()
    if (MSVC AND MSVC_VERSION LESS 1936)
        message(FATAL_ERROR
                "[Vulkan] MSVC 19.36+ (VS 2022 17.6+) required for named modules.")
    endif ()

    # ── Locate SDK ────────────────────────────────────────────────────────────
    # Don't version-gate here — Vulkan::Vulkan may already exist from the
    # outer find_package in setup_vulkan. Just confirm the target is present.
    if (NOT TARGET Vulkan::Vulkan)
        find_package(Vulkan QUIET)
        if (NOT Vulkan_FOUND)
            message(FATAL_ERROR
                    "[Vulkan] SDK not found. Set VULKAN_SDK or install from lunarg.com.")
        endif ()
    endif ()
    message(STATUS "[Vulkan] SDK ${Vulkan_VERSION} at ${Vulkan_INCLUDE_DIR}")

    # Warn if below the version where :video partition first appeared
    if (Vulkan_VERSION VERSION_LESS "1.3.261")
        message(FATAL_ERROR
                "[Vulkan] SDK >= 1.3.261 required for vulkan.cppm "
                "(you have ${Vulkan_VERSION}).")
    endif ()

    # Normalise singular include path
    if (NOT Vulkan_INCLUDE_DIR)
        list(GET Vulkan_INCLUDE_DIRS 0 Vulkan_INCLUDE_DIR)
        set(Vulkan_INCLUDE_DIR "${Vulkan_INCLUDE_DIR}" CACHE PATH "" FORCE)
    endif ()

    set(_CPPM "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm")
    set(_VIDEO_CPPM "${Vulkan_INCLUDE_DIR}/vulkan/vulkan_video.cppm")

    if (NOT EXISTS "${_CPPM}")
        message(FATAL_ERROR "[Vulkan] vulkan.cppm not found at ${_CPPM}")
    endif ()

    # vulkan.cppm does `export import :video` unconditionally — the partition
    # MUST be in the same FILE_SET or GCC's BMI scanner can't resolve it.
    if (NOT EXISTS "${_VIDEO_CPPM}")
        message(FATAL_ERROR
                "[Vulkan] vulkan_video.cppm not found at ${_VIDEO_CPPM}\n"
                "vulkan.cppm unconditionally imports it as a partition. "
                "SDK >= 1.3.261 is required.")
    endif ()

    # ── Build the module target ───────────────────────────────────────────────
    add_library(Vulkan-HppModule STATIC)
    add_library(Vulkan::HppModule ALIAS Vulkan-HppModule)

    target_compile_definitions(Vulkan-HppModule PUBLIC
            VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
            VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
            VULKAN_HPP_USE_CXX_MODULE
    )

    # PRIVATE: the module interface itself needs to see the headers during
    # the global-module-fragment (#include) phase, but consumers get
    # everything via the module interface — they must NOT have these on
    # their include path or they'll try to #include vulkan.hpp directly.
    target_include_directories(Vulkan-HppModule PRIVATE "${Vulkan_INCLUDE_DIR}")
    target_link_libraries(Vulkan-HppModule PUBLIC Vulkan::Vulkan)

    target_compile_features(Vulkan-HppModule PUBLIC cxx_std_23)
    set_target_properties(Vulkan-HppModule PROPERTIES CXX_MODULE_STD ON)

    if (MSVC)
        target_compile_options(Vulkan-HppModule PUBLIC
                /std:c++latest
                /permissive-
                /Zc:preprocessor
                /translateInclude
        )
    endif ()

    # Both files in ONE FILE_SET — CMake's p1689r5 dependency scanner must
    # see the partition (.video) before it compiles the primary module unit.
    # Ordering within the list doesn't matter; the scanner handles it.
    target_sources(Vulkan-HppModule
            PUBLIC
            FILE_SET CXX_MODULES
            TYPE CXX_MODULES
            BASE_DIRS "${Vulkan_INCLUDE_DIR}"
            FILES
            "${_VIDEO_CPPM}"   # partition :video  — must be in same FILE_SET
            "${_CPPM}"         # primary unit      — export import :video
    )

    message(STATUS "[Vulkan] HppModule target created (C++23, both partitions)")
endfunction()

# -----------------------------------------------------------------------------
# Public API
# -----------------------------------------------------------------------------
option(ENABLE_VK_MODULE "Build Vulkan-Hpp as a C++23 named module" ON)

function(setup_vulkan target_name)

    # ── Step 1: Vulkan::Vulkan discovery (unchanged logic, trimmed) ───────────
    if (NOT TARGET Vulkan::Vulkan)
        find_package(Vulkan QUIET)
        if (NOT Vulkan_FOUND)
            message(FATAL_ERROR "[Vulkan] SDK not found. Set VULKAN_SDK or install from lunarg.com.")
        endif ()
    else ()
        message(STATUS "[Vulkan] Vulkan::Vulkan already present, skipping discovery")
    endif ()

    # ── Step 2: Module or header-only linkage ─────────────────────────────────
    if (ENABLE_VK_MODULE)
        _deer_ensure_vulkan_hpp_module()
        target_link_libraries(${target_name} PUBLIC Vulkan::HppModule)
        # Transparent to consumers – no USE_CPP20_MODULES ifdefs needed
    else ()
        # Lightweight interface: same alias, zero cost
        if (NOT TARGET Vulkan::HppModule)
            add_library(Vulkan-HppModule-Iface INTERFACE)
            add_library(Vulkan::HppModule ALIAS Vulkan-HppModule-Iface)
            target_link_libraries(Vulkan-HppModule-Iface INTERFACE Vulkan::Vulkan)
            target_compile_definitions(Vulkan-HppModule-Iface INTERFACE
                    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
                    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
                    VULKAN_HPP_USE_CXX_MODULE
            )
        endif ()
        target_link_libraries(${target_name} PUBLIC Vulkan::HppModule)
    endif ()

    # ── Step 2b: Platform surface defines ────────────────────────────────────
    if (WIN32)
        target_compile_definitions(Vulkan-HppModule PUBLIC VK_USE_PLATFORM_WIN32_KHR)
        target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_WIN32_KHR)
    elseif (APPLE)
        target_compile_definitions(Vulkan-HppModule PUBLIC VK_USE_PLATFORM_MACOS_MVK)
        target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_MACOS_MVK)
    elseif (ANDROID)
        target_compile_definitions(Vulkan-HppModule PUBLIC VK_USE_PLATFORM_ANDROID_KHR)
        target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_ANDROID_KHR)
    elseif (UNIX)
        # Runtime detection picks Wayland or X11, but we compile in support
        # for both so the same binary works on either compositor.
        # CMake doesn't know at configure-time which display server will be
        # running, so we probe for the headers and enable what's available.
        find_package(PkgConfig QUIET)

        if (PkgConfig_FOUND)
            pkg_check_modules(WAYLAND_CLIENT QUIET wayland-client)
            pkg_check_modules(XCB QUIET xcb)
            pkg_check_modules(X11 QUIET x11)
        else ()
            # Fallback: check for headers directly if pkg-config is absent
            find_path(WAYLAND_INCLUDE_DIR wayland-client.h
                    HINTS /usr/include /usr/local/include)
            find_path(XCB_INCLUDE_DIR xcb/xcb.h
                    HINTS /usr/include /usr/local/include)
            find_path(X11_INCLUDE_DIR X11/Xlib.h
                    HINTS /usr/include /usr/local/include)

            if (WAYLAND_INCLUDE_DIR)
                set(WAYLAND_CLIENT_FOUND TRUE)
            endif ()
            if (XCB_INCLUDE_DIR)
                set(XCB_FOUND TRUE)
            endif ()
            if (X11_INCLUDE_DIR)
                set(X11_FOUND TRUE)
            endif ()
        endif ()

        if (WAYLAND_CLIENT_FOUND)
            target_compile_definitions(Vulkan-HppModule PUBLIC VK_USE_PLATFORM_WAYLAND_KHR)
            target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_WAYLAND_KHR)
            message(STATUS "[Vulkan] Surface: Wayland enabled")
        endif ()
        if (XCB_FOUND)
            target_compile_definitions(Vulkan-HppModule PUBLIC VK_USE_PLATFORM_XCB_KHR)
            target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_XCB_KHR)
            message(STATUS "[Vulkan] Surface: XCB enabled")
        endif ()
        if (X11_FOUND)
            target_compile_definitions(Vulkan-HppModule PUBLIC VK_USE_PLATFORM_XLIB_KHR)
            target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_XLIB_KHR)
            message(STATUS "[Vulkan] Surface: Xlib enabled")
        endif ()

        if (NOT WAYLAND_CLIENT_FOUND AND NOT XCB_FOUND AND NOT X11_FOUND)
            message(FATAL_ERROR
                    "[Vulkan] No display server headers found. "
                    "Install libwayland-dev, libxcb-dev, or libx11-dev.")
        endif ()
    endif ()
    # ── Step 3: Own headers ───────────────────────────────────────────────────
    target_include_directories(${target_name} PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/source/api")

    # ── Step 4: glslang ───────────────────────────────────────────────────────
    if (TARGET Vulkan::glslang)
        target_link_libraries(${target_name} PUBLIC Vulkan::glslang)
        message(STATUS "[Vulkan] glslang: SDK target")
    else ()
        foreach (_ROOT
                "${CMAKE_SOURCE_DIR}/3rdparty/glslang"
                "${CMAKE_SOURCE_DIR}/third_party/glslang"
                "${CMAKE_SOURCE_DIR}/extern/glslang"
        )
            if (EXISTS "${_ROOT}/CMakeLists.txt" AND NOT TARGET glslang)
                set(GLSLANG_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
                add_subdirectory("${_ROOT}" "${CMAKE_BINARY_DIR}/_vendor/glslang" EXCLUDE_FROM_ALL)
                message(STATUS "[Vulkan] glslang: vendored at ${_ROOT}")
                break()
            endif ()
        endforeach ()
        if (TARGET glslang)
            target_link_libraries(${target_name} PUBLIC glslang SPIRV)
        else ()
            message(WARNING "[Vulkan] glslang not found – shader compilation unavailable.")
        endif ()
    endif ()

    # ── Step 5: SPIRV-Tools ───────────────────────────────────────────────────
    if (TARGET Vulkan::SPIRV-Tools)
        target_link_libraries(${target_name} PUBLIC Vulkan::SPIRV-Tools)
        message(STATUS "[Vulkan] SPIRV-Tools: SDK target")
    else ()
        find_package(SPIRV-Tools QUIET)
        if (SPIRV-Tools_FOUND)
            target_link_libraries(${target_name} PUBLIC SPIRV-Tools)
        else ()
            foreach (_ROOT
                    "${CMAKE_SOURCE_DIR}/3rdparty/SPIRV-Tools"
                    "${CMAKE_SOURCE_DIR}/third_party/SPIRV-Tools"
                    "${CMAKE_SOURCE_DIR}/extern/SPIRV-Tools"
            )
                if (EXISTS "${_ROOT}/CMakeLists.txt" AND NOT TARGET SPIRV-Tools)
                    add_subdirectory("${_ROOT}" "${CMAKE_BINARY_DIR}/_vendor/SPIRV-Tools" EXCLUDE_FROM_ALL)
                    message(STATUS "[Vulkan] SPIRV-Tools: vendored at ${_ROOT}")
                    break()
                endif ()
            endforeach ()
            if (TARGET SPIRV-Tools)
                target_link_libraries(${target_name} PUBLIC SPIRV-Tools)
            else ()
                message(WARNING "[Vulkan] SPIRV-Tools not found.")
            endif ()
        endif ()
    endif ()

    # ── Step 6: Windows DLL copy ──────────────────────────────────────────────
    if (WIN32)
        foreach (_CFG RELEASE RELWITHDEBINFO DEBUG "")
            if (_CFG)
                get_target_property(_VK_LIB Vulkan::Vulkan "IMPORTED_LOCATION_${_CFG}")
            else ()
                get_target_property(_VK_LIB Vulkan::Vulkan IMPORTED_LOCATION)
            endif ()
            if (_VK_LIB)
                break()
            endif ()
        endforeach ()

        if (_VK_LIB)
            get_filename_component(_VK_LIB_DIR "${_VK_LIB}" DIRECTORY)
            foreach (_CANDIDATE
                    "${_VK_LIB_DIR}/vulkan-1.dll"
                    "${_VK_LIB_DIR}/../Bin/vulkan-1.dll"
                    "${_VK_LIB_DIR}/../bin/vulkan-1.dll"
                    "${_VK_LIB_DIR}/../../Bin/vulkan-1.dll"
            )
                cmake_path(NORMAL_PATH _CANDIDATE OUTPUT_VARIABLE _N)
                if (EXISTS "${_N}")
                    add_custom_command(TARGET ${target_name} POST_BUILD
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_N}"
                            "$<TARGET_FILE_DIR:${target_name}>"
                            COMMENT "[Vulkan] Copying vulkan-1.dll"
                            VERBATIM)
                    break()
                endif ()
            endforeach ()
        endif ()
    endif ()

endfunction()
