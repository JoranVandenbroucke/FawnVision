#pragma once
#include "../deer_vulkan_core.hpp"

#include "buffer.hpp"
#include "device.hpp"
#include "dispatch.hpp"
#include "image_view.hpp"
#include "sampler.hpp"

namespace deer_vulkan
{
struct DescriptorImageInfo
{
    Sampler sampler{};
    ImageView view{};
    Image image{};
};

struct DescriptorLayout
{
    std::uint32_t binding{};
    std::uint32_t descriptorType{};
    std::uint32_t descriptorCount{};
    std::uint32_t stageFlags{};
};

struct Descriptor
{
    vk::DescriptorSetLayout descriptorSetLayout{nullptr};
    vk::PipelineLayout pipelineLayout{nullptr};
    vk::DescriptorSet descriptorSet{nullptr};
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    std::vector<vk::DescriptorImageInfo> imageInfos;
    std::vector<vk::DescriptorBufferInfo> bufferInfos;
};

[[nodiscard]] inline auto Initialize(const Dispatch& dispatch, const Device& device, const std::span<DescriptorLayout> descriptorLayouts, Descriptor& descriptor) noexcept
    -> vk_status
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings(descriptorLayouts.size());
    for (std::size_t i = 0; i < descriptorLayouts.size(); ++i)
    {
        bindings[i] = vk::DescriptorSetLayoutBinding{
            .binding            = descriptorLayouts[i].binding,
            .descriptorType     = static_cast<vk::DescriptorType>(descriptorLayouts[i].descriptorType),
            .descriptorCount    = descriptorLayouts[i].descriptorCount,
            .stageFlags         = static_cast<vk::ShaderStageFlags>(descriptorLayouts[i].stageFlags),
            .pImmutableSamplers = nullptr,
        };
    }

    const vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{
        .sType        = vk::StructureType::eDescriptorSetLayoutBindingFlagsCreateInfo,
        .pNext        = nullptr,
        .flags        = {},
        .bindingCount = static_cast<std::uint32_t>(bindings.size()),
        .pBindings    = bindings.data(),
    };
    descriptor.descriptorSetLayout = device.device.createDescriptorSetLayout(layoutCreateInfo, nullptr, dispatch.dispatch);

    const vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{
        .sType                  = vk::StructureType::ePipelineLayoutCreateInfo,
        .pNext                  = nullptr,
        .flags                  = {},
        .setLayoutCount         = 1U,
        .pSetLayouts            = &descriptor.descriptorSetLayout,
        .pushConstantRangeCount = 0U,
        .pPushConstantRanges    = nullptr,
    };
    descriptor.pipelineLayout = device.device.createPipelineLayout(pipelineLayoutCreateInfo, nullptr, dispatch.dispatch);

    const vk::DescriptorSetAllocateInfo allocInfo{
        .sType              = vk::StructureType::eDescriptorSetAllocateInfo,
        .pNext              = nullptr,
        .descriptorPool     = device.descriptorPool,
        .descriptorSetCount = 1U,
        .pSetLayouts        = &descriptor.descriptorSetLayout,
    };
    descriptor.descriptorSet = device.device.allocateDescriptorSets(allocInfo, dispatch.dispatch).front();

    const std::size_t layoutCount = descriptorLayouts.size();
    descriptor.writeDescriptorSets.reserve(layoutCount);
    descriptor.imageInfos.reserve(layoutCount);
    descriptor.bufferInfos.reserve(layoutCount);

    return vk_status::ok;
}

inline void Cleanup(const Dispatch& dispatch, const Device& device, Descriptor& descriptor) noexcept
{

    if (const vk_status status{FromVkResult(device.device.freeDescriptorSets(device.descriptorPool, 1U, &descriptor.descriptorSet, dispatch.dispatch))}; IsError(status))
    {
        return;
    }

    device.device.destroyPipelineLayout(descriptor.pipelineLayout, nullptr, dispatch.dispatch);
    device.device.destroyDescriptorSetLayout(descriptor.descriptorSetLayout, nullptr, dispatch.dispatch);

    descriptor.writeDescriptorSets.clear();
    descriptor.imageInfos.clear();
    descriptor.bufferInfos.clear();
}

inline void BindImage(Descriptor& descriptor, const Sampler& sampler, const ImageView& imageView, const Image& image, const std::uint32_t binding) noexcept
{
    descriptor.imageInfos.push_back(vk::DescriptorImageInfo{.sampler = sampler.sampler, .imageView = imageView.imageView, .imageLayout = image.layout});
    descriptor.writeDescriptorSets.push_back(vk::WriteDescriptorSet{
        .pNext            = nullptr,
        .dstSet           = descriptor.descriptorSet,
        .dstBinding       = binding,
        .dstArrayElement  = 0U,
        .descriptorCount  = 1U,
        .descriptorType   = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo       = &descriptor.imageInfos.back(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    });
}

inline void BindBuffer(Descriptor& descriptor, const Buffer& buffer, const uint64_t offset, const uint64_t range, const std::uint32_t binding,
                       const vk::DescriptorType type = vk::DescriptorType::eUniformBuffer) noexcept
{
    descriptor.bufferInfos.push_back(vk::DescriptorBufferInfo{.buffer = buffer.buffer, .offset = offset, .range = range});
    descriptor.writeDescriptorSets.push_back(vk::WriteDescriptorSet{
        .pNext            = nullptr,
        .dstSet           = descriptor.descriptorSet,
        .dstBinding       = binding,
        .dstArrayElement  = 0U,
        .descriptorCount  = 1U,
        .descriptorType   = type,
        .pImageInfo       = nullptr,
        .pBufferInfo      = &descriptor.bufferInfos.back(),
        .pTexelBufferView = nullptr,
    });
}

inline void UpdateDescriptor(const Dispatch& dispatch, const Device& device, const Descriptor& descriptor) noexcept
{
    device.device.updateDescriptorSets(static_cast<std::uint32_t>(descriptor.writeDescriptorSets.size()), descriptor.writeDescriptorSets.data(), 0U, nullptr, dispatch.dispatch);
}

} // namespace deer_vulkan
