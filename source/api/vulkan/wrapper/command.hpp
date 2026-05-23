#pragma once
#include "../deer_vulkan_core.hpp"

#include "buffer.hpp"
#include "device.hpp"
#include "dispatch.hpp"
#include "image.hpp"
#include "shader.hpp"

namespace deer_vulkan
{
struct VertexAttributes
{
    std::uint32_t location{};
    std::uint32_t binding{};
    std::uint32_t format{};
    std::uint32_t offset{};
    bool isInstance{};
};

struct CommandPool
{
    vk::CommandPool commandPool{nullptr};
};

struct CommandBuffer
{
    std::vector<vk::CommandBuffer> commandBuffer{nullptr};
    std::uint32_t count{0};
};

struct RenderParams
{
    const ImageView* colorImageView{nullptr};
    const ImageView* depthImageView{nullptr};
    std::array<float, 3> clearColor{0.0F, 0.0F, 0.0F};
    float depthClear{1.0F};
    std::int32_t xOffset{};
    std::int32_t yOffset{};
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t stencilClear{};
};

struct StageAccess
{
    vk::PipelineStageFlags stageMask;
    vk::AccessFlags accessMask;
};

[[nodiscard]] constexpr auto IsDepthLayout(const vk::ImageLayout layout) noexcept -> bool
{
    return layout == vk::ImageLayout::eDepthAttachmentOptimal || layout == vk::ImageLayout::eDepthReadOnlyOptimal || layout == vk::ImageLayout::eDepthStencilReadOnlyOptimal
        || layout == vk::ImageLayout::eDepthStencilAttachmentOptimal || layout == vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal
        || layout == vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal;
}

[[nodiscard]] constexpr auto IsStencilLayout(const vk::ImageLayout layout) noexcept -> bool
{
    return layout == vk::ImageLayout::eStencilAttachmentOptimal || layout == vk::ImageLayout::eStencilReadOnlyOptimal || layout == vk::ImageLayout::eDepthStencilReadOnlyOptimal
        || layout == vk::ImageLayout::eDepthStencilAttachmentOptimal || layout == vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal
        || layout == vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal;
}

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const std::uint32_t queueFamilyIndex, CommandPool& commandPool) noexcept -> vk_status
{
    const vk::CommandPoolCreateInfo createInfo{
        .sType            = vk::StructureType::eCommandPoolCreateInfo,
        .pNext            = nullptr,
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = queueFamilyIndex,
    };
    commandPool.commandPool = device.device.createCommandPool(createInfo, nullptr, dispatch.dispatch);

    return vk_status::ok;
}

[[nodiscard]] inline auto CreateCommandBuffer(const Dispatch& dispatch, const Device& device, const CommandPool& commandPool, const uint32_t count,
                                              CommandBuffer& commandBuffer) noexcept -> vk_status
{
    const vk::CommandBufferAllocateInfo allocInfo{
        .sType              = vk::StructureType::eCommandBufferAllocateInfo,
        .pNext              = nullptr,
        .commandPool        = commandPool.commandPool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = count,
    };

    commandBuffer.commandBuffer = device.device.allocateCommandBuffers(allocInfo, dispatch.dispatch);
    commandBuffer.count         = count;

    return vk_status::ok;
}

inline void Cleanup(const Dispatch& dispatch, const Device& device, const CommandPool& commandPool) noexcept
{
    if (commandPool.commandPool == nullptr)
    {
        return;
    }
    device.device.destroyCommandPool(commandPool.commandPool, nullptr, dispatch.dispatch);
}

inline void Cleanup(const Dispatch& dispatch, const Device& device, const CommandPool& commandPool, CommandBuffer& commandBuffer) noexcept
{
    if (commandPool.commandPool == nullptr || commandBuffer.commandBuffer.empty() || commandBuffer.count == 0)
    {
        return;
    }
    device.device.freeCommandBuffers(commandPool.commandPool, commandBuffer.commandBuffer, dispatch.dispatch);
    commandBuffer.commandBuffer.clear();
    commandBuffer.count = 0U;
}

