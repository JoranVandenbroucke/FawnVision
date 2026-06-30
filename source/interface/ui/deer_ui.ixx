//
// Copyright (c) 2024.
// Author: Joran.
//

module;
#include "shaders.hpp"

export module DeerUI;
import FawnAlgebra;
import FawnVision;
import std;

using namespace fawn_algebra;

namespace deer_ui
{
namespace detail
{
#pragma pack(push, 1)
template <typename Member, std::size_t O>
struct Pad
{
    char pad[O];
    Member m;
};
#pragma pack(pop)

template <typename Member>
struct Pad<Member, 0>
{
    Member m;
};

template <typename Base, typename Member, std::size_t O>
struct MakeUnion
{
#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4324) // structure padded due to alignment specifier
#elif defined(__GNUC__) || defined(__clang__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpadded"
#endif
    union U
    {
        char c{};
        Base base;
        Pad<Member, O> pad;
    };
#if defined(_MSC_VER)
#    pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#    pragma GCC diagnostic pop
#endif
    constexpr static U u{};
};

template <typename Member, typename Base, typename Orig>
struct offset_of_impl
{
    template <std::size_t off, auto union_part = &MakeUnion<Base, Member, off>::u>
    static constexpr std::ptrdiff_t offset2(Member Orig::* member)
    {
        if constexpr (off > sizeof(Base))
        {
            throw 1;
        }
        else
        {
            const auto diff1{&(static_cast<const Orig*>(&union_part->base)->*member)};
            const auto diff2{&union_part->pad.m};
            if (diff1 > diff2)
            {
                constexpr auto MIN{sizeof(Member) < alignof(Orig) ? sizeof(Member) : alignof(Orig)};
                return offset2<off + MIN>(member);
            }
            return off;
        }
    }
};

template <class Member, class Base>
std::tuple<Member, Base> get_types(Member Base::*);

template <class TheBase = void, class TT>
constexpr std::ptrdiff_t offset_of(TT member)
{
    using T      = decltype(get_types(std::declval<TT>()));
    using Member = std::tuple_element_t<0, T>;
    using Orig   = std::tuple_element_t<1, T>;
    using Base   = std::conditional_t<std::is_void_v<TheBase>, Orig, TheBase>;
    return offset_of_impl<Member, Base, Orig>::template offset2<0>(member);
}

template <auto member, class TheBase = void>
constexpr std::ptrdiff_t offset_of()
{
    return offset_of<TheBase>(member);
}
} // namespace detail

// ---------------------------------------------------------------------------
// Handles
// ---------------------------------------------------------------------------

export using CanvasHandle    = std::uint16_t;
export using UIElementHandle = std::uint16_t;

export inline constexpr CanvasHandle invalidCanvas{static_cast<std::uint16_t>(~0U)};
export inline constexpr UIElementHandle invalidElement{static_cast<std::uint16_t>(~0U)};

// ---------------------------------------------------------------------------
// Dirty flags
// ---------------------------------------------------------------------------

enum class DirtyFlags : std::uint8_t
{
    none     = 0,
    layout   = 1 << 0,
    style    = 1 << 1,
    interact = 1 << 2,
    all      = layout | style | interact,
};

[[nodiscard]] constexpr auto operator|(DirtyFlags a, DirtyFlags b) noexcept -> DirtyFlags
{
    return static_cast<DirtyFlags>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

[[nodiscard]] constexpr auto operator&(DirtyFlags a, DirtyFlags b) noexcept -> DirtyFlags
{
    return static_cast<DirtyFlags>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

constexpr auto ClearFlag(DirtyFlags v, DirtyFlags f) noexcept -> DirtyFlags
{
    return static_cast<DirtyFlags>(static_cast<std::uint8_t>(v) & ~static_cast<std::uint8_t>(f));
}

constexpr auto HasDirty(const DirtyFlags v, const DirtyFlags f) noexcept -> bool
{
    return (v & f) == f;
}

// ---------------------------------------------------------------------------
// Canvas flags
// ---------------------------------------------------------------------------

export enum class canvas_flag : std::uint16_t {
    none                 = 0,
    fullscreen           = 1 << 0,
    hidden               = 1 << 1,
    menu_bar             = 1 << 2,
    background           = 1 << 3,
    resizable            = 1 << 4,
    movable              = 1 << 5,
    accepts_inputs       = 1 << 6,
    modal                = 1 << 7,
    always_on_top        = 1 << 8,
    focus_on_appearing   = 1 << 9,
    vertical_scrollbar   = 1 << 10,
    horizontal_scrollbar = 1 << 11,
};

export [[nodiscard]] constexpr auto operator|(canvas_flag a, canvas_flag b) noexcept -> canvas_flag
{
    return static_cast<canvas_flag>(static_cast<std::uint16_t>(a) | static_cast<std::uint16_t>(b));
}

export [[nodiscard]] constexpr auto operator&(canvas_flag a, canvas_flag b) noexcept -> canvas_flag
{
    return static_cast<canvas_flag>(static_cast<std::uint16_t>(a) & static_cast<std::uint16_t>(b));
}

export constexpr auto HasFlag(const canvas_flag v, const canvas_flag f) noexcept -> bool
{
    return (v & f) == f;
}

// ---------------------------------------------------------------------------
// Layout
// ---------------------------------------------------------------------------

export struct LayoutProperties
{
    ushort2 padding{};
    ushort2 margin{};
    ushort2 size{}; // 0 = fill parent on that axis
    uchar2 border{};
    uchar2 flex{}; // flex-grow per axis
};

// ---------------------------------------------------------------------------
// Components — stored in parallel SOA vectors per Canvas
//
// Every component is optional — presence is tracked by a parallel
// std::vector<bool> (or presence sentinel) rather than std::optional<T>
// inline in UIElement, keeping the node struct small and the component
// data in contiguous arrays.
//
// Index into component vectors == UIElementHandle.
// ---------------------------------------------------------------------------

// ── Click component ────────────────────────────────────────────────────────
struct ClickComponent
{
    std::function<void()> onPress{};
    std::function<void()> onRelease{};
    std::function<void()> onHover{};
    std::function<void()> onDoubleClick{};
};

// ── Image component ────────────────────────────────────────────────────────
// Either a standalone Texture or a UV rect into a shared atlas.
// Exactly one of the two paths is active — indicated by useAtlas.
struct ImageComponent
{
    // Atlas path: a rect in normalized [0,1] UV space within the atlas.
    float4 uvRect{0.0F, 0.0F, 1.0F, 1.0F}; // (u0, v0, u1, v1)
    // Standalone path: texture owned externally, non-owning view.
    const fawn_vision::Texture* texture{nullptr};
    bool useAtlas{false};
};

// ── Text component — stubbed, fill in when font atlas is ready ─────────────
struct TextComponent
{
    std::string text{};
    float fontSize{14.0F};
    float4 color{1.0F, 1.0F, 1.0F, 1.0F};
    // TODO: font handle, alignment, wrapping
};

// ---------------------------------------------------------------------------
// UIElement — the pure node (tree linkage + layout + interaction state)
// No component data lives here — keeps this 32 bytes
// ---------------------------------------------------------------------------

struct UIElement
{
    UIElementHandle parent{invalidElement};
    UIElementHandle firstChild{invalidElement};
    std::uint8_t childCount{0};

    LayoutProperties layoutProperties{};
    ushort4 computedBox{};
    float4 backgroundColor{1.0F, 1.0F, 1.0F, 1.0F};

    bool hovered{false};
    bool pressed{false};
    bool enabled{true};
    DirtyFlags dirty{DirtyFlags::all};
};

// ---------------------------------------------------------------------------
// ComponentStore — SOA storage, indexed by UIElementHandle
//
// Each array is exactly canvas.elements.size() long.
// A null std::optional / null pointer / empty string signals absence.
// ---------------------------------------------------------------------------

struct ComponentStore
{
    // Parallel to UIElement pool — index == UIElementHandle
    std::vector<std::optional<ClickComponent>> click{};
    std::vector<std::optional<ImageComponent>> image{};
    std::vector<std::optional<TextComponent>> text{};

    auto Resize(const std::size_t n) -> void
    {
        click.resize(n);
        image.resize(n);
        text.resize(n);
    }

    auto InsertAt(const UIElementHandle idx) -> void
    {
        const auto it{static_cast<std::ptrdiff_t>(idx)};
        click.insert(click.begin() + it, std::nullopt);
        image.insert(image.begin() + it, std::nullopt);
        text.insert(text.begin() + it, std::nullopt);
    }

    [[nodiscard]] auto Size() const noexcept -> std::size_t
    {
        return click.size();
    }
};

// ---------------------------------------------------------------------------
// Canvas — flat element pool + SOA component store
// ---------------------------------------------------------------------------

struct Canvas
{
    std::vector<UIElement> elements{};
    ComponentStore components;

    LayoutProperties layoutProperties{};
    ushort4 boundingBox{}; // (x, y, w, h) in screen pixels
    std::uint8_t layer{0};
    canvas_flag flags{canvas_flag::none};

    bool layoutDirty{true};
    bool instancesDirty{true};
};

// ---------------------------------------------------------------------------
// GPU instance — one per visible element per draw call batch
// ---------------------------------------------------------------------------

struct alignas(16) UIInstanceData
{
    float2 scale{1.0F, 1.0F};
    float2 translate{0.0F, 0.0F};
    float4 color{1.0F, 1.0F, 1.0F, 1.0F};
    float4 uvRect{0.0F, 0.0F, 1.0F, 1.0F}; // image UV rect; (0,0,1,1) = no image
};

// ---------------------------------------------------------------------------
// Quad geometry
// ---------------------------------------------------------------------------

struct Vertex
{
    float2 position{};
    float2 uv{};
};

inline constexpr std::array quadVertices{
    Vertex{{0.0F, 0.0F}, {0.0F, 0.0F}},
    Vertex{{0.0F, 1.0F}, {0.0F, 1.0F}},
    Vertex{{1.0F, 1.0F}, {1.0F, 1.0F}},
    Vertex{{1.0F, 0.0F}, {1.0F, 0.0F}},
};
inline constexpr std::array quadIndices{0U, 1U, 2U, 2U, 3U, 0U};
inline constexpr std::uint32_t maxInstancesPerBatch{64U};

// ---------------------------------------------------------------------------
// Render graph pass data
// ---------------------------------------------------------------------------

struct PassData
{
    fawn_vision::Shader shader{};
    fawn_vision::Mesh mesh{};
    fawn_vision::Buffer instanceBuffer{};
};

// ---------------------------------------------------------------------------
// UIRenderer
// ---------------------------------------------------------------------------

export struct UIRenderer
{
    std::vector<Canvas> canvases{};
    PassData* renderData{nullptr};

    // Per-canvas cached instance lists — rebuilt only when dirty.
    // Index matches canvases[i].
    std::vector<std::vector<UIInstanceData>> cachedInstances{};

    bool anyDirty{true};
};

// ---------------------------------------------------------------------------
// Internal — layout pass
// ---------------------------------------------------------------------------

namespace detail
{

[[nodiscard]] constexpr auto ComputeBox(const ushort2& parentSize, const ushort2& cursor, const LayoutProperties& layout) noexcept -> ushort4
{
    const std::uint16_t w{layout.size.x != 0U ? layout.size.x : static_cast<std::uint16_t>(parentSize.x - static_cast<std::uint16_t>((layout.margin.x + layout.padding.x) * 2U))};
    const std::uint16_t h{layout.size.y != 0U ? layout.size.y : static_cast<std::uint16_t>(parentSize.y - static_cast<std::uint16_t>((layout.margin.y + layout.padding.y) * 2U))};
    const std::uint16_t x{static_cast<std::uint16_t>(cursor.x + layout.margin.x + layout.padding.x)};
    const std::uint16_t y{static_cast<std::uint16_t>(cursor.y + layout.margin.y + layout.padding.y)};
    return {x, y, w, h};
}

// Depth-first layout traversal — only visits dirty nodes.
void LayoutSubtree(Canvas& canvas, const UIElementHandle nodeIdx, const ushort2& parentPos, const ushort2& parentSize) noexcept
{
    UIElement& node{canvas.elements[nodeIdx]};

    if (HasDirty(node.dirty, DirtyFlags::layout))
    {
        node.computedBox = ComputeBox(parentSize, parentPos, node.layoutProperties);
        node.dirty       = ClearFlag(node.dirty, DirtyFlags::layout);
    }

    ushort2 cursor{node.computedBox.x, node.computedBox.y};

    for (std::uint8_t ci{}; ci < node.childCount; ++ci)
    {
        const UIElementHandle childIdx{static_cast<UIElementHandle>(node.firstChild + ci)};
        LayoutSubtree(canvas, childIdx, cursor, {node.computedBox.z, node.computedBox.w});

        // Advance cursor for next sibling (simple vertical flow)
        const UIElement& child{canvas.elements[childIdx]};
        cursor.y = static_cast<std::uint16_t>(child.computedBox.y + child.computedBox.w + child.layoutProperties.margin.y + child.layoutProperties.padding.y);
    }
}

void LayoutCanvas(Canvas& canvas) noexcept
{
    if (!canvas.layoutDirty)
    {
        return;
    }

    ushort2 cursor{canvas.boundingBox.x, canvas.boundingBox.y};
    const ushort2 canvasSize{canvas.boundingBox.z, canvas.boundingBox.w};

    for (UIElementHandle i{}; i < static_cast<UIElementHandle>(canvas.elements.size()); ++i)
    {
        if (canvas.elements[i].parent != invalidElement)
        {
            continue;
        }

        LayoutSubtree(canvas, i, cursor, canvasSize);

        const UIElement& root{canvas.elements[i]};
        cursor.y = static_cast<std::uint16_t>(root.computedBox.y + root.computedBox.w + root.layoutProperties.margin.y + root.layoutProperties.padding.y);
    }

    canvas.layoutDirty = false;
}

// Rebuilds the instance list for one canvas.
// Only called when canvas.instancesDirty is true.
void RebuildCanvasInstances(Canvas& canvas, std::vector<UIInstanceData>& out, const float sx, const float sy) noexcept
{
    out.clear();

    for (UIElementHandle i{}; i < static_cast<UIElementHandle>(canvas.elements.size()); ++i)
    {
        const UIElement& el{canvas.elements[i]};

        // Compute tinted colour from interaction state
        float4 color = el.backgroundColor;
        if (!el.enabled)
        {
            color.w *= 0.4F;
        }
        else if (el.pressed)
        {
            color = color * float4{0.7F, 0.7F, 0.7F, 1.0F};
        }
        else if (el.hovered)
        {
            color = color * float4{1.15F, 1.15F, 1.15F, 1.0F};
        }

        // UV rect: from image component if present, else full (0,0,1,1)
        float4 uvRect{0.0F, 0.0F, 1.0F, 1.0F};
        if (const auto& img{canvas.components.image[i]})
        {
            uvRect = img->uvRect;
            // If standalone texture: uvRect is still (0,0,1,1) unless the
            // caller set it — the descriptor binding is handled separately.
        }

        out.push_back({
            .scale     = {sx * el.computedBox.z, sy * el.computedBox.w},
            .translate = {-1.0F + el.computedBox.x * sx, -1.0F + el.computedBox.y * sy},
            .color     = color,
            .uvRect    = uvRect,
        });
    }

    canvas.instancesDirty = false;
}

void Render(UIRenderer& ui, const PassData* pass, const fawn_vision::RenderPassContext& ctx)
{
    const float sw{static_cast<float>(ctx.swapChain.extent.width)};
    const float sh{static_cast<float>(ctx.swapChain.extent.height)};
    const float sx{2.0F / sw};
    const float sy{2.0F / sh};

    if (ui.anyDirty)
    {
        std::ranges::stable_sort(ui.canvases,
                                 [](const Canvas& a, const Canvas& b)
                                 {
                                     return a.layer < b.layer;
                                 });
        ui.anyDirty = false;
    }

    for (std::size_t ci{}; ci < ui.canvases.size(); ++ci)
    {
        Canvas& canvas{ui.canvases[ci]};
        LayoutCanvas(canvas);

        if (canvas.instancesDirty)
        {
            RebuildCanvasInstances(canvas, ui.cachedInstances[ci], sx, sy);
        }
    }

    // ── Pipeline state — set once per frame ──────────────────────────────
    using namespace fawn_vision;

    BindShader(ctx, pass->shader);
    SetViewport(ctx, 0.0F, 0.0F, sw, sh);
    SetScissor(ctx, static_cast<std::uint32_t>(sw), static_cast<std::uint32_t>(sh), 0, 0);

    SetAlphaToCoverageEnable(ctx, false);
    SetColorBlendEnable(ctx, true);
    SetColorBlendEquation(ctx, blend_factor::src_alpha, blend_factor::one_minus_src_alpha, blend_operator::add, blend_factor::one, blend_factor::one_minus_src_alpha,
                          blend_operator::add);
    SetColorWriteMask(ctx, color_component::r | color_component::g | color_component::b | color_component::a);

    SetPolygonMode(ctx, polygon_mode::fill);
    SetCullMode(ctx, cull_mode::none);
    SetFrontFace(ctx, false);
    SetLineWidth(ctx, 1.0F);
    SetDepthBiasEnable(ctx, false);
    SetDepthTestEnable(ctx, false);
    SetDepthWriteEnable(ctx, false);
    SetDepthCompareOperator(ctx, compare_operator::always);
    SetPrimitiveRestartEnable(ctx, false);
    SetPrimitiveTopology(ctx, primitive_topology::triangle_list);
    SetRasterizationSamples(ctx, 1U);
    SetRasterizerDiscardEnable(ctx, false);
    SetStencilTestEnable(ctx, false);

    constexpr std::array attributes{
        VertexAttributes{0, 0, format::r32g32_sfloat, static_cast<std::uint32_t>(offset_of(&Vertex::position)), false},
        VertexAttributes{1, 0, format::r32g32_sfloat, static_cast<std::uint32_t>(offset_of(&Vertex::uv)), false},
        VertexAttributes{2, 1, format::r32g32_sfloat, static_cast<std::uint32_t>(offset_of(&UIInstanceData::scale)), true},
        VertexAttributes{3, 1, format::r32g32_sfloat, static_cast<std::uint32_t>(offset_of(&UIInstanceData::translate)), true},
        VertexAttributes{4, 1, format::r32g32b32a32_sfloat, static_cast<std::uint32_t>(offset_of(&UIInstanceData::color)), true},
        VertexAttributes{5, 1, format::r32g32b32a32_sfloat, static_cast<std::uint32_t>(offset_of(&UIInstanceData::uvRect)), true},
    };
    SetVertexInput(ctx, std::span(attributes), sizeof(Vertex), sizeof(UIInstanceData));
    BindMesh(ctx, pass->mesh);

    // ── Draw each canvas ──────────────────────────────────────────────────
    for (std::size_t ci{}; ci < ui.canvases.size(); ++ci)
    {
        if (HasFlag(ui.canvases[ci].flags, canvas_flag::hidden))
        {
            continue;
        }

        const std::vector<UIInstanceData>& instances{ui.cachedInstances[ci]};
        if (instances.empty())
        {
            continue;
        }

        const std::size_t total{instances.size()};
        for (std::size_t i{}; i < total; i += maxInstancesPerBatch)
        {
            const std::uint32_t batchSize{static_cast<std::uint32_t>(std::min<std::size_t>(maxInstancesPerBatch, total - i))};

            CopyData(ctx, pass->instanceBuffer, std::span(instances.data() + i, batchSize));

            BindBuffer(ctx, pass->instanceBuffer);

            Draw(ctx, static_cast<std::uint32_t>(quadIndices.size()), batchSize, 0U, 0U);
        }
    }
}
} // namespace detail

// ---------------------------------------------------------------------------
// Internal — canvas element insertion helper
// ---------------------------------------------------------------------------

namespace detail
{
[[nodiscard]] auto AppendElement(Canvas& canvas, UIElement&& el) noexcept -> UIElementHandle
{
    const UIElementHandle parentHandle{el.parent};

    UIElementHandle insertAt;

    if (parentHandle == invalidElement)
    {
        // Root element — just append at end
        insertAt = static_cast<UIElementHandle>(canvas.elements.size());
        canvas.elements.push_back(std::move(el));
        canvas.components.Resize(canvas.elements.size());
    }
    else
    {
        const UIElement& parent{canvas.elements[parentHandle]};

        // Insert position: immediately after last existing child of parent.
        // If no children yet, insert right after parent itself.
        insertAt = parent.childCount == 0 ? static_cast<UIElementHandle>(parentHandle + 1U) : static_cast<UIElementHandle>(parent.firstChild + parent.childCount);

        canvas.elements.insert(canvas.elements.begin() + insertAt, std::move(el));
        canvas.components.InsertAt(insertAt);

        // Fix up all handles that shifted due to the insert
        for (UIElementHandle i{}; i < static_cast<UIElementHandle>(canvas.elements.size()); ++i)
        {
            UIElement& e{canvas.elements[i]};
            if (e.parent != invalidElement && e.parent >= insertAt)
            {
                ++e.parent;
            }
            if (e.firstChild != invalidElement && e.firstChild >= insertAt)
            {
                ++e.firstChild;
            }
        }

        // Wire into parent
        if (parent.childCount == 0)
        {
            canvas.elements[parentHandle].firstChild = insertAt;
        }
        ++canvas.elements[parentHandle].childCount;
    }

    return insertAt;
}
} // namespace detail

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

export [[nodiscard]] inline auto Initialize(const fawn_vision::Renderer& renderer, fawn_vision::RenderGraph& renderGraph, UIRenderer& ui) noexcept -> std::int32_t
{
    fawn_vision::RenderPassHandle pass{fawn_vision::AddRasterRenderPass<PassData>(renderGraph, ui.renderData)};

    const std::array shaderData{
        fawn_vision::ShaderData{fawn_vision::shader_stage::vertex, {g_uiVert, g_uiVert + g_uiVertSize}},
        fawn_vision::ShaderData{fawn_vision::shader_stage::fragment, {g_uiFrag, g_uiFrag + g_uiFragSize}},
    };

    if (fawn_vision::CreateShader(renderer, std::span(shaderData), ui.renderData->shader) != fawn_vision::gfx_status::ok) [[unlikely]]
    {
        return -1;
    }

    if (fawn_vision::Initialize(renderer, ui.renderData->mesh, std::span(quadIndices), std::span(quadVertices)) != fawn_vision::gfx_status::ok) [[unlikely]]
    {
        return -1;
    }

    if (fawn_vision::Initialize(renderer, maxInstancesPerBatch * sizeof(UIInstanceData), fawn_vision::buffer_usage::vertex_buffer,
                                fawn_vision::memory_property::host_visible | fawn_vision::memory_property::host_coherent, ui.renderData->instanceBuffer)
        != fawn_vision::gfx_status::ok) [[unlikely]]
    {
        return -1;
    }

    //todo: find a better way to set syncMode and signalValue
    renderGraph.passes[pass.index]->syncMode = fawn_vision::SYNC_WAIT_SIGNAL;
    renderGraph.passes[pass.index]->signalValue = 1;

    fawn_vision::SetRenderFunc<PassData>(renderGraph, pass,
                                         [&ui](const PassData* pPass, const fawn_vision::RenderPassContext& ctx)
                                         {
                                             detail::Render(ui, pPass, ctx);
                                         });

    return 0;
}

export inline auto Cleanup(const fawn_vision::Renderer& renderer, UIRenderer& ui) noexcept -> void
{
    if (ui.renderData)
    {
        fawn_vision::Cleanup(renderer, ui.renderData->instanceBuffer);
        fawn_vision::Cleanup(renderer, ui.renderData->mesh);
        fawn_vision::Cleanup(renderer, ui.renderData->shader);
        ui.renderData = nullptr;
    }
    ui.canvases.clear();
    ui.cachedInstances.clear();
}

// ── Canvas ─────────────────────────────────────────────────────────────────

export inline auto NotifyWindowResized(UIRenderer& ui) noexcept -> void
{
    for (Canvas& canvas : ui.canvases)
    {
        canvas.layoutDirty    = true;
        canvas.instancesDirty = true;
    }
    ui.anyDirty = true;
}

export inline auto CreateCanvas(UIRenderer& ui, const ushort2& position, const ushort2& size, const std::uint8_t layer, const canvas_flag flags, CanvasHandle& outHandle) noexcept
    -> void
{
    const ushort2 actualSize{
        size.x != 0U ? size.x : static_cast<std::uint16_t>(1920U - position.x),
        size.y != 0U ? size.y : static_cast<std::uint16_t>(1080U - position.y),
    };
    outHandle = static_cast<CanvasHandle>(ui.canvases.size());
    ui.canvases.push_back(Canvas{
        .elements         = {},
        .components       = {},
        .layoutProperties = {.padding = {}, .margin = {}, .size = actualSize, .border = {}, .flex = {}},
        .boundingBox      = {position.x, position.y, actualSize.x, actualSize.y},
        .layer            = layer,
        .flags            = flags,
        .layoutDirty      = true,
        .instancesDirty   = true,
    });
    ui.cachedInstances.emplace_back(); // matching slot
    ui.anyDirty = true;
}

export inline auto DestroyCanvas(UIRenderer& ui, const CanvasHandle handle) noexcept -> void
{
    if (handle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    ui.canvases.erase(ui.canvases.begin() + handle);
    ui.cachedInstances.erase(ui.cachedInstances.begin() + handle);
    ui.anyDirty = true;
}

// ── Element creation ────────────────────────────────────────────────────────
// One function — components are attached separately after creation.

export inline auto AddElement(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle parentHandle, const LayoutProperties& layout, const float4& backgroundColor,
                              UIElementHandle& outHandle) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        outHandle = invalidElement;
        return;
    }
    Canvas& canvas{ui.canvases[canvasHandle]};

    UIElement el{
        .parent           = parentHandle,
        .layoutProperties = layout,
        .backgroundColor  = backgroundColor,
        .dirty            = DirtyFlags::all,
    };

    outHandle = detail::AppendElement(canvas, std::move(el));

    canvas.layoutDirty    = true;
    canvas.instancesDirty = true;
    ui.anyDirty           = true;
}

// ── Component attachment — each is independent and optional ─────────────────

export inline auto AttachClickComponent(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle elementHandle, std::function<void()> onPress = {},
                                        std::function<void()> onRelease = {}, std::function<void()> onHover = {}, std::function<void()> onDoubleClick = {}) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    auto& store{ui.canvases[canvasHandle].components};
    if (elementHandle >= store.Size()) [[unlikely]]
    {
        return;
    }
    store.click[elementHandle] = ClickComponent{
        std::move(onPress),
        std::move(onRelease),
        std::move(onHover),
        std::move(onDoubleClick),
    };
}

export inline auto AttachImageComponent(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle elementHandle,
                                        const fawn_vision::Texture* texture, // null = use atlas
                                        const float4& uvRect = float4{0.0F, 0.0F, 1.0F, 1.0F}) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    auto& store{ui.canvases[canvasHandle].components};
    if (elementHandle >= store.Size()) [[unlikely]]
    {
        return;
    }
    store.image[elementHandle] = ImageComponent{
        .uvRect   = uvRect,
        .texture  = texture,
        .useAtlas = texture == nullptr,
    };
    ui.canvases[canvasHandle].instancesDirty = true;
    ui.anyDirty                              = true;
}

