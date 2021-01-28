#include "CommandList.hpp"
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/Pipeline/pipeline.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetAlloc.hpp>
#include <Renderer/Backend/Images/Image.hpp>
#include <Renderer/Backend/Images/Sampler.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Buffers/RingBuffer.hpp>
#include <Renderer/Backend/ShaderProgram/ShaderProgram.hpp>
#include <Renderer/Backend/Pipeline/GraphicsState/GraphicsState.hpp>
#include <Renderer/Core/Alignement/Alignement.hpp>

#include <Renderer/Backend/RayTracing/TLAS/TLAS.hpp>

TRE_NS_START

void Renderer::CommandBufferDeleter::operator()(CommandBuffer* cmd)
{
    cmd->renderBackend->GetObjectsPool().commandBuffers.Free(cmd);
}

Renderer::CommandBuffer::CommandBuffer(RenderBackend* backend, VkCommandBuffer buffer, Type type) :
    renderBackend(backend), commandBuffer(buffer), type(type), allocatedSets{}, dirty{},
    program(NULL), state(NULL), pipeline(NULL), stateUpdate(false), renderToSwapchain(false)
{
}

void Renderer::CommandBuffer::Begin()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        ASSERTF(true, "Failed to begin recording command buffer!");
    }
}

void Renderer::CommandBuffer::End()
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
       ASSERTF(true, "failed to record command buffer!");
    }
}

void Renderer::CommandBuffer::Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ)
{
    this->BindPipeline();
    vkCmdDispatch(commandBuffer, groupX, groupY, groupZ);
}

void Renderer::CommandBuffer::SetViewport(const VkViewport& viewport)
{
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void Renderer::CommandBuffer::SetScissor(const VkRect2D& scissor)
{
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::CommandBuffer::BeginRenderPass(VkClearColorValue clearColor)
{
    VkClearValue clearValue[2];
    clearValue[0].color = clearColor;
    clearValue[1].depthStencil = { 1.f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = renderBackend->GetRenderContext().GetSwapchain().GetRenderPass();
    renderPassInfo.framebuffer       = renderBackend->GetRenderContext().GetSwapchain().GetCurrentFramebuffer();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = renderBackend->GetRenderContext().GetSwapchain().GetExtent();
    renderPassInfo.clearValueCount   = 2;
    renderPassInfo.pClearValues      = clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    subpassIndex = 0;
}

void Renderer::CommandBuffer::BeginRenderPass(const RenderPassInfo& info, VkSubpassContents contents)
{
    ASSERT(type == Type::ASYNC_COMPUTE || type == Type::ASYNC_TRANSFER);

    renderPass = &renderBackend->RequestRenderPass(info);
    framebuffer = &renderBackend->RequestFramebuffer(info, renderPass);
    this->InitViewportScissor(info, framebuffer);

    VkClearValue clearValues[MAX_ATTACHMENTS + 1];
    uint32 clearValuesCount = 0;

    memset(framebufferAttachments, 0, sizeof(framebufferAttachments));

    for (uint32 i = 0; i < info.colorAttachmentCount; i++) {
        ASSERT(!info.colorAttachments[i]);

        framebufferAttachments[i] = info.colorAttachments[i];

        if (info.clearAttachments & (1u << i)) {
            clearValues[i].color = info.clearColor[i];
            clearValuesCount = i + 1;
        }

        if (info.colorAttachments[i]->GetImage()->IsSwapchainImage()) {
            renderToSwapchain = true;
        }
    }

    if (info.depthStencil && (info.opFlags & RENDER_PASS_OP_CLEAR_DEPTH_STENCIL_BIT) != 0) {
        framebufferAttachments[info.colorAttachmentCount] = info.depthStencil;
        clearValues[info.colorAttachmentCount].depthStencil = info.clearDepthStencil;
        clearValuesCount = info.colorAttachmentCount + 1;
    }

    VkRenderPassBeginInfo beginInfo;
    beginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext             = NULL;
    beginInfo.renderPass        = renderPass->GetApiObject();
    beginInfo.framebuffer       = framebuffer->GetApiObject();
    beginInfo.renderArea        = scissor;
    beginInfo.clearValueCount   = clearValuesCount;
    beginInfo.pClearValues      = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, contents);

    if (program && state && !stateUpdate) {
        this->BindPipeline();
    }

    // this->SetViewport(viewport);
    // this->SetScissor(scissor);
    subpassIndex = 0;
}

void Renderer::CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::CommandBuffer::NextRenderPass(VkSubpassContents contents)
{
    ASSERT(renderPass == NULL);

    vkCmdNextSubpass(commandBuffer, contents);
    subpassIndex++;
}

void Renderer::CommandBuffer::BindPipeline(const Pipeline& pipeline)
{
    // ASSERT(type != Type::GENERIC);
    this->pipeline = &pipeline;
    vkCmdBindPipeline(commandBuffer, (VkPipelineBindPoint)pipeline.GetPipelineType(), pipeline.GetApiObject());
}

void Renderer::CommandBuffer::BindVertexBuffer(const Buffer& buffer, DeviceSize offset)
{
    VkBuffer vertexBuffers[] = { buffer.GetApiObject() };
    VkDeviceSize offsets[]   = { offset };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void Renderer::CommandBuffer::BindIndexBuffer(const Buffer& buffer, DeviceSize offset)
{
    vkCmdBindIndexBuffer(commandBuffer, buffer.GetApiObject(), offset, VK_INDEX_TYPE_UINT16);
}

void Renderer::CommandBuffer::DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance)
{
    this->FlushDescriptorSets();
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void Renderer::CommandBuffer::Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance)
{
    this->FlushDescriptorSets();
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void Renderer::CommandBuffer::BindDescriptorSet(const Pipeline& pipeline, const std::initializer_list<VkDescriptorSet>& descriptors,
    const std::initializer_list<uint32>& dyncOffsets)
{
    vkCmdBindDescriptorSets(commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline.GetPipelineLayout().GetApiObject(),
        0, (uint32)descriptors.size(), descriptors.begin(), (uint32)dyncOffsets.size(), dyncOffsets.begin());
}

void Renderer::CommandBuffer::SetGraphicsState(GraphicsState& state)
{
    if (type != Type::GENERIC)
        return;

    if (this->state && this->state->GetHash() == state.GetHash())
        return;

    pipeline = NULL;
    this->state = &state;
}

void Renderer::CommandBuffer::BindShaderProgram(const ShaderProgram& program)
{
    if (this->program && this->program->GetHash() == program.GetHash())
        return;

    pipeline = NULL;
    this->program = &program;
}

void Renderer::CommandBuffer::SetUniformBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset, DeviceSize range)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    auto& b = bindings.bindings[set][binding];

    // TODO: for the cache I dont think we need to place the VK object but instead a hash of the vk object and its state (wether its written or not)
    /*if (bindings.cache[set][binding] == (uint64)buffer.GetApiObject() && b.resource.buffer.offset == offset && b.resource.buffer.range == range) {
        return;
    }*/

    b.resource.buffer = VkDescriptorBufferInfo{ buffer.GetApiObject(), offset, range };
    b.dynamicOffset   = 0;

    bindings.cache[set][binding] = (uint64)buffer.GetApiObject();
    dirty.sets |= (1u << set);
}

void Renderer::CommandBuffer::SetUniformBuffer(uint32 set, uint32 binding, const RingBuffer& buffer, DeviceSize offset, DeviceSize range)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    if (range == VK_WHOLE_SIZE) {
        range = buffer.GetUnitSize();
    }

    auto& b = bindings.bindings[set][binding];

    // Think about what if everything is the same just the offset changed! we shouldnt then update dirty set instead just update the set
    // that have the dynamic offset set in it
    /*if (bindings.cache[set][binding] == (uint64)buffer.GetApiObject() && b.resource.buffer.offset == offset && b.resource.buffer.range == range) {
        b.dynamicOffset = buffer.GetCurrentOffset();
        dirty.dynamicSets |= 1u << set;
    }else{*/
        // update everything
        b.resource.buffer = VkDescriptorBufferInfo{ buffer.GetApiObject(), offset, range };
        b.dynamicOffset = buffer.GetCurrentOffset();

        bindings.cache[set][binding] = (uint64)buffer.GetApiObject();
        dirty.sets |= (1u << set);
    //}
}

