#include "CommandList.hpp"
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/Pipeline/GraphicsPipeline.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetAlloc.hpp>
#include <Renderer/Backend/Images/Image.hpp>
#include <Renderer/Backend/Images/Sampler.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Buffers/RingBuffer.hpp>

TRE_NS_START

Renderer::CommandBuffer::CommandBuffer(RenderBackend* backend, VkCommandBuffer buffer, Type type) :
    renderBackend(backend), commandBuffer(buffer), type(type), allocatedSets{}, dirty{}, renderToSwapchain(false)
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
    beginInfo.renderPass        = renderPass->GetAPIObject();
    beginInfo.framebuffer       = framebuffer->GetAPIObject();
    beginInfo.renderArea        = scissor;
    beginInfo.clearValueCount   = clearValuesCount;
    beginInfo.pClearValues      = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, contents);
    this->SetViewport(viewport);
    this->SetScissor(scissor);

    subpassIndex = 0;
}

void Renderer::CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::CommandBuffer::NextRenderPass(VkSubpassContents contents)
{
    ASSERT(renderPass != NULL);
    vkCmdNextSubpass(commandBuffer, contents);
    subpassIndex++;
}

void Renderer::CommandBuffer::BindPipeline(const GraphicsPipeline& pipeline)
{
    this->pipeline = &pipeline;
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetAPIObject());
}

void Renderer::CommandBuffer::BindVertexBuffer(const Buffer& buffer, DeviceSize offset)
{
    VkBuffer vertexBuffers[] = { buffer.GetAPIObject() };
    VkDeviceSize offsets[]   = { offset };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void Renderer::CommandBuffer::BindIndexBuffer(const Buffer& buffer, DeviceSize offset)
{
    vkCmdBindIndexBuffer(commandBuffer, buffer.GetAPIObject(), offset, VK_INDEX_TYPE_UINT16);
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

void Renderer::CommandBuffer::BindDescriptorSet(const GraphicsPipeline& pipeline, const std::initializer_list<VkDescriptorSet>& descriptors, 
    const std::initializer_list<uint32>& dyncOffsets)
{
    vkCmdBindDescriptorSets(commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline.GetPipelineLayout().GetAPIObject(),
        0, (uint32)descriptors.size(), descriptors.begin(), (uint32)dyncOffsets.size(), dyncOffsets.begin());
}

void Renderer::CommandBuffer::SetUniformBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset, DeviceSize range)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    auto& b = bindings.bindings[set][binding];
    b.resource.buffer = VkDescriptorBufferInfo{ buffer.GetAPIObject(), offset, range };
    b.dynamicOffset   = 0;

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
    b.resource.buffer = VkDescriptorBufferInfo{ buffer.GetAPIObject(), offset, range };
    b.dynamicOffset   = buffer.GetCurrentOffset();

    dirty.sets |= (1u << set);

    // TODO: think about what if everything is the same just the offset changed! we shouldnt then update dirty set instead just update the set
    // that have the dynamic offset set in it
    /*
    if (nothing changed and only dynamicOffset changed) {
        dirty_sets_dynamic |= 1u << set;
    }else{
        // update everything
    }
    */
}

void Renderer::CommandBuffer::SetStorageBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset, DeviceSize range)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    auto& b = bindings.bindings[set][binding];
    b.resource.buffer = VkDescriptorBufferInfo{ buffer.GetAPIObject(), offset, range };
    b.dynamicOffset = 0;

    dirty.sets |= (1u << set);

    // TODO: the set storage buffer and set uniform buffer are similar! Maybe we can optimise using some tricks and dynamic offsets
}

void Renderer::CommandBuffer::SetTexture(uint32 set, uint32 binding, const ImageView& texture)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    auto& b = bindings.bindings[set][binding];

    b.resource.image.imageView = texture.GetAPIObject();
    b.resource.image.imageLayout = texture.GetInfo().image->GetInfo().layout;

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
    b.resource.image.sampler = sampler.GetAPIObject();
    // b.resource.image.imageView = VK_NULL_HANDLE;

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
        b.resource.image.imageView = view->GetAPIObject();

        dirty.sets |= 1u << set;
    }
}

void Renderer::CommandBuffer::PushConstants(ShaderStagesFlags stages, const void* data, VkDeviceSize size, VkDeviceSize offset)
{
    const auto& pipelineLayout = pipeline->GetPipelineLayout();
    const auto& pushConstant = pipelineLayout.GetPushConstantRangeFromStage(stages);

    vkCmdPushConstants(commandBuffer, pipelineLayout.GetAPIObject(), stages, pushConstant.offset + (uint32)offset, (uint32)size, data);
}