// ---------------------------------------------------------------------------
// Recording — errors only, no per-frame spam
// ---------------------------------------------------------------------------

inline void BeginCommand(const Dispatch& dispatch, const CommandBuffer& commandBuffer) noexcept
{
    constexpr vk::CommandBufferBeginInfo beginInfo{};
    commandBuffer.commandBuffer.front().begin(beginInfo, dispatch.dispatch);
}

inline void BeginSingleCommand(const Dispatch& dispatch, const CommandBuffer& commandBuffer) noexcept
{
    constexpr vk::CommandBufferBeginInfo beginInfo{
        .sType            = vk::StructureType::eCommandBufferBeginInfo,
        .pNext            = nullptr,
        .flags            = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
        .pInheritanceInfo = nullptr,
    };
    commandBuffer.commandBuffer.front().begin(beginInfo, dispatch.dispatch);
}

inline void EndCommand(const Dispatch& dispatch, const CommandBuffer& commandBuffer) noexcept
{
    commandBuffer.commandBuffer.front().end(dispatch.dispatch);
}

// ---------------------------------------------------------------------------
// Dynamic rendering — no logging (called every frame)
// ---------------------------------------------------------------------------

inline void BeginRender(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const RenderParams& params) noexcept
{
    const vk::ClearValue colorClearValue{vk::ClearColorValue{params.clearColor[0], params.clearColor[1], params.clearColor[2], 1.0F}};
    const vk::ClearValue depthClearValue{vk::ClearDepthStencilValue{params.depthClear, params.stencilClear}};

    const vk::RenderingAttachmentInfo colorAttachment{
        .sType              = vk::StructureType::eRenderingAttachmentInfo,
        .pNext              = nullptr,
        .imageView          = params.colorImageView != nullptr ? params.colorImageView->imageView : nullptr,
        .imageLayout        = vk::ImageLayout::eAttachmentOptimalKHR,
        .resolveMode        = vk::ResolveModeFlagBits::eNone,
        .resolveImageView   = nullptr,
        .resolveImageLayout = vk::ImageLayout::eUndefined,
        .loadOp             = vk::AttachmentLoadOp::eClear,
        .storeOp            = vk::AttachmentStoreOp::eStore,
        .clearValue         = colorClearValue,
    };

    const vk::RenderingAttachmentInfo depthAttachment{
        .sType              = vk::StructureType::eRenderingAttachmentInfo,
        .pNext              = nullptr,
        .imageView          = params.depthImageView != nullptr ? params.depthImageView->imageView : nullptr,
        .imageLayout        = vk::ImageLayout::eAttachmentOptimalKHR,
        .resolveMode        = vk::ResolveModeFlagBits::eNone,
        .resolveImageView   = nullptr,
        .resolveImageLayout = vk::ImageLayout::eUndefined,
        .loadOp             = vk::AttachmentLoadOp::eClear,
        .storeOp            = vk::AttachmentStoreOp::eStore,
        .clearValue         = depthClearValue,
    };

    const vk::RenderingInfo renderingInfo{
        .sType                = vk::StructureType::eRenderingInfo,
        .pNext                = nullptr,
        .flags                = {},
        .renderArea           = vk::Rect2D{vk::Offset2D{params.xOffset, params.yOffset}, vk::Extent2D{params.width, params.height}},
        .layerCount           = 1U,
        .viewMask             = 0U,
        .colorAttachmentCount = params.colorImageView != nullptr ? 1U : 0U,
        .pColorAttachments    = params.colorImageView != nullptr ? &colorAttachment : nullptr,
        .pDepthAttachment     = params.depthImageView != nullptr ? &depthAttachment : nullptr, // todo use the format to see if depth is used
        .pStencilAttachment   = params.depthImageView != nullptr ? &depthAttachment : nullptr, // todo use the format to see if stencil is used
    };

    commandBuffer.commandBuffer.front().beginRendering(renderingInfo, dispatch.dispatch);
}

inline void EndRender(const Dispatch& dispatch, const CommandBuffer& commandBuffer) noexcept
{
    commandBuffer.commandBuffer.front().endRendering(dispatch.dispatch);
}