void Renderer::CommandBuffer::SetStorageBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset, DeviceSize range)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);
    auto& b = bindings.bindings[set][binding];

    if (bindings.cache[set][binding] == (uint64)buffer.GetApiObject() && b.resource.buffer.offset == offset && b.resource.buffer.range == range) {
        return;
    }

    b.resource.buffer = VkDescriptorBufferInfo{ buffer.GetApiObject(), offset, range };
    b.dynamicOffset = 0;

    bindings.cache[set][binding] = (uint64)buffer.GetApiObject();
    dirty.sets |= (1u << set);
}

void Renderer::CommandBuffer::SetTexture(uint32 set, uint32 binding, const ImageView& texture)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    auto& b = bindings.bindings[set][binding];
    VkImageLayout layout = texture.GetInfo().image->GetInfo().layout;

    if (bindings.cache[set][binding] == (uint64)texture.GetApiObject() && b.resource.image.imageLayout == layout) {
        return;
    }

    b.resource.image.imageView = texture.GetApiObject();
    b.resource.image.imageLayout = layout;

    bindings.cache[set][binding] = (uint64)texture.GetApiObject();
    dirty.sets |= (1u << set);
}

void Renderer::CommandBuffer::SetTexture(uint32 set, uint32 binding, const ImageView& texture, const Sampler& sampler)
{
    this->SetSampler(set, binding, sampler);
    this->SetTexture(set, binding, texture);
}

void Renderer::CommandBuffer::SetSampler(uint32 set, uint32 binding, const Sampler& sampler)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    auto& b = bindings.bindings[set][binding];

    if (bindings.cache[set][binding] == (uint64)sampler.GetApiObject()) {
        return;
    }

    b.resource.image.sampler = sampler.GetApiObject();

    bindings.cache[set][binding] = (uint64)sampler.GetApiObject();
    dirty.sets |= (1u << set);
}

void Renderer::CommandBuffer::SetInputAttachments(uint32 set, uint32 startBinding)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(startBinding + renderPass->GetInputAttachmentsCount(subpassIndex) >= MAX_DESCRIPTOR_BINDINGS);

    uint32 inputAttachmentCount = renderPass->GetInputAttachmentsCount(subpassIndex);

    for (uint32 i = 0; i < inputAttachmentCount; i++) {
        auto& ref = renderPass->GetInputAttachment(subpassIndex, i);

        if (ref.attachment == VK_ATTACHMENT_UNUSED)
            continue;
        
        const ImageView* view = framebufferAttachments[ref.attachment];
        ASSERT(!view);
        ASSERT(!(view->GetImage()->GetInfo().usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT));

        auto& b = bindings.bindings[set][startBinding + i];
        b.resource.image.imageLayout = ref.layout;
        b.resource.image.imageView = view->GetApiObject();

        dirty.sets |= 1u << set;
    }
}

