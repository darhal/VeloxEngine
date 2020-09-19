#include "RenderBackend.hpp"
#include <Renderer/Backend/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/SwapChain/SwapChain.hpp>
#include <Renderer/Backend/Images/Image.hpp>

TRE_NS_START

Renderer::RenderBackend::RenderBackend(TRE::Window* wnd) : 
    stagingManager{ &renderDevice.internal }, 
    renderContext(&renderDevice),
    framebufferAllocator(&renderDevice)
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

Renderer::ImageHandle Renderer::RenderBackend::CreateImage(const ImageCreateInfo& createInfo, const void* data)
{
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
        info.queueFamilyIndexCount = (uint32)queueFamilyIndices.GetElementCount();
        info.pQueueFamilyIndices = queueFamilyIndices.GetData();
    }

    VkImage apiImage;
    if (vkCreateImage(renderDevice.GetDevice(), &info, NULL, &apiImage) != VK_SUCCESS) {
        ASSERTF(true, "failed to create a image!");
    }
    
    MemoryView imageMemory;
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(renderDevice.GetDevice(), apiImage, &memRequirements);
    uint32 memoryTypeIndex = Buffer::FindMemoryTypeIndex(renderDevice.internal, memRequirements.memoryTypeBits, memUsage);
    imageMemory = gpuMemoryAllocator.Allocate(memoryTypeIndex, memRequirements.size, memRequirements.alignment);
    vkBindImageMemory(renderDevice.GetDevice(), apiImage, imageMemory.memory, imageMemory.offset);

    ImageHandle ret = ImageHandle(objectsPool.images.Allocate(apiImage, createInfo, imageMemory));

    if (data) {
        if (memUsage == MemoryUsage::GPU_ONLY) {
            stagingManager.Stage(*ret, data, createInfo.width * createInfo.height * FormatToChannelCount(createInfo.format));
        } else {
            // TODO: add uploading directly from GPU
            ASSERTF(true, "Not supported!");
        }
    } else {
        // TODO: add layout trasnisioning here: using staging manager:
    }

    return ret;
}

Renderer::ImageViewHandle Renderer::RenderBackend::CreateImageView(const ImageViewCreateInfo& createInfo)
{
    ImageViewCreateInfo info = createInfo;
    const auto& imageCreateInfo = createInfo.image->GetInfo();

    if (createInfo.format == VK_FORMAT_UNDEFINED) {
        info.format = imageCreateInfo.format;
    }

    if (createInfo.viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM) {
        info.viewType = GetImageViewType(imageCreateInfo, &createInfo);
    }

    if (createInfo.levels == VK_REMAINING_MIP_LEVELS) {
        info.levels = imageCreateInfo.levels - createInfo.baseLevel;
    }

    if (createInfo.layers == VK_REMAINING_ARRAY_LAYERS) {
        info.layers = imageCreateInfo.layers - createInfo.baseLayer;
    }

    VkImageViewCreateInfo viewInfo;
    viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext      = NULL;
    viewInfo.flags      = 0;
    viewInfo.image      = info.image->GetAPIObject();
    viewInfo.viewType   = info.viewType;
    viewInfo.format     = info.format;
    viewInfo.components = info.swizzle;

    viewInfo.subresourceRange = { 
        FormatToAspectMask(viewInfo.format), 
        info.baseLevel, info.levels, 
        info.baseLayer, info.layers
    };

    VkImageView apiImageView;
    vkCreateImageView(renderDevice.GetDevice(), &viewInfo, NULL, &apiImageView);

    ImageViewHandle ret(objectsPool.imageViews.Allocate(apiImageView, info));
    return ret;
}

