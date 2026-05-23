//
// Copyright (c) 2024.
// Author: Joran.
//

module;
export module FawnVision:Mesh;
import :Buffer;
import :Enum;
import :Renderer;
import FawnAlgebra;

import std;

namespace fawn_vision
{

export struct Mesh
{
    Buffer indexBuffer{};
    Buffer vertexBuffer{};
    std::uint32_t indexCount{0U};
    std::uint32_t vertexCount{0U};
};

// ---------------------------------------------------------------------------
// Internal helpers — not exported
// ---------------------------------------------------------------------------

template <std::integral Integer, std::size_t IE = std::dynamic_extent>
[[nodiscard]] auto CreateIndexBuffer(const Renderer& renderer, Mesh& mesh, const std::span<const Integer, IE> indices) noexcept -> gfx_status
{
    const std::uint64_t byteSize = static_cast<std::uint64_t>(indices.size()) * sizeof(Integer);

    if (Initialize(renderer, byteSize, buffer_usage::index_buffer, memory_property::host_visible | memory_property::host_coherent, mesh.indexBuffer) != gfx_status::ok)
    {
        return gfx_status::not_ok;
    }

    CopyData<Integer, IE>(renderer, mesh.indexBuffer, indices);
    mesh.indexCount = static_cast<std::uint32_t>(indices.size());
    return gfx_status::ok;
}

template <typename Vertex, std::size_t VE = std::dynamic_extent>
[[nodiscard]] auto CreateVertexBuffer(const Renderer& renderer, Mesh& mesh, const std::span<const Vertex, VE> vertices) noexcept -> gfx_status
{
    const std::uint64_t byteSize = static_cast<std::uint64_t>(vertices.size()) * sizeof(Vertex);

    if (Initialize(renderer, byteSize, buffer_usage::vertex_buffer, memory_property::host_visible | memory_property::host_coherent, mesh.vertexBuffer) != gfx_status::ok)
    {
        return gfx_status::not_ok;
    }

    CopyData<Vertex, VE>(renderer, mesh.vertexBuffer, vertices);
    mesh.vertexCount = static_cast<std::uint32_t>(vertices.size());
    return gfx_status::ok;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

export template <std::integral Integer, std::size_t IE = std::dynamic_extent, typename Vertex, std::size_t VE = std::dynamic_extent>
[[nodiscard]] auto Initialize(const Renderer& renderer, Mesh& mesh, const std::span<const Integer, IE> indices, const std::span<const Vertex, VE> vertices) noexcept -> gfx_status
{
    if (CreateIndexBuffer<Integer, IE>(renderer, mesh, indices) != gfx_status::ok) [[unlikely]]
    {
        return gfx_status::not_ok;
    }
    if (CreateVertexBuffer<Vertex, VE>(renderer, mesh, vertices) != gfx_status::ok) [[unlikely]]
    {
        Cleanup(renderer, mesh.indexBuffer);
        return gfx_status::not_ok;
    }

    return gfx_status::ok;
}

export inline auto Cleanup(const Renderer& renderer, Mesh& mesh) noexcept -> void
{
    Cleanup(renderer, mesh.indexBuffer);
    Cleanup(renderer, mesh.vertexBuffer);
    mesh.indexCount  = 0U;
    mesh.vertexCount = 0U;
}

export template <std::integral Integer, std::size_t IE = std::dynamic_extent>
[[nodiscard]] auto RecreateIndexBuffer(const Renderer& renderer, Mesh& mesh, const std::span<const Integer, IE> indices) noexcept -> gfx_status
{
    Cleanup(renderer, mesh.indexBuffer);
    mesh.indexCount = 0U;
    return CreateIndexBuffer<Integer, IE>(renderer, mesh, indices);
}

export template <typename Vertex, std::size_t VE = std::dynamic_extent>
[[nodiscard]] auto RecreateVertexBuffer(const Renderer& renderer, Mesh& mesh, const std::span<const Vertex, VE> vertices) noexcept -> gfx_status
{
    Cleanup(renderer, mesh.vertexBuffer);
    mesh.vertexCount = 0U;
    return CreateVertexBuffer<Vertex, VE>(renderer, mesh, vertices);
}
} // namespace fawn_vision