void Renderer::CommandBuffer::PushConstants(ShaderStagesFlags stages, const void* data, VkDeviceSize size, VkDeviceSize offset)
{
    const auto& pipelineLayout = pipeline->GetPipelineLayout();
    const auto& pushConstant = pipelineLayout.GetPushConstantRangeFromStage(stages);

    vkCmdPushConstants(commandBuffer, pipelineLayout.GetApiObject(), stages, pushConstant.offset + (uint32)offset, (uint32)size, data);
}

void Renderer::CommandBuffer::SetAccelerationStrucure(uint32 set, uint32 binding, const Tlas& tlas)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);
    auto& b = bindings.bindings[set][binding];

    // TODO: maybe some checks here in the future!
    //if (bindings.cache[set][binding] == (uint64)buffer.GetApiObject() && b.resource.buffer.offset == offset && b.resource.buffer.range == range) {
    //    return;
    //}

    b.resource.accl.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    b.resource.accl.pNext = NULL;
    b.resource.accl.accelerationStructureCount = 1;
    b.resource.accl.pAccelerationStructures = &tlas.GetApiObject();

    bindings.cache[set][binding] = (uint64)tlas.GetApiObject();
    dirty.sets |= (1u << set);
}

void Renderer::CommandBuffer::SetAccelerationStrucure(uint32 set, uint32 binding, VkAccelerationStructureKHR* tlas)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);
    auto& b = bindings.bindings[set][binding];

    // TODO: maybe some checks here in the future!
    //if (bindings.cache[set][binding] == (uint64)buffer.GetApiObject() && b.resource.buffer.offset == offset && b.resource.buffer.range == range) {
    //    return;
    //}

    b.resource.accl.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    b.resource.accl.pNext = NULL;
    b.resource.accl.accelerationStructureCount = 1;
    b.resource.accl.pAccelerationStructures = tlas;

    bindings.cache[set][binding] = (uint64)(*tlas);
    dirty.sets |= (1u << set);
}

void Renderer::CommandBuffer::TraceRays(uint32 width, uint32 height, uint32 depth)
{
    // TODO: out image!
    ///if (true) {
    //    this->SetTexture();
    //}

    this->FlushDescriptorSets();

    const RenderDevice& device = renderBackend->GetRenderDevice();
    const uint32_t groupHandleSize = device.GetRtProperties().shaderGroupHandleSize;  // Size of a program identifier
    const uint32_t baseAlignment = device.GetRtProperties().shaderGroupBaseAlignment;  // Size of shader alignment
    const auto& sbt = pipeline->GetSBT();

    // Size of a program identifier
    const uint32_t groupSize = Utils::AlignUp(groupHandleSize, baseAlignment);
    const uint32_t groupStride = groupSize;

    using Stride = VkStridedDeviceAddressRegionKHR;

    /*std::array<Stride, 4> strideAddresses{
        Stride{sbtAddress + 0u * groupSize, groupStride, groupSize * 1},  // raygen
        Stride{sbtAddress + 1u * groupSize, groupStride, groupSize * 1},  // miss
        Stride{sbtAddress + 2u * groupSize, groupStride, groupSize * 1},  // hit
        Stride{0u, 0u, 0u} 
    };*/

    std::array<Stride, 4> strideAddresses{
        Stride{sbt.GetSbtAddress(0), groupSize, groupSize},  // raygen
        Stride{sbt.GetSbtAddress(1), groupSize, groupSize},  // miss
        Stride{sbt.GetSbtAddress(2), groupSize, groupSize},  // hit
        Stride{0u, 0u, 0u}
    };

    vkCmdTraceRaysKHR(commandBuffer, &strideAddresses[0], &strideAddresses[1], &strideAddresses[2], &strideAddresses[3], width, height, depth);
}

void Renderer::CommandBuffer::TraceRays(VkPipeline pipeline, const SBT& sbt, uint32 width, uint32 height, uint32 depth)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline);
    this->FlushDescriptorSets();

    auto& raygen = sbt.GetSbtEntry(0);
    auto& raymiss = sbt.GetSbtEntry(1);
    auto& rayhist = sbt.GetSbtEntry(2);
    auto& raycall = sbt.GetSbtEntry(3);
    vkCmdTraceRaysKHR(commandBuffer, &raygen, &raymiss, &rayhist, &raycall, width, height, depth);
}


