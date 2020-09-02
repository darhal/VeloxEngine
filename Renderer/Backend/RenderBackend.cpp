#include "RenderBackend.hpp"
#include <Renderer/Backend/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/SwapChain/SwapChain.hpp>

TRE_NS_START

Renderer::RenderBackend::RenderBackend(TRE::Window* wnd) : stagingManager{ &renderDevice.internal }, renderContext(&renderDevice)
{
    renderDevice.internal.renderContext = &renderContext.internal;
    renderContext.internal.renderDevice = &renderDevice.internal;

    renderInstance.CreateRenderInstance();

    renderContext.CreateRenderContext(wnd, renderInstance.internal);
    renderDevice.CreateRenderDevice(renderInstance.internal, renderContext.internal);
    renderContext.InitRenderContext(renderInstance.internal, renderDevice.internal);

    stagingManager.Init();
    gpuMemoryAllocator.Init(renderDevice.internal);
}

Renderer::RenderBackend::~RenderBackend()
{
    vkDeviceWaitIdle(renderDevice.internal.device);
    
    renderContext.DestroyRenderContext(renderInstance.internal, renderDevice.internal, renderContext.internal);

    stagingManager.Shutdown();
    renderDevice.DestroryRenderDevice();
    renderInstance.DestroyRenderInstance();
}

void Renderer::RenderBackend::BeginFrame()
{
    renderContext.BeginFrame(renderDevice.internal, stagingManager);
}

void Renderer::RenderBackend::EndFrame()
{
    renderContext.EndFrame(renderDevice.internal);
}

Renderer::Buffer Renderer::RenderBackend::CreateBuffer(DeviceSize size, const void* data, uint32 usage,
    MemoryUsage memoryUsage, uint32 queueFamilies)
{
    Internal::RenderDevice& renderDevice = this->renderDevice.internal;

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

    if (data && (memoryUsage == MemoryUsage::CPU_ONLY || memoryUsage == MemoryUsage::CPU_CACHED || memoryUsage == MemoryUsage::CPU_COHERENT)) {
        buffer.WriteToBuffer(size, data);
    }

    return buffer;
}

Renderer::Buffer Renderer::RenderBackend::CreateStagingBuffer(DeviceSize size, const void* data)
{
    return this->CreateBuffer(size, data, BufferUsage::TRANSFER_SRC, MemoryUsage::CPU_ONLY);
}

Renderer::RingBuffer Renderer::RenderBackend::CreateRingBuffer(DeviceSize size, const void* data, uint32 usage, MemoryUsage memoryUsage, uint32 queueFamilies)
{
    const DeviceSize alignment = this->renderDevice.internal.gpuProperties.limits.minUniformBufferOffsetAlignment;
    const DeviceSize padding = (alignment - (size % alignment)) % alignment;
    const DeviceSize alignedSize = size + padding;

    // Removing padding from total size, as we dont need the last bytes for alignement
    Buffer buffer = this->CreateBuffer(alignedSize * MAX_FRAMES - padding, data, usage, memoryUsage, queueFamilies);

    RingBuffer ringBuffer;
    ringBuffer.apiBuffer = buffer.apiBuffer;
    ringBuffer.bufferMemory = buffer.bufferMemory;
    ringBuffer.ring_size = MAX_FRAMES;
    ringBuffer.unit_size = alignedSize;
    ringBuffer.bufferIndex = 0;

    return ringBuffer;
}

TRE_NS_END