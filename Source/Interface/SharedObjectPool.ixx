//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "utilities.hpp"

#include <bit>
#include <ranges>
#include <unordered_map>
#include <vector>

export module FawnVision.SharedObjectPool;

namespace FawnVision
{
struct SSharedObjectPoolBase
{
    SSharedObjectPoolBase()                                                          = default;
    virtual ~SSharedObjectPoolBase()                                                 = default;
    SSharedObjectPoolBase(const SSharedObjectPoolBase& other)                        = delete;
    SSharedObjectPoolBase(SSharedObjectPoolBase&& other) noexcept                    = delete;
    auto operator=(const SSharedObjectPoolBase& other) -> SSharedObjectPoolBase&     = delete;
    auto operator=(SSharedObjectPoolBase&& other) noexcept -> SSharedObjectPoolBase& = delete;


    virtual void Clear() = 0;
};
export template <class T>
struct SSharedObjectPool final : SSharedObjectPoolBase
{
    SSharedObjectPool() = default;
    std::vector<T*> pool{};
    void Clear() override
    {
        for (auto element : pool)
        {
            delete element;
        }
        pool.clear();
    }
};
export struct RenderGraphObjectPool
{
    std::unordered_map<std::size_t, SSharedObjectPoolBase*> allocatedPools;
};

export template <typename T>
T* Get(SSharedObjectPool<T>* pool)
{
    if (pool->pool.empty())
    {
        pool->pool.emplace_back(new T{});
    }
    T* obj = pool->pool.back();
    pool->pool.pop_back();
    return obj;
}

export template <typename T>
void Release(SSharedObjectPool<T>* pool, T* obj)
{
    pool->pool.emplace_back(obj);
}

export template <typename T, typename K = T>
T* Get(RenderGraphObjectPool& renderPool)
{
    constexpr std::size_t key{Balbino::Hash<K>()};
    if (!renderPool.allocatedPools.contains(key))
    {
        renderPool.allocatedPools[key] = new SSharedObjectPool<T>();
    }
    return Get(std::bit_cast<SSharedObjectPool<T>*>(renderPool.allocatedPools[key]));
}

export template <typename T>
void Release(RenderGraphObjectPool& renderPool, T* value)
{
    if (constexpr std::size_t key{Balbino::Hash<T>()}; renderPool.allocatedPools.contains(key))
    {
        Release(std::bit_cast<SSharedObjectPool<T>*>(renderPool.allocatedPools[key]), value);
    }
}

export void Cleanup(RenderGraphObjectPool& renderPool)
{
    for (const auto& objectPoolBase : renderPool.allocatedPools | std::views::values)
    {
        objectPoolBase->Clear();
    }
    renderPool.allocatedPools.clear();
}

} // namespace FawnVision