void Renderer::CommandBuffer::PrepareGenerateMipmapBarrier(const Image& image, VkImageLayout baseLevelLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, bool needTopLevelBarrier)
{
    auto& info = image.GetInfo();
    VkImageMemoryBarrier barriers[2] = {};
    ASSERT(info.levels <= 1);

    for (uint32 i = 0; i < 2; i++) {
        barriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barriers[i].image = image.GetApiObject();
        barriers[i].subresourceRange.aspectMask = FormatToAspectMask(info.format);
        barriers[i].subresourceRange.layerCount = info.layers;
        barriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        if (i == 0) {
            barriers[i].oldLayout = baseLevelLayout;
            barriers[i].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barriers[i].srcAccessMask = srcAccess;
            barriers[i].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barriers[i].subresourceRange.baseMipLevel = 0;
            barriers[i].subresourceRange.levelCount = 1;
        } else {
            barriers[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            barriers[i].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barriers[i].srcAccessMask = 0;
            barriers[i].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barriers[i].subresourceRange.baseMipLevel = 1;
            barriers[i].subresourceRange.levelCount = info.levels - 1;
        }
    }

    this->Barrier(srcStage, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, NULL, 0, NULL,
        needTopLevelBarrier ? 2 : 1,
        needTopLevelBarrier ? barriers : barriers + 1);
}

void Renderer::CommandBuffer::GenerateMipmap(const Image& image)
{
    // https://vulkan-tutorial.com/Generating_Mipmaps
    // https://vulkan-tutorial.com/code/28_mipmapping.cpp

    const auto& info = image.GetInfo();
    VkOffset3D size = { int32(info.width), int32(info.height), int32(info.depth) }; 
    const VkOffset3D origin = { 0, 0, 0 };

    // ASSERT(info.layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    VkImageMemoryBarrier imgBarrier;
    imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imgBarrier.pNext = NULL;
    imgBarrier.image = image.GetApiObject();
    imgBarrier.subresourceRange = { FormatToAspectMask(info.format), 0, 1, 0, info.layers };
    imgBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imgBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    for (uint32 i = 1; i < info.levels; i++) {
        VkOffset3D srcSize = size;
        size.x = TRE::Math::Max(size.x >> 1, 1);
        size.y = TRE::Math::Max(size.y >> 1, 1);
        size.z = TRE::Math::Max(size.z >> 1, 1);

        this->BlitImage(image, image,
            origin, size, origin, srcSize, i, i - 1, 0, 0, info.layers, VK_FILTER_LINEAR);

        imgBarrier.subresourceRange.baseMipLevel = i;
        this->Barrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, NULL, 0, NULL, 1, &imgBarrier);
    }
}

void Renderer::CommandBuffer::CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, DeviceSize size, DeviceSize srcOffset, DeviceSize dstOffset)
{
    ASSERT(size >= dstBuffer.GetBufferInfo().size);

    VkBufferCopy copy;
    copy.srcOffset = srcOffset;
    copy.dstOffset = dstOffset;
    copy.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer.GetApiObject(), dstBuffer.GetApiObject(), 1, &copy);
}

void Renderer::CommandBuffer::CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer)
{
    VkBufferCopy copy;
    copy.srcOffset = 0;
    copy.dstOffset = 0;
    copy.size = dstBuffer.GetBufferInfo().size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer.GetApiObject(), dstBuffer.GetApiObject(), 1, &copy);
}

void Renderer::CommandBuffer::CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, const VectorView<VkBufferCopy>& copies)
{
    vkCmdCopyBuffer(commandBuffer, srcBuffer.GetApiObject(), dstBuffer.GetApiObject(), copies.size, copies.data);
}

void Renderer::CommandBuffer::CopyBufferToImage(const Buffer& srcBuffer, const Image& dstImage, VkDeviceSize bufferOffset,
    const VkOffset3D& imageOffset, const VkExtent3D& imageExtent, uint32_t bufferRowLength, uint32_t bufferImageHeight, 
    const VkImageSubresourceLayers& imageSubresource)
{
    const VkBufferImageCopy copy = {
        bufferOffset,
        bufferRowLength, bufferImageHeight,
        imageSubresource, imageOffset, imageExtent
    };

    this->CopyBufferToImage(srcBuffer, dstImage, {&copy, 1});
}

void Renderer::CommandBuffer::CopyBufferToImage(const Buffer& srcBuffer, const Image& dstImage, VkDeviceSize bufferOffset,
    uint32 mipLevel, uint32_t bufferRowLength, uint32_t bufferImageHeight)
{
    const auto& info = dstImage.GetInfo();
    VkBufferImageCopy copy;
    copy.bufferOffset = bufferOffset;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource = { FormatToAspectMask(info.format) , mipLevel, 0, info.layers };
    copy.imageOffset = { 0, 0, 0 };
    copy.imageExtent = { info.width, info.height, info.depth };

    this->CopyBufferToImage(srcBuffer, dstImage, { &copy, 1 });
}

void Renderer::CommandBuffer::CopyBufferToImage(const Buffer& srcBuffer, const Image& dstImage, const VectorView<VkBufferImageCopy>& copies)
{
    vkCmdCopyBufferToImage(commandBuffer, srcBuffer.GetApiObject(), dstImage.GetApiObject(),
        dstImage.GetLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL), 
        copies.size, copies.data);
}

void Renderer::CommandBuffer::CopyImageToBuffer(const Image& srcImage, const Buffer& dstBuffer, VkDeviceSize bufferOffset,
    const VkOffset3D& imageOffset, const VkExtent3D& imageExtent, uint32_t bufferRowLength, uint32_t bufferImageHeight, 
    const VkImageSubresourceLayers& imageSubresource)
{
    const VkBufferImageCopy copy = {
        bufferOffset,
        bufferRowLength, bufferImageHeight,
        imageSubresource, imageOffset, imageExtent
    };

    this->CopyImageToBuffer(srcImage, dstBuffer, { &copy, 1 });
}

void Renderer::CommandBuffer::CopyImageToBuffer(const Image& srcImage, const Buffer& dstBuffer, VkDeviceSize bufferOffset, 
    uint32 mipLevel, uint32_t bufferRowLength, uint32_t bufferImageHeight)
{
    const auto& info = srcImage.GetInfo();
    VkBufferImageCopy copy;
    copy.bufferOffset = bufferOffset;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource = { FormatToAspectMask(info.format) , mipLevel, 0, info.layers };
    copy.imageOffset = { 0, 0, 0 };
    copy.imageExtent = { info.width, info.height, info.depth };

    this->CopyImageToBuffer(srcImage, dstBuffer, { &copy, 1 });
}

