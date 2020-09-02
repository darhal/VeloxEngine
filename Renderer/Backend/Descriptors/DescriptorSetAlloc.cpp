#include "DescriptorSetAlloc.hpp"

TRE_NS_START

Renderer::DescriptorSetAllocator::DescriptorSetAllocator(Internal::RenderDevice* renderDevice, const DescriptorSetLayout& layout) : 
	renderDevice(renderDevice), descriptorSetLayout(layout), poolSizeCount(0)
{
}

void Renderer::DescriptorSetAllocator::Init()
{
    const VkDescriptorSetLayoutBinding* const bindings = descriptorSetLayout.GetDescriptorSetLayoutBindings() ;

    for (uint32 i = 0; i < descriptorSetLayout.GetBindingsCount(); i++) {
        poolSize[i] = VkDescriptorPoolSize{ bindings[i].descriptorType, bindings[i].descriptorCount };
        poolSizeCount++;
    }

    this->AllocatePool();
}

void Renderer::DescriptorSetAllocator::AllocatePool()
{
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizeCount;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = MAX_SETS_PER_POOL;
    VkDescriptorPool pool;

    if (vkCreateDescriptorPool(renderDevice->device, &poolInfo, NULL, &pool) != VK_SUCCESS) {
        ASSERTF(true, "failed to create descriptor pool!");
    }

    descriptorSetPools.emplace_back(pool);

    VkDescriptorSetLayout layouts[MAX_SETS_PER_POOL];
    std::fill(std::begin(layouts), std::end(layouts), descriptorSetLayout.GetAPIObject());

    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.descriptorPool     = descriptorSetPools.back();
    allocInfo.descriptorSetCount = MAX_SETS_PER_POOL;
    allocInfo.pSetLayouts        = layouts;

    emptyDescriptors.resize(MAX_SETS_PER_POOL);

    if (vkAllocateDescriptorSets(renderDevice->device, &allocInfo, emptyDescriptors.data()) != VK_SUCCESS) {
        ASSERTF(true, "failed to allocate descriptor sets!");
    }
}

VkDescriptorSet Renderer::DescriptorSetAllocator::Allocate()
{
    if (emptyDescriptors.size() == 0) {
        this->AllocatePool();
    }

    VkDescriptorSet descriptorSet = emptyDescriptors.back();
    emptyDescriptors.pop_back();
    return descriptorSet;
}

void Renderer::DescriptorSetAllocator::Free(VkDescriptorSet set)
{
    emptyDescriptors.push_back(set);
}

TRE_NS_END