// ---------------------------------------------------------------------------
// Dynamic state setters — no logging (per-frame hot path)
// ---------------------------------------------------------------------------

inline void SetViewport(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const float xPos, const float yPos, const float width, const float height,
                        const float minDepth, const float maxDepth) noexcept
{
    const vk::Viewport viewport{
        .x        = xPos,
        .y        = yPos,
        .width    = width,
        .height   = height,
        .minDepth = minDepth,
        .maxDepth = maxDepth,
    };
    commandBuffer.commandBuffer.front().setViewportWithCount(viewport, dispatch.dispatch);
}

inline void SetScissor(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint32_t width, const std::uint32_t height, const std::int32_t xPos,
                       const std::int32_t yPos) noexcept
{
    const vk::Rect2D scissor{
        .offset = {.x = xPos, .y = yPos},
        .extent = {.width = width, .height = height},
    };
    commandBuffer.commandBuffer.front().setScissorWithCount(scissor, dispatch.dispatch);
}

inline void SetAlphaToCoverageEnable(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool enable) noexcept
{
    commandBuffer.commandBuffer.front().setAlphaToCoverageEnableEXT(static_cast<vk::Bool32>(enable), dispatch.dispatch);
}

inline void SetColorBlendEnable(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool enable) noexcept
{
    const vk::Bool32 isBlendEnabled{static_cast<vk::Bool32>(enable)};
    commandBuffer.commandBuffer.front().setColorBlendEnableEXT(0U, 1U, &isBlendEnabled, dispatch.dispatch);
}

inline void SetColorBlendEquation(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint32_t srcColorBlendFactor, const std::uint32_t dstColorBlendFactor,
                                  const std::uint32_t colorBlendOp, const std::uint32_t srcAlphaBlendFactor, const std::uint32_t dstAlphaBlendFactor,
                                  const std::uint32_t alphaBlendOp) noexcept
{
    const vk::ColorBlendEquationEXT blendEquation{
        .srcColorBlendFactor = static_cast<vk::BlendFactor>(srcColorBlendFactor),
        .dstColorBlendFactor = static_cast<vk::BlendFactor>(dstColorBlendFactor),
        .colorBlendOp        = static_cast<vk::BlendOp>(colorBlendOp),
        .srcAlphaBlendFactor = static_cast<vk::BlendFactor>(srcAlphaBlendFactor),
        .dstAlphaBlendFactor = static_cast<vk::BlendFactor>(dstAlphaBlendFactor),
        .alphaBlendOp        = static_cast<vk::BlendOp>(alphaBlendOp),
    };

    commandBuffer.commandBuffer.front().setColorBlendEquationEXT(0U, 1U, &blendEquation, dispatch.dispatch);
}

inline void SetColorWriteMask(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint32_t mask) noexcept
{
    const vk::ColorComponentFlags flags{static_cast<vk::ColorComponentFlagBits>(mask)};
    commandBuffer.commandBuffer.front().setColorWriteMaskEXT(0U, 1U, &flags, dispatch.dispatch);
}

inline void SetCullMode(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint8_t cullMode) noexcept
{
    const vk::CullModeFlags cullModeFlags{static_cast<vk::CullModeFlagBits>(cullMode)};
    commandBuffer.commandBuffer.front().setCullModeEXT(cullModeFlags, dispatch.dispatch);
}

inline void SetDepthBiasEnable(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool enable) noexcept
{
    commandBuffer.commandBuffer.front().setDepthBiasEnableEXT(static_cast<vk::Bool32>(enable), dispatch.dispatch);
}

inline void SetDepthCompareOperator(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint8_t compareOp) noexcept
{
    commandBuffer.commandBuffer.front().setDepthCompareOpEXT(static_cast<vk::CompareOp>(compareOp), dispatch.dispatch);
}

inline void SetDepthTestEnable(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool enable) noexcept
{
    commandBuffer.commandBuffer.front().setDepthTestEnableEXT(static_cast<vk::Bool32>(enable), dispatch.dispatch);
}