void Renderer::CommandBuffer::CopyImageToBuffer(const Image& srcImage, const Buffer& dstBuffer, const VectorView<VkBufferImageCopy>& copies)
{
    vkCmdCopyImageToBuffer(commandBuffer, srcImage.GetApiObject(),
        srcImage.GetLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL),
        dstBuffer.GetApiObject(), copies.size, copies.data);
}

void Renderer::CommandBuffer::CopyImage(const Image& srcImage, const Image& dstImage, const VkExtent3D& extent, 
    uint32 srcMipLevel, uint32 dstMipLevel, const VkOffset3D& srcOffset, const VkOffset3D& dstOffset)
{
    VkImageCopy copy;
    const auto& srcInfo = srcImage.GetInfo();
    const auto& dstInfo = dstImage.GetInfo();
    copy.srcSubresource = { FormatToAspectMask(srcInfo.format), srcMipLevel, 0, srcInfo.layers};
    copy.srcOffset = srcOffset;
    copy.dstSubresource = { FormatToAspectMask(dstInfo.format), dstMipLevel, 0, dstInfo.layers };
    copy.dstOffset = dstOffset;
    copy.extent = extent;
    this->CopyImage(srcImage, dstImage, VectorView<VkImageCopy>{&copy, 1});
}

void Renderer::CommandBuffer::CopyImage(const Image& srcImage, const Image& dstImage, const VectorView<VkImageCopy>& regions)
{
    vkCmdCopyImage(commandBuffer,
        srcImage.GetApiObject(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dstImage.GetApiObject(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        regions.size, regions.data
    );
}

void Renderer::CommandBuffer::BlitImage(const Image& dst, const Image& src, const VkOffset3D& dstOffset, const VkOffset3D& dstExtent,
    const VkOffset3D& srcOffset, const VkOffset3D& srcExtent, uint32 dstLevel, uint32 srcLevel, uint32 dstBaseLayer, uint32 srcBaseLayer, 
    uint32 numLayers, VkFilter filter)
{
    const auto AddOffset = [](const VkOffset3D& a, const VkOffset3D& b) -> VkOffset3D
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    };

    const VkImageBlit blit = {
        { FormatToAspectMask(src.GetInfo().format), srcLevel, srcBaseLayer, numLayers },
        { srcOffset, AddOffset(srcOffset, srcExtent) },
        { FormatToAspectMask(dst.GetInfo().format), dstLevel, dstBaseLayer, numLayers },
        { dstOffset, AddOffset(dstOffset, dstExtent) },
    };

    vkCmdBlitImage(commandBuffer,
        src.GetApiObject(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dst.GetApiObject(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &blit,
        filter);
}

Renderer::EventHandle Renderer::CommandBuffer::SignalEvent(VkPipelineStageFlags stages)
{
    auto event = renderBackend->RequestPiplineEvent();
    vkCmdSetEvent(commandBuffer, event->GetApiObject(), stages);
    event->SetStages(stages);
    return event;
}

void Renderer::CommandBuffer::WaitEvents(EventHandle* event, uint32 eventsCount, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages,
    const VkMemoryBarrier* barriers, uint32 barriersCount, const VkBufferMemoryBarrier* buffersBarriers, uint32 bufferCount, 
    const VkImageMemoryBarrier* imagesBarriers, uint32 imageCount)
{
    StaticVector<VkEvent> events;
    
    for (uint32 i = 0; i < eventsCount; i++) {
        events.EmplaceBack((*event)->GetApiObject());
    }

    vkCmdWaitEvents(commandBuffer, eventsCount, events.Data(), srcStages, dstStages, 
        barriersCount, barriers, bufferCount, buffersBarriers, imageCount, imagesBarriers);
}

void Renderer::CommandBuffer::FullBarrier()
{
    this->Barrier(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_WRITE_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT);
}

void Renderer::CommandBuffer::PixelBarrier()
{
    VkMemoryBarrier barrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER };
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_DEPENDENCY_BY_REGION_BIT, 1, &barrier, 0, NULL, 0, NULL);
}

void Renderer::CommandBuffer::Barrier(VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages, uint32 barriers, 
    const VkMemoryBarrier* globals, uint32 bufferBarriers, const VkBufferMemoryBarrier* buffers, uint32 imageBarriers, 
    const VkImageMemoryBarrier* images)
{
    vkCmdPipelineBarrier(commandBuffer, srcStages, dstStages, 0, barriers, globals, bufferBarriers, buffers, imageBarriers, images);
}

void Renderer::CommandBuffer::Barrier(VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess)
{
    VkMemoryBarrier barrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER };
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 1, &barrier, 0, NULL, 0, NULL);
}

void Renderer::CommandBuffer::BufferBarrier(const Buffer& buffer, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess)
{
    VkBufferMemoryBarrier barrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    barrier.buffer = buffer.GetApiObject();
    barrier.offset = 0;
    barrier.size = buffer.GetBufferInfo().size;

    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, NULL, 1, &barrier, 0, NULL);
}

