#include "CommandList.hpp"
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/Pipeline/GraphicsPipeline.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetAlloc.hpp>
#include <Renderer/Backend/Images/Image.hpp>
#include <Renderer/Backend/Images/Sampler.hpp>

TRE_NS_START

Renderer::CommandBuffer::CommandBuffer(RenderBackend* backend, VkCommandBuffer buffer, Type type) :
    renderBackend(backend), commandBuffer(buffer), type(type), allocatedSets{}, dirtySets{}
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
}

void Renderer::CommandBuffer::BeginRenderPass(const RenderPassInfo& info, VkSubpassContents contents)
{
    renderPass = &renderBackend->RequestRenderPass(info);
    framebuffer = &renderBackend->RequestFramebuffer(info, renderPass);
    this->InitViewportScissor(info, framebuffer);

    VkClearValue clearValues[MAX_ATTACHMENTS + 1];
    uint32 clearValuesCount = 0;

    for (unsigned i = 0; i < info.colorAttachmentCount; i++) {
        ASSERT(!info.colorAttachments[i]);

        if (info.clearAttachments & (1u << i)) {
            clearValues[i].color = info.clearColor[i];
            clearValuesCount = i + 1;
        }
    }

    if (info.depthStencil && (info.opFlags & RENDER_PASS_OP_CLEAR_DEPTH_STENCIL_BIT) != 0) {
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
}

void Renderer::CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
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
    b.resource.buffer = VkDescriptorBufferInfo{ buffer.GetAPIObject(), 0, range };
    b.dynamicOffset   = (uint32)offset;

    dirtySets.dirtySets |= (1u << set);
    dirtySets.dirtyBindings[set] |= (1u << binding);
}

void Renderer::CommandBuffer::SetTexture(uint32 set, uint32 binding, const ImageView& texture)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    auto& b = bindings.bindings[set][binding];

    b.resource.image.imageView = texture.GetAPIObject();
    b.resource.image.imageLayout = texture.GetInfo().image->GetInfo().layout;

    dirtySets.dirtySets |= (1u << set);
    dirtySets.dirtyBindings[set] |= (1u << binding);
}

void Renderer::CommandBuffer::SetSampler(uint32 set, uint32 binding, const Sampler& sampler)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(binding >= MAX_DESCRIPTOR_BINDINGS);

    auto& b = bindings.bindings[set][binding];
    b.resource.image.sampler = sampler.GetAPIObject();

    dirtySets.dirtySets |= (1u << set);
    dirtySets.dirtyBindings[set] |= (1u << binding);
}

void Renderer::CommandBuffer::SetTexture(uint32 set, uint32 binding, const ImageView& texture, const Sampler& sampler)
{
    this->SetTexture(set, binding, texture);
    this->SetSampler(set, binding, sampler);
}

void Renderer::CommandBuffer::UpdateDescriptorSet(uint32 set, VkDescriptorSet descSet, const DescriptorSetLayout& layout, const ResourceBinding* bindings)
{
    ASSERT(set >= MAX_DESCRIPTOR_SET);
    ASSERT(dirtySets.dirtyBindings[set] == 0);

    VkWriteDescriptorSet writes[MAX_DESCRIPTOR_BINDINGS];
    uint32 writeCount = 0;

    for (uint32 binding = 0; binding < MAX_DESCRIPTOR_BINDINGS; binding++) {
        //if (dirtySets.dirtyBindings[set] & (1u << binding)) {
            if (layout.GetDescriptorSetLayoutBinding(binding).descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) {
                writes[writeCount].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[writeCount].pNext            = NULL;
                writes[writeCount].dstSet           = descSet;
                writes[writeCount].descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                writes[writeCount].descriptorCount  = 1;
                writes[writeCount].dstBinding       = binding;
                writes[writeCount].dstArrayElement  = 0;
                writes[writeCount].pBufferInfo      = &bindings[binding].resource.buffer;
                writes[writeCount].pImageInfo       = NULL;
                writes[writeCount].pTexelBufferView = NULL;

                writeCount++;
            } else if (layout.GetDescriptorSetLayoutBinding(binding).descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                writes[writeCount].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[writeCount].pNext            = NULL;
                writes[writeCount].dstSet           = descSet;
                writes[writeCount].descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writes[writeCount].descriptorCount  = 1;
                writes[writeCount].dstBinding       = binding;
                writes[writeCount].dstArrayElement  = 0;
                writes[writeCount].pBufferInfo      = NULL;
                writes[writeCount].pImageInfo       = &bindings[binding].resource.image;
                writes[writeCount].pTexelBufferView = NULL;

                writeCount++;
            }
        //}
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

    for (uint32 binding = 0; binding < setLayout.GetBindingsCount(); binding++) {
        h.Data(reinterpret_cast<const uint32*>(&resourceBinding[binding].resource), sizeof(ResourceBinding::Resource));

        if (setBindings[binding].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) {
            dyncOffset[numDyncOffset++] = resourceBinding[binding].dynamicOffset;
        }
    }

    Hash hash = h.Get();
    std::pair<VkDescriptorSet, bool> alloc = layout.GetAllocator(set)->Find(hash);

    if (!alloc.second) {
        this->UpdateDescriptorSet(set, alloc.first, setLayout, resourceBinding);
    }
    
    vkCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout().GetAPIObject(), 
        0, 1, &alloc.first, numDyncOffset, dyncOffset);

    allocatedSets[set] = alloc.first;
    dirtySets.dirtySets = dirtySets.dirtySets & ~(1u << set);
    dirtySets.dirtyBindings[set] = 0;
}

void Renderer::CommandBuffer::FlushDescriptorSets()
{
    if (!dirtySets.dirtySets)
        return;

    for (uint32 set = 0; set < MAX_DESCRIPTOR_SET; set++) {
        if (dirtySets.dirtySets & (1u << set)) {
            this->FlushDescriptorSet(set);
        }
    }
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

TRE_NS_END