inline void SetDepthWriteEnable(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool enable) noexcept
{
    commandBuffer.commandBuffer.front().setDepthWriteEnableEXT(static_cast<vk::Bool32>(enable), dispatch.dispatch);
}

inline void SetFrontFace(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool isClockwise) noexcept
{
    commandBuffer.commandBuffer.front().setFrontFaceEXT(static_cast<vk::FrontFace>(isClockwise), dispatch.dispatch);
}

inline void SetLineWidth(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const float lineWidth) noexcept
{
    commandBuffer.commandBuffer.front().setLineWidth(lineWidth, dispatch.dispatch);
}

inline void SetPolygonMode(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint32_t mode) noexcept
{
    commandBuffer.commandBuffer.front().setPolygonModeEXT(static_cast<vk::PolygonMode>(mode), dispatch.dispatch);
}

inline void SetPrimitiveRestartEnable(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool enable) noexcept
{
    commandBuffer.commandBuffer.front().setPrimitiveRestartEnableEXT(static_cast<vk::Bool32>(enable), dispatch.dispatch);
}

inline void SetPrimitiveTopology(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint8_t topology) noexcept
{
    commandBuffer.commandBuffer.front().setPrimitiveTopologyEXT(static_cast<vk::PrimitiveTopology>(topology), dispatch.dispatch);
}

inline void SetRasterizationSamples(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint8_t sampleSize, const std::uint32_t mask) noexcept
{
    const auto samples{static_cast<vk::SampleCountFlagBits>(sampleSize)};
    const vk::SampleMask sampleMask{mask};
    commandBuffer.commandBuffer.front().setRasterizationSamplesEXT(samples, dispatch.dispatch);
    commandBuffer.commandBuffer.front().setSampleMaskEXT(samples, &sampleMask, dispatch.dispatch);
}

inline void SetRasterizerDiscardEnable(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool enable) noexcept
{
    commandBuffer.commandBuffer.front().setRasterizerDiscardEnableEXT(static_cast<vk::Bool32>(enable), dispatch.dispatch);
}

inline void SetStencilTestEnable(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const bool enable) noexcept
{
    commandBuffer.commandBuffer.front().setStencilTestEnableEXT(static_cast<vk::Bool32>(enable), dispatch.dispatch);
}

inline void SetVertexInput(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint32_t vertexSize, const std::uint32_t instanceSize,
                           const std::span<VertexAttributes> attributes) noexcept
{

    std::array<vk::VertexInputBindingDescription2EXT, 2> bindings{};
    std::uint32_t bindingCount = 1U;

    bindings[0] = vk::VertexInputBindingDescription2EXT{
        .sType     = vk::StructureType::eVertexInputBindingDescription2EXT,
        .pNext     = nullptr,
        .binding   = 0U,
        .stride    = vertexSize,
        .inputRate = vk::VertexInputRate::eVertex,
        .divisor   = 1U,
    };

    if (instanceSize != 0U)
    {
        bindings[1] = vk::VertexInputBindingDescription2EXT{
            .sType     = vk::StructureType::eVertexInputBindingDescription2EXT,
            .pNext     = nullptr,
            .binding   = 1U,
            .stride    = instanceSize,
            .inputRate = vk::VertexInputRate::eInstance,
            .divisor   = 1U,
        };
        bindingCount = 2U;
    }

    std::vector<vk::VertexInputAttributeDescription2EXT> vertexAttributes(attributes.size());
    for (std::size_t i{}; i < attributes.size(); ++i)
    {
        vertexAttributes[i] = vk::VertexInputAttributeDescription2EXT{
            .sType    = vk::StructureType::eVertexInputAttributeDescription2EXT,
            .pNext    = nullptr,
            .location = attributes[i].location,
            .binding  = attributes[i].binding,
            .format   = static_cast<vk::Format>(attributes[i].format),
            .offset   = attributes[i].offset,
        };
    }

    commandBuffer.commandBuffer.front().setVertexInputEXT(bindingCount, bindings.data(), static_cast<std::uint32_t>(vertexAttributes.size()), vertexAttributes.data(),
                                                          dispatch.dispatch);
}