void Renderer::CommandBuffer::ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, 
    VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess)
{
    ASSERT(image.GetInfo().domain == ImageDomain::TRANSIENT);

    VkImageMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = NULL;
    barrier.image = image.GetApiObject();
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange = { FormatToAspectMask(image.GetInfo().format), 0, image.GetInfo().levels, 0, image.GetInfo().layers };

    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, NULL, 0, NULL, 1, &barrier);
}

void Renderer::CommandBuffer::ChangeImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, 
    VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask)
{
    VkImageMemoryBarrier imageMemoryBarrier;
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext = NULL;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.oldLayout = oldLayout;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.image = image.GetApiObject();
    imageMemoryBarrier.subresourceRange = subresourceRange;
    imageMemoryBarrier.srcAccessMask = ImageOldLayoutToPossibleSrcAccess(oldLayout);
    imageMemoryBarrier.dstAccessMask = ImageNewLayoutToPossibleDstAccess(newLayout, &imageMemoryBarrier);

    vkCmdPipelineBarrier(
        commandBuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, NULL,
        0, NULL,
        1, &imageMemoryBarrier);
}

void Renderer::CommandBuffer::UpdateDescriptorSet(uint32 set, VkDescriptorSet descSet, const DescriptorSetLayout& layout, const ResourceBinding* bindings)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);

    VkWriteDescriptorSet writes[MAX_DESCRIPTOR_BINDINGS];
    uint32 writeCount = 0;

    /*
    * NOT YET IMPLEMENTED/
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
    VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
    VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT = 1000138000,
    */

    for (uint32 binding = 0; binding < layout.GetBindingsCount(); binding++) {
        auto& layoutBinding = layout.GetDescriptorSetLayoutBinding(binding);

        if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            || layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
            || layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
            || layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
            for (uint32 i = 0; i < layoutBinding.descriptorCount; i++) {
                writes[writeCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[writeCount].pNext = NULL;
                writes[writeCount].dstSet = descSet;
                writes[writeCount].descriptorType = layoutBinding.descriptorType;
                writes[writeCount].descriptorCount = 1;
                writes[writeCount].dstBinding = binding;
                writes[writeCount].dstArrayElement = i;
                writes[writeCount].pBufferInfo = &bindings[binding + i].resource.buffer;
                writes[writeCount].pImageInfo = NULL;
                writes[writeCount].pTexelBufferView = NULL;
            }

            writeCount++;
        } else if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER 
            || layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
            || layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER
            || layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
            || layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
            for (uint32 i = 0; i < layoutBinding.descriptorCount; i++) {
                writes[writeCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[writeCount].pNext = NULL;
                writes[writeCount].dstSet = descSet;
                writes[writeCount].descriptorType = layoutBinding.descriptorType;
                writes[writeCount].descriptorCount = 1;
                writes[writeCount].dstBinding = binding;
                writes[writeCount].dstArrayElement = i;
                writes[writeCount].pBufferInfo = NULL;
                writes[writeCount].pImageInfo = &bindings[binding + i].resource.image;
                writes[writeCount].pTexelBufferView = NULL;
            }

            writeCount++;
        } else if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
            for (uint32 i = 0; i < layoutBinding.descriptorCount; i++) {
                writes[writeCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[writeCount].pNext = &bindings[binding + i].resource.accl;
                writes[writeCount].dstSet = descSet;
                writes[writeCount].descriptorType = layoutBinding.descriptorType;
                writes[writeCount].descriptorCount = 1;
                writes[writeCount].dstBinding = binding;
                writes[writeCount].dstArrayElement = i;
                writes[writeCount].pBufferInfo = NULL;
                writes[writeCount].pImageInfo = NULL;
                writes[writeCount].pTexelBufferView = NULL;
            }

            writeCount++;
        }
    }

    printf("Updating descriptor sets: writting count:%u\n", writeCount);
    vkUpdateDescriptorSets(renderBackend->GetRenderDevice().GetDevice(), writeCount, writes, 0, NULL);
}

void Renderer::CommandBuffer::FlushDescriptorSet(uint32 set)
{
    ASSERT(pipeline == NULL);
    ASSERT(set >= MAX_DESCRIPTOR_SET);

    const PipelineLayout& layout = pipeline->GetShaderProgram()->GetPipelineLayout();
    const DescriptorSetLayout& setLayout = layout.GetDescriptorSetLayout(set);
    const VkDescriptorSetLayoutBinding* setBindings = setLayout.GetDescriptorSetLayoutBindings();
    const ResourceBinding* resourceBinding = bindings.bindings[set];
    uint32 dyncOffset[MAX_DESCRIPTOR_BINDINGS];
    uint32 numDyncOffset = 0;
    Hasher h;

    for (uint32 i = 0; i < setLayout.GetBindingsCount(); i++) {
        auto& bindingLayout = setBindings[i];

        for (uint32 j = 0; j < bindingLayout.descriptorCount; j++) {
            uint32 bindingResourceIndex = bindingLayout.binding + j;
            h.Data(reinterpret_cast<const uint32*>(&resourceBinding[bindingResourceIndex].resource), sizeof(ResourceBinding::Resource));

            if (setBindings[bindingResourceIndex].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC 
                || setBindings[bindingResourceIndex].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
                dyncOffset[numDyncOffset++] = resourceBinding[bindingResourceIndex].dynamicOffset;
            }
        }
    }

    Hash hash = h.Get();
    std::pair<VkDescriptorSet, bool> alloc = layout.GetAllocator(set)->Find(hash);

    if (!alloc.second) {
        this->UpdateDescriptorSet(set, alloc.first, setLayout, resourceBinding);
    }
    
    vkCmdBindDescriptorSets(
        commandBuffer, (VkPipelineBindPoint)pipeline->GetPipelineType(), pipeline->GetPipelineLayout().GetApiObject(),
        set, 1, &alloc.first, numDyncOffset, dyncOffset);

    allocatedSets[set] = alloc.first;
    dirty.sets = dirty.sets & ~(1u << set);
}