// Text component is stubbed — sets data, renders nothing until atlas is ready.
export inline auto AttachTextComponent(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle elementHandle, std::string text, const float fontSize = 14.0F,
                                       const float4& color = {1.0F, 1.0F, 1.0F, 1.0F}) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    auto& store{ui.canvases[canvasHandle].components};
    if (elementHandle >= store.Size()) [[unlikely]]
    {
        return;
    }
    store.text[elementHandle] = TextComponent{
        .text     = std::move(text),
        .fontSize = fontSize,
        .color    = color,
    };
    // No dirty mark — text is not rendered yet
}

// ── Mutations ──────────────────────────────────────────────────────────────

export inline auto SetElementColor(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle elementHandle, const float4& color) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    Canvas& canvas{ui.canvases[canvasHandle]};
    if (elementHandle >= canvas.elements.size()) [[unlikely]]
    {
        return;
    }
    UIElement& el{canvas.elements[elementHandle]};
    if (el.backgroundColor.x == color.x && el.backgroundColor.y == color.y && el.backgroundColor.z == color.z && el.backgroundColor.w == color.w)
    {
        return; // no change
    }
    el.backgroundColor    = color;
    el.dirty              = el.dirty | DirtyFlags::style;
    canvas.instancesDirty = true;
    ui.anyDirty           = true;
}

