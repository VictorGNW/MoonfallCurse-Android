#if defined(__ANDROID__) && defined(ENABLE_OOT3D_VULKAN)
#include "gfx_vulkan_pica_internal.h"
#include "fast/renderer/spirv_pack.h"
#include "fast/renderer3ds/vulkan_image_contract.h"

#include <cstring>

namespace Fast {
namespace {

TRIAEVUM_SPIRV_PACK_EMBED_BEGIN
alignas(uint32_t) const unsigned char kAstcEncoder[] = {
#if TRIAEVUM_SPIRV_PACK_EMBED
#embed "../extension/third_party/bcn_layer/astc_encoder.spv"
#else
#include "astc_encoder.spv.inc"
#endif
};
TRIAEVUM_SPIRV_PACK_EMBED_END

constexpr uint32_t kMaxSetsPerFrame = 128;
constexpr uint32_t kMaxMipLevels = 13; // DDS loader admits textures up to 4096x4096.

struct MipUpload {
    std::array<int32_t, 2> Extent{};
    uint32_t BlocksX = 0;
    uint32_t BlocksY = 0;
    VkDescriptorBufferInfo Input{};
    VkDescriptorBufferInfo Output{};
};

} // namespace

bool GfxRenderingAPIVulkan::CreateBcnAstcPipeline() {
    if (mBcnAstcPipelineAttempted)
        return mBcnAstcPipeline != VK_NULL_HANDLE;
    mBcnAstcPipelineAttempted = true;

    VkShaderModule module = VK_NULL_HANDLE;
    try {
        const std::array bindings{
            VkDescriptorSetLayoutBinding{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT},
            VkDescriptorSetLayoutBinding{1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT},
        };
        VkDescriptorSetLayoutCreateInfo setInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        setInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        setInfo.pBindings = bindings.data();
        CheckVk(vkCreateDescriptorSetLayout(mDevice, &setInfo, nullptr, &mBcnAstcSetLayout),
                "vkCreateDescriptorSetLayout(BCn ASTC)");
        const VkPushConstantRange push{VK_SHADER_STAGE_COMPUTE_BIT, 0, 2 * sizeof(int32_t)};
        VkPipelineLayoutCreateInfo layout{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        layout.setLayoutCount = 1;
        layout.pSetLayouts = &mBcnAstcSetLayout;
        layout.pushConstantRangeCount = 1;
        layout.pPushConstantRanges = &push;
        CheckVk(vkCreatePipelineLayout(mDevice, &layout, nullptr, &mBcnAstcPipelineLayout),
                "vkCreatePipelineLayout(BCn ASTC)");

        std::array<uint32_t, sizeof(kAstcEncoder) / sizeof(uint32_t)> code;
        std::memcpy(code.data(), kAstcEncoder, sizeof(kAstcEncoder));
        VkShaderModuleCreateInfo shader{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        shader.codeSize = sizeof(kAstcEncoder);
        shader.pCode = code.data();
        CheckVk(vkCreateShaderModule(mDevice, &shader, nullptr, &module), "vkCreateShaderModule(BCn ASTC)");
        VkComputePipelineCreateInfo pipeline{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        pipeline.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipeline.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        pipeline.stage.module = module;
        pipeline.stage.pName = "main";
        pipeline.layout = mBcnAstcPipelineLayout;
        CheckVk(vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &pipeline, nullptr, &mBcnAstcPipeline),
                "vkCreateComputePipelines(BCn ASTC)");
    } catch (const std::exception& error) {
        DestroyBcnAstcPipeline();
        std::fprintf(stderr, "TRIAEVUM_BCN_ASTC fallback=pipeline reason=%s\n", error.what());
    }
    if (module != VK_NULL_HANDLE)
        vkDestroyShaderModule(mDevice, module, nullptr);
    if (mBcnAstcPipeline != VK_NULL_HANDLE)
        std::fprintf(stderr, "TRIAEVUM_BCN_ASTC mode=active\n");
    return mBcnAstcPipeline != VK_NULL_HANDLE;
}

void GfxRenderingAPIVulkan::DestroyBcnAstcPipeline() {
    if (mBcnAstcPipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(mDevice, mBcnAstcPipeline, nullptr);
    if (mBcnAstcPipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(mDevice, mBcnAstcPipelineLayout, nullptr);
    if (mBcnAstcSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(mDevice, mBcnAstcSetLayout, nullptr);
    mBcnAstcPipeline = VK_NULL_HANDLE;
    mBcnAstcPipelineLayout = VK_NULL_HANDLE;
    mBcnAstcSetLayout = VK_NULL_HANDLE;
}

void GfxRenderingAPIVulkan::ResetBcnAstcFrame(FrameResources& frame, bool shutdown) {
    for (auto& buffer : frame.RetiredAstcBuffers)
        DestroyBuffer(buffer);
    frame.RetiredAstcBuffers.clear();
    if (frame.AstcDescriptorPool == VK_NULL_HANDLE)
        return;
    if (shutdown) {
        vkDestroyDescriptorPool(mDevice, frame.AstcDescriptorPool, nullptr);
        frame.AstcDescriptorPool = VK_NULL_HANDLE;
    } else {
        CheckVk(vkResetDescriptorPool(mDevice, frame.AstcDescriptorPool, 0),
                "vkResetDescriptorPool(BCn ASTC)");
    }
}

bool GfxRenderingAPIVulkan::TryUploadBcnAsAstc(
    TextureRecord& texture, std::span<const uint8_t> pixels,
    std::span<const uint8_t> lowerLevels, uint32_t width, uint32_t height,
    uint32_t mipLevels, VkFormat rgbaFormat) {
    if (!mBcnAstcEnabled || !mFrameActive || !mipLevels || mipLevels > kMaxMipLevels)
        return false;
    const bool srgb = rgbaFormat == VK_FORMAT_R8G8B8A8_SRGB;
    if (srgb && !mBcnAstcSrgbSupported)
        return false;
    const VkFormat format = srgb ? VK_FORMAT_ASTC_4x4_SRGB_BLOCK : VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
    constexpr VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    if (!Renderer3ds::VulkanImage2DSupported(mPhysicalDevice, format, usage, width, height, mipLevels, 1, VK_SAMPLE_COUNT_1_BIT))
        return false;

    const auto align = [&](VkDeviceSize bytes) {
        return (bytes + mBcnAstcBufferAlignment - 1) & ~(mBcnAstcBufferAlignment - 1);
    };
    std::array<MipUpload, kMaxMipLevels> mips{};
    VkDeviceSize inputSize = 0;
    VkDeviceSize outputSize = 0;
    size_t lowerSize = 0;
    for (uint32_t level = 0; level < mipLevels; ++level) {
        const auto w = std::max(1U, width >> level);
        const auto h = std::max(1U, height >> level);
        auto& mip = mips[level];
        mip.Extent = {static_cast<int32_t>(w), static_cast<int32_t>(h)};
        mip.BlocksX = (w + 3) / 4;
        mip.BlocksY = (h + 3) / 4;
        mip.Input.offset = align(inputSize);
        mip.Input.range = VkDeviceSize(w) * h * 4;
        mip.Output.offset = align(outputSize);
        mip.Output.range = VkDeviceSize(mip.BlocksX) * mip.BlocksY * 16;
        inputSize = mip.Input.offset + mip.Input.range;
        outputSize = mip.Output.offset + mip.Output.range;
        if (mip.Input.range > mBcnAstcMaxBufferRange || mip.Output.range > mBcnAstcMaxBufferRange)
            return false;
        if (level != 0)
            lowerSize += mip.Input.range;
    }
    if (pixels.size() != mips[0].Input.range || lowerLevels.size() != lowerSize ||
        !CreateBcnAstcPipeline())
        return false;

    auto& frame = mFrameResources[mCurrentFrame];
    std::array<VkDescriptorSet, kMaxMipLevels> sets{};
    BufferAllocation input;
    BufferAllocation output;
    try {
        if (frame.AstcDescriptorPool == VK_NULL_HANDLE) {
            const VkDescriptorPoolSize size{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, kMaxSetsPerFrame * 2};
            VkDescriptorPoolCreateInfo pool{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
            pool.maxSets = kMaxSetsPerFrame;
            pool.poolSizeCount = 1;
            pool.pPoolSizes = &size;
            CheckVk(vkCreateDescriptorPool(mDevice, &pool, nullptr, &frame.AstcDescriptorPool),
                    "vkCreateDescriptorPool(BCn ASTC)");
        }
        std::array<VkDescriptorSetLayout, kMaxMipLevels> layouts;
        layouts.fill(mBcnAstcSetLayout);
        VkDescriptorSetAllocateInfo allocation{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocation.descriptorPool = frame.AstcDescriptorPool;
        allocation.descriptorSetCount = mipLevels;
        allocation.pSetLayouts = layouts.data();
        const VkResult allocated = vkAllocateDescriptorSets(mDevice, &allocation, sets.data());
        if (allocated == VK_ERROR_OUT_OF_POOL_MEMORY || allocated == VK_ERROR_FRAGMENTED_POOL)
            return false;
        CheckVk(allocated, "vkAllocateDescriptorSets(BCn ASTC)");

        frame.RetiredAstcBuffers.reserve(frame.RetiredAstcBuffers.size() + 2);
        input = CreateBuffer(inputSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
        output = CreateBuffer(outputSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);
        CreateNativePicaTextureStorage(texture, width, height, mipLevels, format, false);
        texture.UploadedMipLevels = mipLevels;
        RecreateSampler(texture);
    } catch (const std::exception& error) {
        DestroyBuffer(input);
        DestroyBuffer(output);
        DestroyTexture(texture);
        if (!mBcnAstcUploadFailureLogged) {
            mBcnAstcUploadFailureLogged = true;
            std::fprintf(stderr, "TRIAEVUM_BCN_ASTC fallback=upload reason=%s\n", error.what());
        }
        return false;
    }
    // The fence also covers frames that submit several command buffer prefixes.
    frame.RetiredAstcBuffers.push_back(input);
    frame.RetiredAstcBuffers.push_back(output);
    size_t lowerOffset = 0;
    for (uint32_t level = 0; level < mipLevels; ++level) {
        auto& mip = mips[level];
        const auto source = level == 0 ? pixels : lowerLevels.subspan(lowerOffset, mip.Input.range);
        if (level != 0)
            lowerOffset += source.size();
        std::memcpy(static_cast<uint8_t*>(input.Mapped) + mip.Input.offset, source.data(), source.size());
        mip.Input.buffer = input.Buffer;
        mip.Output.buffer = output.Buffer;
        std::array<VkWriteDescriptorSet, 2> writes{};
        const VkDescriptorBufferInfo* infos[] = {&mip.Input, &mip.Output};
        for (uint32_t binding = 0; binding < writes.size(); ++binding) {
            writes[binding] = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            writes[binding].dstSet = sets[level];
            writes[binding].dstBinding = binding;
            writes[binding].descriptorCount = 1;
            writes[binding].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writes[binding].pBufferInfo = infos[binding];
        }
        vkUpdateDescriptorSets(mDevice, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }

    EndNativePicaRenderPass();
    const VkCommandBuffer command = mCommandBuffers[mCurrentFrame];
    if (mRenderPassActive) {
        vkCmdEndRenderPass(command);
        mRenderPassActive = false;
        mOverlayRenderPassActive = false;
    }
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_COMPUTE, mBcnAstcPipeline);
    std::array<VkBufferImageCopy, kMaxMipLevels> copies{};
    for (uint32_t level = 0; level < mipLevels; ++level) {
        const auto& mip = mips[level];
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_COMPUTE, mBcnAstcPipelineLayout,
                                0, 1, &sets[level], 0, nullptr);
        vkCmdPushConstants(command, mBcnAstcPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0,
                           sizeof(mip.Extent), mip.Extent.data());
        vkCmdDispatch(command, (mip.BlocksX + 7) / 8, (mip.BlocksY + 7) / 8, 1);
        copies[level].bufferOffset = mip.Output.offset;
        copies[level].imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, level, 0, 1};
        copies[level].imageExtent = {static_cast<uint32_t>(mip.Extent[0]),
                                     static_cast<uint32_t>(mip.Extent[1]), 1};
    }

    VkBufferMemoryBarrier blocks{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
    blocks.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    blocks.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    blocks.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    blocks.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    blocks.buffer = output.Buffer;
    blocks.size = VK_WHOLE_SIZE;
    VkImageMemoryBarrier image{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    image.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    image.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image.image = texture.Image;
    image.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, mipLevels, 0, 1};
    vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0, 0, nullptr, 1, &blocks, 1, &image);
    vkCmdCopyBufferToImage(command, output.Buffer, texture.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           mipLevels, copies.data());
    image.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    image.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    image.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image);
    texture.Uploaded = true;
    return true;
}

} // namespace Fast
#endif