void Renderer::CommandBuffer::FlushDescriptorSets()
{
    if (pipeline == NULL) {
        this->BindPipeline();
    }

    if (!dirty.sets)
        return;

    uint8 dirtySets = dirty.sets;

    for (uint32 set = 0; set < MAX_DESCRIPTOR_SET; set++) {
        if (dirty.sets & (1u << set)) {
            this->FlushDescriptorSet(set);
        }
    }

    dirty.dynamicSets &= ~dirtySets;

    if (dirty.dynamicSets) {
        for (uint32 set = 0; set < MAX_DESCRIPTOR_SET; set++) {
            if (dirty.dynamicSets & (1u << set)) {
                this->RebindDescriptorSet(set);
            }
        }

        dirty.dynamicSets = 0;
    }
}

void Renderer::CommandBuffer::RebindDescriptorSet(uint32 set)
{
    const PipelineLayout& layout = pipeline->GetShaderProgram()->GetPipelineLayout();
    const DescriptorSetLayout& setLayout = layout.GetDescriptorSetLayout(set);
    const VkDescriptorSetLayoutBinding* setBindings = setLayout.GetDescriptorSetLayoutBindings();
    const ResourceBinding* resourceBinding = bindings.bindings[set];
    uint32 dyncOffset[MAX_DESCRIPTOR_BINDINGS];
    uint32 numDyncOffset = 0;
    Hasher h;

    for (uint32 i = 0; i < setLayout.GetBindingsCount(); i++) {
        auto& bindingLayout = setBindings[i];
        for (uint32 j = 0; j < bindingLayout.descriptorCount; j++) {
            uint32 bindingResourceIndex = bindingLayout.binding + j;
            if (setBindings[bindingResourceIndex].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                || setBindings[bindingResourceIndex].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
                dyncOffset[numDyncOffset++] = resourceBinding[bindingResourceIndex].dynamicOffset;
            }
        }
    }

    auto sets = &allocatedSets[set];
    vkCmdBindDescriptorSets(
        commandBuffer, (VkPipelineBindPoint)pipeline->GetPipelineType(), pipeline->GetPipelineLayout().GetApiObject(),
        set, 1, sets, numDyncOffset, dyncOffset);
}

void Renderer::CommandBuffer::InitViewportScissor(const RenderPassInfo& info, const Framebuffer* fb)
{
    VkRect2D rect = info.renderArea;
    rect.offset.x = TRE::Math::Min(fb->GetWidth(), uint32_t(rect.offset.x));
    rect.offset.y = TRE::Math::Min(fb->GetHeight(), uint32_t(rect.offset.y));
    rect.extent.width = TRE::Math::Min(fb->GetWidth() - rect.offset.x, rect.extent.width);
    rect.extent.height = TRE::Math::Min(fb->GetHeight() - rect.offset.y, rect.extent.height);

    viewport = { 0.0f, 0.0f, float(fb->GetWidth()), float(fb->GetHeight()), 0.0f, 1.0f };
    scissor  = rect;
}

void Renderer::CommandBuffer::BindPipeline()
{
    ASSERT(!program);
    ASSERT(!renderPass); // Only if its graphics


    // TODO: think about the renderpass we just need a compatible one not one that is exactly fit
    // The one we have is the exact renderpass which will cause the creation of multiple PSO's 
    // that we can't afford the time for
    if (type == Type::GENERIC && renderPass) {
        if (stateUpdate) {
            state->SaveChanges();
        }

        this->BindPipeline(renderBackend->RequestPipeline(*program, *renderPass, *state));

        if (pipeline->IsStateDynamic(VK_DYNAMIC_STATE_VIEWPORT)) {
            this->SetViewport(viewport);
        }

        if (pipeline->IsStateDynamic(VK_DYNAMIC_STATE_SCISSOR)) {
            this->SetScissor(scissor);
        }

        if (pipeline->IsStateDynamic(VK_DYNAMIC_STATE_LINE_WIDTH)) {
            vkCmdSetLineWidth(commandBuffer, state->rasterizationState.lineWidth);
        }

        if (pipeline->IsStateDynamic(VK_DYNAMIC_STATE_DEPTH_BIAS)) {
            vkCmdSetDepthBias(commandBuffer, state->rasterizationState.depthBiasConstantFactor, state->rasterizationState.depthBiasClamp,
                state->rasterizationState.depthBiasSlopeFactor);
        }

        if (pipeline->IsStateDynamic(VK_DYNAMIC_STATE_BLEND_CONSTANTS)) {
            vkCmdSetBlendConstants(commandBuffer, state->colorBlendState.blendConstants);
        }

        if (pipeline->IsStateDynamic(VK_DYNAMIC_STATE_DEPTH_BOUNDS)) {
            vkCmdSetDepthBounds(commandBuffer, state->depthStencilState.minDepthBounds, state->depthStencilState.maxDepthBounds);
        }
    } else {
        this->BindPipeline(renderBackend->RequestPipeline(*program));
    }
}