export inline auto SetElementLayout(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle elementHandle, const LayoutProperties layout) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    Canvas& canvas{ui.canvases[canvasHandle]};
    if (elementHandle >= canvas.elements.size()) [[unlikely]]
    {
        return;
    }
    canvas.elements[elementHandle].layoutProperties = layout;
    canvas.elements[elementHandle].dirty            = canvas.elements[elementHandle].dirty | DirtyFlags::layout;
    canvas.layoutDirty                              = true;
    canvas.instancesDirty                           = true;
    ui.anyDirty                                     = true;
}

export inline auto SetElementEnabled(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle elementHandle, const bool enabled) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    Canvas& canvas{ui.canvases[canvasHandle]};
    if (elementHandle >= canvas.elements.size()) [[unlikely]]
    {
        return;
    }
    UIElement& el{canvas.elements[elementHandle]};
    if (el.enabled == enabled)
    {
        return;
    }
    el.enabled            = enabled;
    el.dirty              = el.dirty | DirtyFlags::style;
    canvas.instancesDirty = true;
    ui.anyDirty           = true;
}

// ── Interaction notifications — called from your input layer ───────────────

export inline auto NotifyHover(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle elementHandle, const bool hovered) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    Canvas& canvas{ui.canvases[canvasHandle]};
    if (elementHandle >= canvas.elements.size()) [[unlikely]]
    {
        return;
    }
    UIElement& el{canvas.elements[elementHandle]};
    if (el.hovered == hovered)
    {
        return;
    }
    el.hovered            = hovered;
    el.dirty              = el.dirty | DirtyFlags::interact;
    canvas.instancesDirty = true;
    ui.anyDirty           = true;

    if (const auto& click = canvas.components.click[elementHandle]; click && click->onHover)
    {
        click->onHover();
    }
}