inline void SetVertexInput(const Dispatch& dispatch, const CommandBuffer& commandBuffer) noexcept
{
    static constexpr vk::VertexInputBindingDescription2EXT emptyBinding{
        .sType     = vk::StructureType::eVertexInputBindingDescription2EXT,
        .pNext     = nullptr,
        .binding   = 0U,
        .stride    = 0U,
        .inputRate = vk::VertexInputRate::eVertex,
        .divisor   = 1U,
    };
    commandBuffer.commandBuffer.front().setVertexInputEXT(1U, &emptyBinding, 0U, nullptr, dispatch.dispatch);
}

// ---------------------------------------------------------------------------
// Bind — errors only
// ---------------------------------------------------------------------------

inline void BindShader(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Shader& shader) noexcept
{
    commandBuffer.commandBuffer.front().bindShadersEXT(static_cast<std::uint32_t>(shader.stages.size()), shader.stages.data(), shader.shaders.data(), dispatch.dispatch);
}

inline void BindDescriptor(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Descriptor& descriptor) noexcept
{
    commandBuffer.commandBuffer.front().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, descriptor.pipelineLayout, 0U, 1U, &descriptor.descriptorSet, 0U, nullptr,
                                                           dispatch.dispatch);
}

inline void BindIndexBuffer(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Buffer& buffer) noexcept
{
    commandBuffer.commandBuffer.front().bindIndexBuffer(buffer.buffer, 0ULL, vk::IndexType::eUint32, dispatch.dispatch);
}

inline void BindVertexBuffer(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Buffer& buffer) noexcept
{
    static constexpr vk::DeviceSize kOffset{0ULL};
    commandBuffer.commandBuffer.front().bindVertexBuffers(0U, 1U, &buffer.buffer, &kOffset, dispatch.dispatch);
}

inline void BindInstanceBuffer(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Buffer& buffer) noexcept
{
    static constexpr vk::DeviceSize kOffset{0ULL};
    commandBuffer.commandBuffer.front().bindVertexBuffers(1U, 1U, &buffer.buffer, &kOffset, dispatch.dispatch);
}

// ---------------------------------------------------------------------------
// Transfer — log one-time ops, skip per-frame CopyBuffers
// ---------------------------------------------------------------------------

inline void CopyBuffers(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Buffer& fromBuffer, const Buffer& toBuffer, const uint64_t size) noexcept
{
    const vk::BufferCopy copyRegion{
        .srcOffset = 0ULL,
        .dstOffset = 0ULL,
        .size      = size,
    };
    commandBuffer.commandBuffer.front().copyBuffer(fromBuffer.buffer, toBuffer.buffer, 1U, &copyRegion, dispatch.dispatch);
}

inline void CopyToImage(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Buffer& buffer, const Image& image, const std::uint32_t width,
                        const std::uint32_t height, const std::uint32_t depth) noexcept
{
    BeginSingleCommand(dispatch, commandBuffer);

    constexpr vk::ImageSubresourceLayers subresource{
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .mipLevel       = 0U,
        .baseArrayLayer = 0U,
        .layerCount     = 1U,
    };

    const vk::BufferImageCopy region{.bufferOffset      = vk::DeviceSize{},
                                     .bufferRowLength   = 0U,
                                     .bufferImageHeight = 0U,
                                     .imageSubresource  = subresource,
                                     .imageOffset       = vk::Offset3D{.x = 0, .y = 0, .z = 0,},
                                     .imageExtent       = vk::Extent3D{
                                         .width  = width,
                                         .height = height,
                                         .depth  = depth,
                                     },};

    commandBuffer.commandBuffer.front().copyBufferToImage(buffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, 1U, &region, dispatch.dispatch);

    EndCommand(dispatch, commandBuffer);
}

