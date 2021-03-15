#include "DescriptorSetAlloc.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

Renderer::DescriptorSetAllocator::DescriptorSetAllocator(RenderDevice* renderDevice, const DescriptorSetLayout& layout) : 
	renderDevice(renderDevice), descriptorSetLayout(layout), poolSizeCount(0), shouldBegin(false)
{
    // this->Init();
}

Renderer::DescriptorSetAllocator::DescriptorSetAllocator(const std::pair<RenderDevice*, const DescriptorSetLayout&>& renderDeviceLayoutPair) :
    renderDevice(renderDeviceLayoutPair.first), 
    descriptorSetLayout(renderDeviceLayoutPair.second), 
    poolSizeCount(0), shouldBegin(false)
{
}

void Renderer::DescriptorSetAllocator::Init()
{
    const VkDescriptorSetLayoutBinding* const bindings = descriptorSetLayout.GetDescriptorSetLayoutBindings() ;

    for (uint32 i = 0; i < descriptorSetLayout.GetBindingsCount(); i++) {
        poolSize[i] = VkDescriptorPoolSize { bindings[i].descriptorType, bindings[i].descriptorCount };
        poolSizeCount++;
    }

    this->AllocatePool();
}

void Renderer::DescriptorSetAllocator::AllocatePool()
{
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount  = poolSizeCount;
    poolInfo.pPoolSizes     = poolSize;
    poolInfo.maxSets        = MAX_SETS_PER_POOL;
    VkDescriptorPool pool;

    CALL_VK(vkCreateDescriptorPool(renderDevice->GetDevice(), &poolInfo, NULL, &pool));

    descriptorSetPools.emplace_back(pool);

    VkDescriptorSetLayout layouts[MAX_SETS_PER_POOL];
    VkDescriptorSet sets[MAX_SETS_PER_POOL];
    std::fill(std::begin(layouts), std::end(layouts), descriptorSetLayout.GetApiObject());

    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext              = NULL;
    allocInfo.descriptorPool     = descriptorSetPools.back();
    allocInfo.descriptorSetCount = MAX_SETS_PER_POOL;
    allocInfo.pSetLayouts        = layouts;

    CALL_VK(vkAllocateDescriptorSets(renderDevice->GetDevice(), &allocInfo, sets));

    for (auto set : sets) {
        descriptorCache.MakeEmpty(set);
    }
}

std::pair<VkDescriptorSet, bool> Renderer::DescriptorSetAllocator::Find(Hash hash)
{
    if (shouldBegin) {
        descriptorCache.BeginFrame();
        shouldBegin = false;
    }

    auto* node = descriptorCache.Request(hash);
    if (node) {
        return { node->set, true };
    }

    node = descriptorCache.RequestEmpty(hash);
    if (node) {
        return { node->set, false };
    }

    this->AllocatePool();
    return { descriptorCache.RequestEmpty(hash)->set, false };
}

void Renderer::DescriptorSetAllocator::Clear()
{
    descriptorCache.Clear();

    for (auto& pool : descriptorSetPools) {
        vkResetDescriptorPool(renderDevice->GetDevice(), pool, 0);
        vkDestroyDescriptorPool(renderDevice->GetDevice(), pool, NULL);
    }

    descriptorSetPools.clear();
}

void Renderer::DescriptorSetAllocator::BeginFrame()
{
    shouldBegin = true;
    descriptorCache.BeginFrame();
}

void Renderer::DescriptorSetAllocator::Destroy()
{
    if (renderDevice) {
        for (const auto& pool : descriptorSetPools) {
            vkDestroyDescriptorPool(renderDevice->GetDevice(), pool, NULL);
        }

        descriptorSetPools.clear();
    }
}

TRE_NS_END


