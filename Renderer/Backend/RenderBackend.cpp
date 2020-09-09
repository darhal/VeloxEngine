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

    this->Init();
}

void Renderer::RenderBackend::Init()
{
    for (uint32 f = 0; f < renderContext.GetNumFrames(); f++) {
        for (uint32 t = 0; t < MAX_THREADS; t++) {
            for (uint32 i = 0; i < (uint32)QueueTypes::MAX; i++) {
                const Internal::QueueFamilyIndices& queueFamilyIndices = renderDevice.GetQueueFamilyIndices();
                PerFrame& frame = perFrame[f];
                frame.commandPools[t][i] = std::move(CommandPool(&renderDevice, queueFamilyIndices.queueFamilies[i]));
            }
        }
    }
}

void Renderer::RenderBackend::ClearFrame()
{
    PerFrame& frame = Frame();

    for (uint32 i = 0; i < (uint32)QueueTypes::MAX; i++) {
        frame.commandPools[0][i].Reset();
        frame.submissions[i].Reset();
    }

    // objectsPool.commandBuffers.Clear();
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
    renderContext.BeginFrame(renderDevice, stagingManager);
    this->ClearFrame();
}

void Renderer::RenderBackend::EndFrame()
{
    const PerFrame& frame = this->Frame();
    const auto& submissions = frame.submissions[(uint32)QueueTypes::GRAPHICS_ONLY];
    renderContext.EndFrame(renderDevice, submissions.GetData(), (uint32)submissions.GetElementCount());
}

Renderer::Image Renderer::RenderBackend::CreateImage(const ImageCreateInfo& createInfo)
{
    Image image;
    MemoryUsage memUsage = MemoryUsage::USAGE_UNKNOWN;

    VkImageCreateInfo info;
    info.sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext       = NULL;
    info.flags       = createInfo.flags;
    info.imageType   = createInfo.type;
    info.format      = createInfo.format;
    info.extent      = { createInfo.width, createInfo.height, createInfo.depth };
    info.mipLevels   = createInfo.levels;
    info.arrayLayers = createInfo.layers;
    info.samples     = createInfo.samples;
    info.usage       = createInfo.usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    switch(createInfo.domain) {
    case ImageDomain::PHYSICAL:
        memUsage = MemoryUsage::GPU_ONLY;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        break;
    case ImageDomain::TRANSIENT:
        memUsage = MemoryUsage::GPU_ONLY;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        break;
    case ImageDomain::LINEAR_HOST:
        memUsage = MemoryUsage::CPU_ONLY;
        info.tiling = VK_IMAGE_TILING_LINEAR;
        info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        break;
    case ImageDomain::LINEAR_HOST_CACHED:
        memUsage = MemoryUsage::CPU_CACHED;
        info.tiling = VK_IMAGE_TILING_LINEAR;
        info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        break;
    }

    if (info.mipLevels == 0) {
        info.mipLevels = 1; // TODO: complete
    }

    StackAlloc<uint32, Internal::QFT_MAX> queueFamilyIndices;
    info.sharingMode = (VkSharingMode)(createInfo.queueFamilies ? SharingMode::CONCURRENT : SharingMode::EXCLUSIVE);

    for (uint32 i = 0; i < Internal::QFT_MAX; i++) {
        if (Internal::QUEUE_FAMILY_FLAGS[i] & createInfo.queueFamilies) {
            queueFamilyIndices.AllocateInit(1, renderDevice.GetQueueFamilyIndices().queueFamilies[i]);
        }
    }

    if (info.sharingMode == VK_SHARING_MODE_CONCURRENT) {
        info.queueFamilyIndexCount = queueFamilyIndices.GetElementCount();
        info.pQueueFamilyIndices = queueFamilyIndices.GetData();
    }

    if (vkCreateImage(renderDevice.GetDevice(), &info, NULL, &image.apiImage) != VK_SUCCESS) {
        ASSERTF(true, "failed to create a image!");
    }
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(renderDevice.GetDevice(), image.apiImage, &memRequirements);
    uint32 memoryTypeIndex = Buffer::FindMemoryTypeIndex(renderDevice.internal, memRequirements.memoryTypeBits, memUsage);
    image.imageMemory = gpuMemoryAllocator.Allocate(memoryTypeIndex, memRequirements.size, memRequirements.alignment);
    vkBindImageMemory(renderDevice.GetDevice(), image.apiImage, image.imageMemory.memory, image.imageMemory.offset);

    return image;
}

