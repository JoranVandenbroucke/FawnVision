//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "Versions.hpp"

#include <cstdint>
#include <functional>
#include <vector>

export module FawnVision.RenderPass;

namespace FawnVision
{
class CSharedObjectPoolBase
{
public:
    virtual ~CSharedObjectPoolBase() = default;
    CSharedObjectPoolBase(const CSharedObjectPoolBase& other) = delete;
    CSharedObjectPoolBase(CSharedObjectPoolBase&& other) BALBINO_NOEXCEPT_SINCE_CXX11 = delete;
    auto operator=(const CSharedObjectPoolBase& other) -> CSharedObjectPoolBase& = delete;
    auto operator=(CSharedObjectPoolBase&& other) BALBINO_NOEXCEPT_SINCE_CXX11 -> CSharedObjectPoolBase& = delete;
};

template <class T>
struct SSharedObjectPool final : CSharedObjectPoolBase
{
    SSharedObjectPool() = default;
    std::vector<std::shared_ptr<T>> pool{};
};

enum class depth_acces :uint8_t
{
    read = 1 << 0,
    write = 1 << 1,
    read_write = read | write
};

enum class access_flags :uint8_t
{
    none = 0,
    read = 1 << 0,
    write = 1 << 1,
    discard = 1 << 2,
    write_all = write | discard,
    read_write = read | write
};

struct SRenderPass
{
};

template <class RenderPass>
struct SRenderPassBase : SRenderPass
{
    bool allowGlobalState{false};
    bool allowPassCulling{true};
    bool allowRendererListCulling{true};
    bool enableAsyncCompute{false};
    bool enableFoveatedRasterization{false};
    bool isCompute{false};
    int index{-1};
    std::function<void(RenderPass&)> renderFunction;
    RenderPass data{};
};

export template <class RenderPass>
struct SComputePass final : SRenderPassBase<RenderPass>
{

};

export template <class RenderPass>
struct SRasterPass final : SRenderPassBase<RenderPass>
{

};

inline std::unordered_map<uint32_t, std::shared_ptr<CSharedObjectPoolBase>> g_allocatedPools{};
inline std::list<std::shared_ptr<SRenderPass>> g_renderPasses{};

template <class T>
BALBINO_CONSTEXPR_SINCE_CXX11 auto Allocate(SSharedObjectPool<T>& sharedObjectPool) -> std::shared_ptr<T>
{
    if (sharedObjectPool.pool.empty())
    {
        return std::make_shared<T>();
    }
    auto obj = sharedObjectPool.pool.back();
    sharedObjectPool.pool.pop_back();
    return obj;
}

template <class T>
BALBINO_CONSTEXPR_SINCE_CXX11 auto Free(SSharedObjectPool<T>& sharedObjectPool, std::shared_ptr<T> obj)
{
    sharedObjectPool.push_back(obj);
}

template <class RenderPass>
BALBINO_CONSTEXPR_SINCE_CXX11 auto Initialize(SRenderPassBase<RenderPass>& renderPass, const bool isComputePass, const int passIndex, const RenderPass& passData)
{
    renderPass.isCompute = isComputePass;
    renderPass.index = passIndex;
    renderPass.data = passData;
}

template <class RenderPass>
BALBINO_CONSTEXPR_SINCE_CXX11 auto Cleanup(SRenderPassBase<RenderPass>& renderPass)
{
    renderPass.allowGlobalState = false;
    renderPass.allowPassCulling = true;
    renderPass.allowRendererListCulling = true;
    renderPass.enableAsyncCompute = false;
    renderPass.enableFoveatedRasterization = false;

    renderPass.index = -1;
}

template <class RenderPass>
void Execute(SRenderPassBase<RenderPass>& renderPass)
{
    renderPass.renderFunction(renderPass.data);
}

template <class RenderPass>
void Release(SRenderPassBase<RenderPass>& renderPass)
{
    g_renderPasses.remove(renderPass);
    Cleanup(renderPass);
}

template <class PassData>
BALBINO_CONSTEXPR auto AddComputerRenderPass(PassData& passData)
{
    BALBINO_CONSTEXPR uint32_t rasterPassKey{Balbino::Hash<SRasterPass<PassData>>()};
    BALBINO_CONSTEXPR uint32_t passKey{Balbino::Hash<SRasterPass<PassData>>()};
    std::shared_ptr<SComputePass<PassData>> renderPass = Allocate<SRasterPass<PassData>>(g_allocatedPools[rasterPassKey]);
    Initialize(renderPass, g_renderPasses.size(), Allocate<PassData>(g_allocatedPools[passKey]), false);

    passData = renderPass.data;

    g_renderPasses.emplace_back(renderPass);

    return renderPass;
}

template <class PassData>
BALBINO_CONSTEXPR auto AddRasterRenderPass(PassData& passData)
{
    BALBINO_CONSTEXPR uint32_t rasterPassKey{Balbino::Hash<SRasterPass<PassData>>()};
    BALBINO_CONSTEXPR uint32_t passKey{Balbino::Hash<SRasterPass<PassData>>()};
    std::shared_ptr<SRasterPass<PassData>> renderPass = Allocate<SRasterPass<PassData>>(g_allocatedPools[rasterPassKey]);
    Initialize(renderPass, g_renderPasses.size(), Allocate<PassData>(g_allocatedPools[passKey]), false);

    passData = renderPass.data;

    g_renderPasses.emplace_back(renderPass);

    return renderPass;
}
// todo, allow the rander graph to be used
} // namespace FawnVision