[[nodiscard]] inline auto GenerateMips(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Image& image, const std::uint32_t width, const std::uint32_t height,
                                       const std::uint32_t mipCount) noexcept -> vk_status
{
    BeginSingleCommand(dispatch, commandBuffer);

    vk::ImageMemoryBarrier barrier{
        .sType               = vk::StructureType::eImageMemoryBarrier,
        .pNext               = nullptr,
        .srcAccessMask       = {},
        .dstAccessMask       = {},
        .oldLayout           = vk::ImageLayout::eUndefined,
        .newLayout           = vk::ImageLayout::eUndefined,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image.image,
        .subresourceRange    = vk::ImageSubresourceRange{
            .aspectMask      = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel    = 0U,
            .levelCount      = 1U,
            .baseArrayLayer  = 0U,
            .layerCount      = 1U,
        },
    };

    auto mipWidth  = static_cast<std::int32_t>(width);
    auto mipHeight = static_cast<std::int32_t>(height);

    for (std::uint32_t i = 1U; i < mipCount; ++i)
    {
        barrier.subresourceRange.baseMipLevel = i - 1U;
        barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout                     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask                 = vk::AccessFlagBits::eTransferRead;

        commandBuffer.commandBuffer.front().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags{}, 0, nullptr, 0,
                                                            nullptr, 1U, &barrier, dispatch.dispatch);

        const vk::ImageBlit blit{
            .srcSubresource = vk::ImageSubresourceLayers{.aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = i - 1U, .baseArrayLayer = 0U, .layerCount = 1U,},
            .srcOffsets     = std::array{vk::Offset3D{.x=0,.y= 0,.z= 0}, vk::Offset3D{.x=mipWidth, .y=mipHeight, .z=1}},
            .dstSubresource = vk::ImageSubresourceLayers{.aspectMask=vk::ImageAspectFlagBits::eColor, .mipLevel= i, .baseArrayLayer= 0U, .layerCount= 1U,},
            .dstOffsets     = std::array{vk::Offset3D{.x=0, .y=0, .z=0}, vk::Offset3D{.x=mipWidth > 1 ? mipWidth >> 1 : 1,.y= mipHeight > 1 ? mipHeight >> 1 : 1,.z= 1}},
        };

        commandBuffer.commandBuffer.front().blitImage(image.image, vk::ImageLayout::eTransferSrcOptimal, image.image, vk::ImageLayout::eTransferDstOptimal, 1U, &blit,
                                                      vk::Filter::eLinear, dispatch.dispatch);

        barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout     = vk::ImageLayout::eTransferDstOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.commandBuffer.front().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags{}, 0U, nullptr,
                                                            0U, nullptr, 1U, &barrier, dispatch.dispatch);

        if (mipWidth > 1)
        {
            mipWidth >>= 1;
        }
        if (mipHeight > 1)
        {
            mipHeight >>= 1;
        }
    }

    barrier.subresourceRange.baseMipLevel = mipCount - 1U;
    barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout                     = vk::ImageLayout::eReadOnlyOptimal;
    barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferRead;
    barrier.dstAccessMask                 = vk::AccessFlagBits::eShaderRead;
    commandBuffer.commandBuffer.front().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlagBits{}, 0, nullptr, 0,
                                                        nullptr, 1U, &barrier, dispatch.dispatch);

    EndCommand(dispatch, commandBuffer);
    return vk_status::ok;
}

[[nodiscard]] constexpr StageAccess getStageAccess(const vk::ImageLayout layout)
{
    switch (layout)
    {
    case vk::ImageLayout::eUndefined: return {vk::PipelineStageFlagBits::eTopOfPipe, vk::AccessFlags{}};
    case vk::ImageLayout::eGeneral: return {vk::PipelineStageFlagBits::eAllCommands, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite};
    case vk::ImageLayout::eColorAttachmentOptimal:
        return {vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead};
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        return {vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead};
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
        return {vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eEarlyFragmentTests,
                vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eDepthStencilAttachmentRead};
    case vk::ImageLayout::eShaderReadOnlyOptimal: return {vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader, vk::AccessFlagBits::eShaderRead};
    case vk::ImageLayout::eTransferSrcOptimal: return {vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferRead};
    case vk::ImageLayout::eTransferDstOptimal: return {vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferWrite};
    case vk::ImageLayout::ePresentSrcKHR: return {vk::PipelineStageFlagBits::eBottomOfPipe, vk::AccessFlags{}};
    default: return {vk::PipelineStageFlagBits::eAllCommands, vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite};
    }
}

