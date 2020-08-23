#include "RenderContext.hpp"
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/WindowSurface/WindowSurface.hpp>
#include <Renderer/Backend/SwapChain/SwapChain.hpp>
#include <Renderer/Backend/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <unordered_set>

TRE_NS_START

Renderer::RenderContext::RenderContext() : internal{ 0 }, stagingManager{&internal}
{

}

void Renderer::RenderContext::CreateRenderContext(TRE::Window* wnd, const Internal::RenderInstance& instance)
{
    internal.window = wnd;
    internal.swapChainData.swapChainExtent = VkExtent2D{ internal.window->getSize().x, internal.window->getSize().y };

    CreateWindowSurface(instance, internal);
}

void Renderer::RenderContext::InitRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice)
{
    CreateSwapChain(renderDevice, internal);
    stagingManager.Init();
    gpuMemoryAllocator.Init(renderDevice);
}

void Renderer::RenderContext::DestroyRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice, Internal::RenderContext& renderContext)
{
    DestroySwapChain(renderDevice, renderContext);
    stagingManager.Shutdown();
    Internal::DestroryWindowSurface(renderInstance.instance, renderContext.surface);
}

void Renderer::RenderContext::ExecuteTransferMemory(VkQueue queue, VkCommandBuffer cmdBuff, VkPipelineStageFlags waitStage, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence, VkDevice device)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuff;


    if (waitSemaphore != VK_NULL_HANDLE) {
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphore;
        submitInfo.pWaitDstStageMask = &waitStage;
    }

    if (signalSemaphore != VK_NULL_HANDLE) {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphore;
    }

    vkQueueSubmit(queue, 1, &submitInfo, fence);

    if (fence && device) {
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &fence);
    }
}

void Renderer::RenderContext::BeginFrame(Internal::RenderDevice& renderDevice)
{
    using namespace Internal;

    SwapChainData& swapChainData = internal.swapChainData;
    RenderContextData& ctxData = internal.contextData;
    ContextFrameResources& frameResources = GetCurrentFrameResource();

    VkDevice device = renderDevice.device;
    uint32 currentFrame = internal.currentFrame;

    vkWaitForFences(device, 1, &swapChainData.fences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &swapChainData.fences[currentFrame]);

    VkResult result = vkAcquireNextImageKHR(device, internal.swapChain, UINT64_MAX, swapChainData.imageAcquiredSemaphores[currentFrame], VK_NULL_HANDLE, &internal.currentImage);
    
    stagingManager.Wait(stagingManager.GetCurrentStagingBuffer());
    stagingManager.Flush();

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChainInternal(renderDevice, internal);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        ASSERTF(true, "Failed to acquire swap chain image!\n");
    }
}