bool Renderer::RenderBackend::CreateBufferInternal(VkBuffer& outBuffer, MemoryView& outMemoryView, const BufferInfo& createInfo)
{
    Internal::RenderDevice& renderDevice = this->renderDevice.internal;
    StackAlloc<uint32, Internal::QFT_MAX> queueFamilyIndices;
    VkSharingMode sharingMode = (VkSharingMode)(createInfo.queueFamilies ? SharingMode::CONCURRENT : SharingMode::EXCLUSIVE);

    for (uint32 i = 0; i < Internal::QFT_MAX; i++) {
        if (Internal::QUEUE_FAMILY_FLAGS[i] & createInfo.queueFamilies) {
            queueFamilyIndices.AllocateInit(1, renderDevice.queueFamilyIndices.queueFamilies[i]);
        }
    }
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = createInfo.size;
    bufferInfo.usage = createInfo.usage;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.flags = 0;

    if (bufferInfo.sharingMode == VK_SHARING_MODE_CONCURRENT) {
        bufferInfo.queueFamilyIndexCount = (uint32)queueFamilyIndices.GetElementCount();
        bufferInfo.pQueueFamilyIndices = queueFamilyIndices.GetData();
    }

    if (vkCreateBuffer(renderDevice.device, &bufferInfo, NULL, &outBuffer) != VK_SUCCESS) {
        ASSERTF(true, "failed to create a buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(renderDevice.device, outBuffer, &memRequirements);
    uint32 memoryTypeIndex = Buffer::FindMemoryTypeIndex(renderDevice, memRequirements.memoryTypeBits, createInfo.domain);
    outMemoryView = gpuMemoryAllocator.Allocate(memoryTypeIndex, createInfo.size, memRequirements.alignment);
    vkBindBufferMemory(renderDevice.device, outBuffer, outMemoryView.memory, outMemoryView.offset);

    return true;
}

Renderer::BufferHandle Renderer::RenderBackend::CreateBuffer(const BufferInfo& createInfo, const void* data)
{
    MemoryView bufferMemory;
    VkBuffer apiBuffer;

    this->CreateBufferInternal(apiBuffer, bufferMemory, createInfo);
    BufferHandle ret(objectsPool.buffers.Allocate(apiBuffer, createInfo, bufferMemory));

    if (data) {
        if (createInfo.domain == MemoryUsage::CPU_ONLY || createInfo.domain == MemoryUsage::CPU_CACHED || createInfo.domain == MemoryUsage::CPU_COHERENT) {
            ret->WriteToBuffer(createInfo.size, data);
        } else {
            stagingManager.Stage(ret->apiBuffer, data, createInfo.size, bufferMemory.alignment);
        }
    }

    return ret;
}

Renderer::RingBufferHandle Renderer::RenderBackend::CreateRingBuffer(const BufferInfo& createInfo, const void* data)
{
    BufferInfo info = createInfo;
    const DeviceSize alignment = this->renderDevice.internal.gpuProperties.limits.minUniformBufferOffsetAlignment;
    const DeviceSize padding = (alignment - (info.size % alignment)) % alignment;
    const DeviceSize alignedSize = info.size + padding;
    info.size = alignedSize * NUM_FRAMES - padding;

    MemoryView bufferMemory;
    VkBuffer apiBuffer;

    // Removing padding from total size, as we dont need the last bytes for alignement
    // alignedSize * NUM_FRAMES - padding, data, usage, memoryUsage, queueFamilies
    this->CreateBufferInternal(apiBuffer, bufferMemory, info);
    RingBufferHandle ret(objectsPool.ringBuffers.Allocate(apiBuffer, info, bufferMemory, (uint32)alignedSize, NUM_FRAMES));

    if (data) {
        if (info.domain == MemoryUsage::CPU_ONLY || info.domain == MemoryUsage::CPU_CACHED || info.domain == MemoryUsage::CPU_COHERENT) {
            ret->WriteToBuffer(createInfo.size, data);
        } else {
            stagingManager.Stage(ret->apiBuffer, data, createInfo.size, bufferMemory.alignment);
        }
    }

    return ret;
}

Renderer::SamplerHandle Renderer::RenderBackend::CreateSampler(const SamplerInfo& createInfo)
{
    VkSampler sampler;
    VkSamplerCreateInfo info;
    SamplerInfo::FillVkSamplerCreateInfo(createInfo, info);
    vkCreateSampler(renderDevice.GetDevice(), &info, NULL, &sampler);
    SamplerHandle ret(objectsPool.samplers.Allocate(sampler, createInfo));
    return ret;
}

Renderer::CommandBufferHandle Renderer::RenderBackend::RequestCommandBuffer(QueueTypes type)
{
    PerFrame& frame = Frame();
    VkCommandBuffer buffer = frame.commandPools[0][(uint32)type].RequestCommandBuffer();
    CommandBufferHandle handle(objectsPool.commandBuffers.Allocate(this, buffer, CommandBuffer::Type::GENERIC));
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

const Renderer::RenderPass& Renderer::RenderBackend::RequestRenderPass(const RenderPassInfo& info, bool compatible)
{
    Hasher h;
    VkFormat formats[MAX_ATTACHMENTS];
    VkFormat depthStencilFormat;
    uint32 lazy = 0;
    uint32 optimal = 0;

    for (uint32 i = 0; i < info.colorAttachmentCount; i++) {
        ASSERT(!info.colorAttachments[i]);

        formats[i] = info.colorAttachments[i]->GetInfo().format;

        if (info.colorAttachments[i]->GetImage()->GetInfo().domain == ImageDomain::TRANSIENT) {
            lazy |= 1u << i;
        }

        if (info.colorAttachments[i]->GetImage()->GetInfo().layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            optimal |= 1u << i;
        }
    }

    if (info.depthStencil) {
        if (info.depthStencil->GetImage()->GetInfo().domain == ImageDomain::TRANSIENT)
            lazy |= 1u << info.colorAttachmentCount;
        if (info.depthStencil->GetImage()->GetInfo().layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            optimal |= 1u << info.colorAttachmentCount;
    }

    h.u32(info.baseLayer);
    h.u32(info.layersCount);
    h.u32(info.subpassesCount);

    for (unsigned i = 0; i < info.subpassesCount; i++) {
        h.u32(info.subpasses[i].colorAttachmentsCount);
        h.u32(info.subpasses[i].inputAttachmentsCount);
        h.u32(info.subpasses[i].resolveAttachmentsCount);
        h.u32(static_cast<uint32_t>(info.subpasses[i].depth_stencil_mode));

        for (unsigned j = 0; j < info.subpasses[i].colorAttachmentsCount; j++)
            h.u32(info.subpasses[i].colorAttachments[j]);
        for (unsigned j = 0; j < info.subpasses[i].inputAttachmentsCount; j++)
            h.u32(info.subpasses[i].inputAttachments[j]);
        for (unsigned j = 0; j < info.subpasses[i].resolveAttachmentsCount; j++)
            h.u32(info.subpasses[i].resolveAttachments[j]);
    }

    depthStencilFormat = info.depthStencil ? info.depthStencil->GetInfo().format : VK_FORMAT_UNDEFINED;
    h.Data(formats, info.colorAttachmentCount * sizeof(VkFormat));
    h.u32(info.colorAttachmentCount);
    h.u32(depthStencilFormat);

    // Compatible render passes do not care about load/store, or image layouts.
    if (!compatible) {
        h.u32(info.opFlags);
        h.u32(info.clearAttachments);
        h.u32(info.loadAttachments);
        h.u32(info.storeAttachments);
        h.u32(optimal);
    }

    // Lazy flag can change external subpass dependencies, which is not compatible.
    h.u32(lazy);

    Hash hash = h.Get();
    auto rp = renderPasses.find(hash);

    if (rp != renderPasses.end()) {
        return rp->second;
    }

    auto rp2 = renderPasses.emplace(hash, RenderPass(renderDevice, info));
    rp->second.hash = h.Get();
    return rp2.first->second;
}

const Renderer::Framebuffer& Renderer::RenderBackend::RequestFramebuffer(const RenderPassInfo& info)
{
    const RenderPass& rp = this->RequestRenderPass(info);
    return framebufferAllocator.RequestFramebuffer(rp, info);
}

TRE_NS_END