[[nodiscard]] constexpr vk::ImageAspectFlags ChooseAspectMask(const vk::ImageLayout layout, const bool hasStencil = false)
{
    switch (layout)
    {
    case vk::ImageLayout::eDepthStencilAttachmentOptimal: [[fallthrough]];
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal: [[fallthrough]];
    case vk::ImageLayout::eDepthAttachmentOptimal: [[fallthrough]];
    case vk::ImageLayout::eDepthReadOnlyOptimal: [[fallthrough]];
    case vk::ImageLayout::eStencilAttachmentOptimal: [[fallthrough]];
    case vk::ImageLayout::eStencilReadOnlyOptimal:
    {
        vk::ImageAspectFlags mask = vk::ImageAspectFlagBits::eDepth;
        if (hasStencil)
        {
            mask |= vk::ImageAspectFlagBits::eStencil;
        }
        return mask;
    }

    default: return vk::ImageAspectFlagBits::eColor;
    }
}

inline void TransitionImageLayout(const Dispatch& dispatch, const CommandBuffer& commandBuffer, Image& image, const std::uint32_t newLayout, const std::uint32_t mipOffset = 0U,
                                  const std::uint32_t mipCount = 1U, const std::uint32_t layerOffset = 0U, const std::uint32_t layerCount = 1U) noexcept
{
    const auto layout{static_cast<vk::ImageLayout>(newLayout)};
    const auto [srsStageMask, srsAccessMask]{getStageAccess(image.layout)};
    const auto [dstStageMask, dstAccessMask]{getStageAccess(layout)};
    const vk::ImageMemoryBarrier barrierPresent{.sType               = vk::StructureType::eImageMemoryBarrier2,
                                           .pNext               = nullptr,
                                           .srcAccessMask       = srsAccessMask,
                                           .dstAccessMask       = dstAccessMask,
                                           .oldLayout           = image.layout,
                                           .newLayout           = layout,
                                           .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                                           .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                                           .image               = image.image,
                                           .subresourceRange    = {
                                               .aspectMask     = ChooseAspectMask(layout),
                                               .baseMipLevel   = mipOffset,
                                               .levelCount     = mipCount,
                                               .baseArrayLayer = layerOffset,
                                               .layerCount     = layerCount,
                                           },};

    commandBuffer.commandBuffer.front().pipelineBarrier(srsStageMask, dstStageMask, {}, 0, nullptr, 0, nullptr, 1, &barrierPresent, dispatch.dispatch);
    image.layout = static_cast<vk::ImageLayout>(newLayout);
}

// ---------------------------------------------------------------------------
// Draw — no logging (per-frame hot path)
// ---------------------------------------------------------------------------

inline void DrawFullscreen(const Dispatch& dispatch, const CommandBuffer& commandBuffer) noexcept
{
    commandBuffer.commandBuffer.front().draw(3U, 1U, 0U, 0U, dispatch.dispatch);
}

inline void DrawIndirect(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const Buffer& buffer, const std::uint32_t offset, const std::uint32_t count,
                         const std::uint32_t stride) noexcept
{
    commandBuffer.commandBuffer.front().drawIndirect(buffer.buffer, offset, count, stride, dispatch.dispatch);
}

inline void DrawIndexed(const Dispatch& dispatch, const CommandBuffer& commandBuffer, const std::uint32_t indexCount, const std::uint32_t instanceCount,
                        const std::uint32_t firstIndex, const std::uint32_t firstInstance) noexcept
{
    commandBuffer.commandBuffer.front().drawIndexed(indexCount, instanceCount, firstIndex, 0, firstInstance, dispatch.dispatch);
}
} // namespace deer_vulkan