Renderer::Buffer Renderer::RenderBackend::CreateBuffer(DeviceSize size, const void* data, uint32 usage,
    MemoryUsage memoryUsage, uint32 queueFamilies)
{
    Internal::RenderDevice& renderDevice = this->renderDevice.internal;
    StackAlloc<uint32, Internal::QFT_MAX> queueFamilyIndices;
    VkSharingMode sharingMode = (VkSharingMode)(queueFamilies ? SharingMode::CONCURRENT : SharingMode::EXCLUSIVE);

    for (uint32 i = 0; i < Internal::QFT_MAX; i++) {
        if (Internal::QUEUE_FAMILY_FLAGS[i] & queueFamilies) {
            queueFamilyIndices.AllocateInit(1, renderDevice.queueFamilyIndices.queueFamilies[i]);
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
        bufferInfo.queueFamilyIndexCount = queueFamilyIndices.GetElementCount();
        bufferInfo.pQueueFamilyIndices = queueFamilyIndices.GetData();
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
    Buffer buffer = this->CreateBuffer(alignedSize * NUM_FRAMES - padding, data, usage, memoryUsage, queueFamilies);

    RingBuffer ringBuffer;
    ringBuffer.apiBuffer = buffer.apiBuffer;
    ringBuffer.bufferMemory = buffer.bufferMemory;
    ringBuffer.ring_size = NUM_FRAMES;
    ringBuffer.unit_size = alignedSize;
    ringBuffer.bufferIndex = 0;

    return ringBuffer;
}

Renderer::CommandBufferHandle Renderer::RenderBackend::RequestCommandBuffer(QueueTypes type)
{
    PerFrame& frame = Frame();
    VkCommandBuffer buffer = frame.commandPools[0][(uint32)type].RequestCommandBuffer();
    CommandBufferHandle handle(objectsPool.commandBuffers.Allocate(&renderContext, buffer, CommandBuffer::Type::GENERIC));
    return handle;
}

Renderer::DescriptorSetAllocator* Renderer::RenderBackend::RequestDescriptorSetAllocator(const DescriptorSetLayout& layout)
{
    Hasher h;
    h.Data(reinterpret_cast<const uint32*>(layout.GetDescriptorSetLayoutBindings()), sizeof(VkDescriptorSetLayoutBinding) * layout.GetBindingsCount());

    // For the weird return value check: https://en.cppreference.com/w/cpp/container/unordered_map/emplace
    const auto ctor_arg = std::pair<RenderDevice*, const DescriptorSetLayout&>(&renderDevice, layout);
    const auto& res = descriptorSetAllocators.emplace(h.Get(), ctor_arg);

    if (res.second) {
        res.first->second.Init();
    }

    return &res.first->second;
}

void Renderer::RenderBackend::Submit(CommandBufferHandle cmd)
{
    PerFrame& frame = Frame();
    VkCommandBuffer* allocCmd = frame.submissions[(uint32)QueueTypes::GRAPHICS_ONLY].Allocate(1);
    *allocCmd = cmd->GetAPIObject();
}

void Renderer::RenderBackend::CreateShaderProgram(const std::initializer_list<ShaderProgram::ShaderStage>& shaderStages, ShaderProgram* shaderProgramOut)
{
    shaderProgramOut->Create(*this, shaderStages);
}


TRE_NS_END