void Renderer::CommandBuffer::PrepareGenerateMipmapBarrier(const Image& image, VkImageLayout baseLevelLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, bool needTopLevelBarrier)
{
    auto& info = image.GetInfo();
    VkImageMemoryBarrier barriers[2] = {};
    ASSERT(info.levels <= 1);

    for (uint32 i = 0; i < 2; i++) {
        barriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barriers[i].image = image.GetAPIObject();
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
    imgBarrier.image = image.GetAPIObject();
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
    vkCmdCopyBuffer(commandBuffer, srcBuffer.GetAPIObject(), dstBuffer.GetAPIObject(), 1, &copy);
}

void Renderer::CommandBuffer::CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer)
{
    VkBufferCopy copy;
    copy.srcOffset = 0;
    copy.dstOffset = 0;
    copy.size = dstBuffer.GetBufferInfo().size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer.GetAPIObject(), dstBuffer.GetAPIObject(), 1, &copy);
}

void Renderer::CommandBuffer::CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, const VectorView<VkBufferCopy>& copies)
{
    vkCmdCopyBuffer(commandBuffer, srcBuffer.GetAPIObject(), dstBuffer.GetAPIObject(), copies.size, copies.data);
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
    vkCmdCopyBufferToImage(commandBuffer, srcBuffer.GetAPIObject(), dstImage.GetAPIObject(),
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

void Renderer::CommandBuffer::CopyImageToBuffer(const Image& srcImage, const Buffer& dstBuffer, VkDeviceSize bufferOffset, uint32 mipLevel, uint32_t bufferRowLength, uint32_t bufferImageHeight)
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
    vkCmdCopyImageToBuffer(commandBuffer, srcImage.GetAPIObject(),
        srcImage.GetLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL),
        dstBuffer.GetAPIObject(), copies.size, copies.data);
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
        src.GetAPIObject(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dst.GetAPIObject(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &blit,
        filter);
}

Renderer::EventHandle Renderer::CommandBuffer::SignalEvent(VkPipelineStageFlags stages)
{
    auto event = renderBackend->RequestPiplineEvent();
    vkCmdSetEvent(commandBuffer, event->GetAPIObject(), stages);
    event->SetStages(stages);
    return event;
}

void Renderer::CommandBuffer::WaitEvents(EventHandle* event, uint32 eventsCount, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages,
    const VkMemoryBarrier* barriers, uint32 barriersCount, const VkBufferMemoryBarrier* buffersBarriers, uint32 bufferCount, 
    const VkImageMemoryBarrier* imagesBarriers, uint32 imageCount)
{
    StaticVector<VkEvent> events;
    
    for (uint32 i = 0; i < eventsCount; i++) {
        events.EmplaceBack((*event)->GetAPIObject());
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
    barrier.buffer = buffer.GetAPIObject();
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
    barrier.image = image.GetAPIObject();
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange = { FormatToAspectMask(image.GetInfo().format), 0, image.GetInfo().levels, 0, image.GetInfo().layers };

    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, NULL, 0, NULL, 1, &barrier);
}

void Renderer::CommandBuffer::UpdateDescriptorSet(uint32 set, VkDescriptorSet descSet, const DescriptorSetLayout& layout, const ResourceBinding* bindings)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);

    VkWriteDescriptorSet writes[MAX_DESCRIPTOR_BINDINGS];
    uint32 writeCount = 0;

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
            || layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT) {
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
        } 
    }

    printf("Updating descriptor sets: writting count:%u\n", writeCount);
    vkUpdateDescriptorSets(renderBackend->GetRenderDevice().GetDevice(), writeCount, writes, 0, NULL);
}

void Renderer::CommandBuffer::FlushDescriptorSet(uint32 set)
{
    ASSERT(pipeline == NULL);
    ASSERT(set >= MAX_DESCRIPTOR_SET);

    const PipelineLayout& layout = pipeline->GetShaderProgram().GetPipelineLayout();
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
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout().GetAPIObject(), 
        set, 1, &alloc.first, numDyncOffset, dyncOffset);

    allocatedSets[set] = alloc.first;
    dirty.sets = dirty.sets & ~(1u << set);
}

void Renderer::CommandBuffer::FlushDescriptorSets()
{
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
    const PipelineLayout& layout = pipeline->GetShaderProgram().GetPipelineLayout();
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

    vkCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout().GetAPIObject(),
        set, 1, &allocatedSets[set], numDyncOffset, dyncOffset);
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

void Renderer::CommandBufferDeleter::operator()(CommandBuffer* cmd)
{
    cmd->renderBackend->GetObjectsPool().commandBuffers.Free(cmd);
}

TRE_NS_END


