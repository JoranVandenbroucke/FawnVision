//
// Created by joran on 5/4/26.
//

#pragma once
#include "../deer_vulkan_core.hpp"

namespace deer_vulkan
{
struct Dispatch
{
    vk::detail::DynamicLoader loader{};
    vk::detail::DispatchLoaderDynamic dispatch{};
};

inline void Initialize(Dispatch& dispatch) noexcept
{
    dispatch.dispatch.init(dispatch.loader);
}
} // namespace deer_vulkan