void Renderer::RenderContext::EndFrame(Internal::RenderDevice& renderDevice)
{
    using namespace Internal;

    SwapChainData& swapChainData = internal.swapChainData;
    RenderContextData& ctxData = internal.contextData;
    ContextFrameResources& frameResources = GetFrameResource(internal.currentFrame);

    VkDevice device = renderDevice.device;
    uint32 currentFrame = internal.currentFrame;
    uint32_t currentBuffer = internal.currentImage;

    

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkCommandBuffer commandBuffers[] = { frameResources.graphicsCommandBuffer };
    VkSemaphore waitSemaphores[] = { swapChainData.imageAcquiredSemaphores[currentFrame] };
    VkSemaphore signalSemaphores[] = { swapChainData.drawCompleteSemaphores[currentFrame] };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffers;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(renderDevice.queues[QFT_GRAPHICS], 1, &submitInfo, swapChainData.fences[currentFrame]) != VK_SUCCESS) {
        ASSERTF(true, "failed to submit draw command buffer!");
    }
    
    if (renderDevice.isPresentQueueSeprate) {
        // If we are using separate queues, change image ownership to the
        // present queue before presenting, waiting for the draw complete
        // semaphore and signalling the ownership released semaphore when
        // finished
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo present_submit_info{};
        present_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        present_submit_info.waitSemaphoreCount = 1;
        present_submit_info.pWaitSemaphores = &swapChainData.drawCompleteSemaphores[currentFrame];
        present_submit_info.pWaitDstStageMask = waitStages;

        present_submit_info.commandBufferCount = 1;
        present_submit_info.pCommandBuffers = &frameResources.presentCommandBuffer; // TODO: change this command buffer (to graphics to present cmd)

        present_submit_info.signalSemaphoreCount = 1;
        present_submit_info.pSignalSemaphores = &swapChainData.imageOwnershipSemaphores[currentFrame];

        if (vkQueueSubmit(renderDevice.queues[QFT_PRESENT], 1, &present_submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
            ASSERTF(true, "failed to submit command buffer to present queue!");
        }
    }

    // Presenting:
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = renderDevice.isPresentQueueSeprate ?
        &swapChainData.imageOwnershipSemaphores[currentFrame] :
        &swapChainData.drawCompleteSemaphores[currentFrame];

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &internal.swapChain;
    presentInfo.pImageIndices = &currentBuffer;

    VkResult result = vkQueuePresentKHR(renderDevice.queues[QFT_PRESENT], &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || internal.framebufferResized) {
        internal.framebufferResized = false;
        RecreateSwapChainInternal(renderDevice, internal);
    } else if (result != VK_SUCCESS) {
        ASSERTF(true, "Failed to present swap chain image!");
    }

    // vkQueueWaitIdle(renderDevice.queues[QFT_PRESENT]);
    internal.currentFrame = (currentFrame + 1) % SwapChainData::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::RenderContext::TransferBuffers(uint32 count, Internal::TransferBufferInfo* transferBufferInfo)
{
    VkCommandBuffer currentCmdBuff = GetCurrentFrameResource().transferCommandBuffer;
    internal.contextData.transferRequests = count;

    Buffer::CopyBuffers(currentCmdBuff, count, transferBufferInfo);
}

void Renderer::RenderContext::FlushTransfers(Internal::RenderDevice& renderDevice)
{
 
    Internal::ContextFrameResources& frameResources = GetCurrentFrameResource();
    Internal::SwapChainData& swapData = internal.swapChainData;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    if (internal.contextData.transferRequests) {
        if (renderDevice.isTransferQueueSeprate) {
            ExecuteTransferMemory(renderDevice.queues[Internal::QFT_TRANSFER], frameResources.transferCommandBuffer, 0,
                VK_NULL_HANDLE, VK_NULL_HANDLE, swapData.transferSyncFence, renderDevice.device);
        } else {
            ExecuteTransferMemory(renderDevice.queues[Internal::QFT_TRANSFER], frameResources.transferCommandBuffer, 0, VK_NULL_HANDLE, VK_NULL_HANDLE, swapData.transferSyncFence, renderDevice.device);
        }
    }

    internal.contextData.transferRequests = 0;
}

Renderer::Buffer Renderer::RenderContext::CreateBuffer(DeviceSize size, const void* data, uint32 usage,
    MemoryUsage memoryUsage, uint32 queueFamilies)
{
    Internal::RenderDevice& renderDevice = *internal.renderDevice;

    uint32 queueFamilyIndices[Internal::QFT_MAX];
    uint32 queueFamilyIndexCount = 0;
    VkSharingMode sharingMode = (VkSharingMode)(queueFamilies ? SharingMode::CONCURRENT : SharingMode::EXCLUSIVE);

    for (uint32 i = 0; i < Internal::QFT_MAX; i++) {
        if (Internal::QUEUE_FAMILY_FLAGS[i] & queueFamilies) {
            queueFamilyIndices[queueFamilyIndexCount++] = renderDevice.queueFamilyIndices.queueFamilies[i];
        }
    }

    Buffer buffer;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.flags = 0;

    if (bufferInfo.sharingMode == VK_SHARING_MODE_CONCURRENT) {
        bufferInfo.queueFamilyIndexCount = queueFamilyIndexCount;
        bufferInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    if (vkCreateBuffer(renderDevice.device, &bufferInfo, NULL, &buffer.apiBuffer) != VK_SUCCESS) {
        ASSERTF(true, "failed to create a buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(renderDevice.device, buffer.apiBuffer, &memRequirements);
    uint32 memoryTypeIndex = Buffer::FindMemoryTypeIndex(renderDevice, memRequirements.memoryTypeBits, memoryUsage);
    buffer.bufferMemory = gpuMemoryAllocator.Allocate(memoryTypeIndex, size, memRequirements.alignment);
    vkBindBufferMemory(renderDevice.device, buffer.apiBuffer, buffer.bufferMemory.memory, buffer.bufferMemory.offset);

    if (data && (memoryUsage == MemoryUsage::USAGE_CPU_ONLY || (memoryUsage == MemoryUsage::USAGE_CPU_TO_GPU))) {
        buffer.WriteToBuffer(size, data);
    }

    return buffer;
}

Renderer::Buffer Renderer::RenderContext::CreateStagingBuffer(DeviceSize size, const void* data)
{
    return this->CreateBuffer(size, data, BufferUsage::TRANSFER_SRC, MemoryUsage::USAGE_CPU_ONLY);
}

Renderer::RingBuffer Renderer::RenderContext::CreateRingBuffer(DeviceSize size, const void* data, uint32 usage, MemoryUsage memoryUsage, uint32 queueFamilies)
{
    RingBuffer ringBuffer;
    Buffer buffer = this->CreateBuffer(size * MAX_FRAMES, data, usage, memoryUsage, queueFamilies);
    
    ringBuffer.apiBuffer = buffer.apiBuffer;
    ringBuffer.bufferMemory = buffer.bufferMemory;
    ringBuffer.ring_size = MAX_FRAMES;
    ringBuffer.unit_size = size;
    ringBuffer.bufferIndex = 0;

    return ringBuffer;
}

const Renderer::Internal::ContextFrameResources& Renderer::RenderContext::GetFrameResource(uint32 i) const
{
    return internal.contextData.contextFrameResources[i];
}

Renderer::Internal::ContextFrameResources& Renderer::RenderContext::GetFrameResource(uint32 i)
{
    return internal.contextData.contextFrameResources[i];
}

const Renderer::Internal::ContextFrameResources& Renderer::RenderContext::GetCurrentFrameResource() const
{
    return internal.contextData.contextFrameResources[internal.currentImage];
}

Renderer::Internal::ContextFrameResources& Renderer::RenderContext::GetCurrentFrameResource()
{
    return internal.contextData.contextFrameResources[internal.currentImage];
}

const Renderer::Internal::SwapChainData& Renderer::RenderContext::GetSwapChainData() const
{
    return internal.swapChainData;
}

TRE_NS_END