export inline auto NotifyPress(UIRenderer& ui, const CanvasHandle canvasHandle, const UIElementHandle elementHandle, const bool pressed) noexcept -> void
{
    if (canvasHandle >= ui.canvases.size()) [[unlikely]]
    {
        return;
    }
    Canvas& canvas{ui.canvases[canvasHandle]};
    if (elementHandle >= canvas.elements.size()) [[unlikely]]
    {
        return;
    }
    UIElement& el{canvas.elements[elementHandle]};
    if (el.pressed == pressed)
    {
        return;
    }
    el.pressed            = pressed;
    el.dirty              = el.dirty | DirtyFlags::interact;
    canvas.instancesDirty = true;
    ui.anyDirty           = true;

    const auto& click{canvas.components.click[elementHandle]};
    if (!click)
    {
        return;
    }
    if (pressed && click->onPress)
    {
        click->onPress();
    }
    if (!pressed && click->onRelease)
    {
        click->onRelease();
    }
}

// ── Hit testing ────────────────────────────────────────────────────────────

export [[nodiscard]] inline auto HitTest(const UIRenderer& ui, const ushort2& mousePos, CanvasHandle& outCanvas, UIElementHandle& outElement) noexcept -> bool
{
    outCanvas  = invalidCanvas;
    outElement = invalidElement;

    // Walk back-to-front (highest layer index = topmost visually)
    for (std::int32_t ci{static_cast<std::int32_t>(ui.canvases.size()) - 1}; ci >= 0; --ci)
    {
        const Canvas& canvas{ui.canvases[static_cast<std::size_t>(ci)]};
        if (HasFlag(canvas.flags, canvas_flag::hidden))
        {
            continue;
        }
        if (!HasFlag(canvas.flags, canvas_flag::accepts_inputs))
        {
            continue;
        }

        for (UIElementHandle ei{}; ei < static_cast<UIElementHandle>(canvas.elements.size()); ++ei)
        {
            const UIElement& el{canvas.elements[ei]};
            if (!el.enabled)
            {
                continue;
            }

            if (const auto& [x, y, z, w]{el.computedBox};
                mousePos.x >= x && mousePos.x < static_cast<std::uint16_t>(x + z) && mousePos.y >= y && mousePos.y < static_cast<std::uint16_t>(y + w))
            {
                outCanvas  = static_cast<CanvasHandle>(ci);
                outElement = ei;
                return true;
            }
        }
    }
    return false;
}
} // namespace deer_ui