void Renderer::CommandBuffer::SetPrimitiveTopology(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
{
    ASSERT(!state);

    state->inputAssemblyState.topology = topology;
    state->inputAssemblyState.primitiveRestartEnable = primitiveRestartEnable;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetDepthClampEnable(VkBool32 depthClampEnable)
{
    ASSERT(!state);

    state->rasterizationState.depthClampEnable = depthClampEnable;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetRasterizerDiscardEnable(VkBool32 rasterizerDiscardEnable)
{
    ASSERT(!state);

    state->rasterizationState.rasterizerDiscardEnable = rasterizerDiscardEnable;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetPolygonMode(VkPolygonMode polygonMode)
{
    ASSERT(!state);

    state->rasterizationState.polygonMode = polygonMode;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetCullMode(VkCullModeFlags cullMode)
{
    ASSERT(!state);

    state->rasterizationState.cullMode = cullMode;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetFrontFrace(VkFrontFace frontFace)
{
    ASSERT(!state);

    state->rasterizationState.frontFace = frontFace;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetDepthBias(VkBool32 depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    ASSERT(!state);

    state->rasterizationState.depthBiasEnable = depthBiasEnable;
    state->rasterizationState.depthBiasConstantFactor = depthBiasConstantFactor;
    state->rasterizationState.depthBiasClamp = depthBiasClamp;
    state->rasterizationState.depthBiasSlopeFactor = depthBiasSlopeFactor;

    if (!(pipeline && pipeline->IsStateDynamic(VK_DYNAMIC_STATE_DEPTH_BIAS))) {
        stateUpdate = true;
    }
}

void Renderer::CommandBuffer::SetLineWidth(float lineWidth)
{
    ASSERT(!state);

    state->rasterizationState.lineWidth = lineWidth;

    if (!(pipeline && pipeline->IsStateDynamic(VK_DYNAMIC_STATE_LINE_WIDTH))) {
        stateUpdate = true;
    }
}

void Renderer::CommandBuffer::SetDepthTest(VkBool32 depthTestEnable, VkBool32 depthWriteEnable)
{
    ASSERT(!state);

    state->depthStencilState.depthTestEnable = depthTestEnable;
    state->depthStencilState.depthWriteEnable = depthWriteEnable;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetDepthBounds(float minDepthBounds, float maxDepthBounds)
{
    ASSERT(!state);

    state->depthStencilState.minDepthBounds = minDepthBounds;
    state->depthStencilState.maxDepthBounds = maxDepthBounds;

    if (!(pipeline && pipeline->IsStateDynamic(VK_DYNAMIC_STATE_DEPTH_BOUNDS))) {
        stateUpdate = true;
    }
}

void Renderer::CommandBuffer::SetDepthCompareOp(VkCompareOp depthCompareOp)
{
    ASSERT(!state);

    state->depthStencilState.depthCompareOp = depthCompareOp;

    stateUpdate = true;
}

void Renderer::CommandBuffer::EnableDepthBoundsTest(VkBool32 depthBoundsTestEnable)
{
    ASSERT(!state);

    state->depthStencilState.depthBoundsTestEnable = depthBoundsTestEnable;

    stateUpdate = true;
}

void Renderer::CommandBuffer::EnableStencilTest(VkBool32 stencilTestEnable)
{
    ASSERT(!state);

    state->depthStencilState.stencilTestEnable = stencilTestEnable;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetStencilOpState(VkStencilOpState front, VkStencilOpState back)
{
    ASSERT(!state);

    state->depthStencilState.front = front;
    state->depthStencilState.back = back;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetLogicOp(VkBool32 logicOpEnable, VkLogicOp logicOp)
{
    ASSERT(!state);

    state->colorBlendState.logicOpEnable = logicOpEnable;
    state->colorBlendState.logicOpEnable = logicOpEnable;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetBlendConstants(float blendConstants[4])
{
    ASSERT(!state);

    memcpy(state->colorBlendState.blendConstants, blendConstants, 4 * sizeof(float));

    if (!(pipeline && pipeline->IsStateDynamic(VK_DYNAMIC_STATE_BLEND_CONSTANTS))) {
        stateUpdate = true;
    }
}

void Renderer::CommandBuffer::SetAttachmentBlendFactor(uint32 attach, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, 
    VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor)
{
    ASSERT(!state);
    ASSERT(attach > state->colorBlendState.attachmentCount);

    state->colorBlendAttachmetns[attach].srcColorBlendFactor = srcColorBlendFactor;
    state->colorBlendAttachmetns[attach].dstColorBlendFactor = dstColorBlendFactor;
    state->colorBlendAttachmetns[attach].srcAlphaBlendFactor = srcAlphaBlendFactor;
    state->colorBlendAttachmetns[attach].dstAlphaBlendFactor = dstAlphaBlendFactor;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetAttachmentBlendOp(uint32 attach, VkBlendOp colorBlendOp, VkBlendOp alphaBlendOp)
{
    ASSERT(!state);
    ASSERT(attach > state->colorBlendState.attachmentCount);

    state->colorBlendAttachmetns[attach].colorBlendOp = colorBlendOp;
    state->colorBlendAttachmetns[attach].alphaBlendOp = alphaBlendOp;

    stateUpdate = true;
}

void Renderer::CommandBuffer::SetAttachmentColorWriteMask(uint32 attach, VkColorComponentFlags colorWriteMask)
{
    ASSERT(!state);
    ASSERT(attach > state->colorBlendState.attachmentCount);

    state->colorBlendAttachmetns[attach].colorWriteMask = colorWriteMask;
    
    stateUpdate = true;
}

TRE_NS_END