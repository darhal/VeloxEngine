#include "PipelineLayout.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::PipelineLayout::Create(RenderDevice& device)
{
	StackAlloc<VkDescriptorSetLayout, MAX_DESCRIPTOR_SET> vkDescSetAlloc;
	VkDescriptorSetLayout* layouts = vkDescSetAlloc.Allocate(descriptorSetLayoutCount);

	for (uint32 i = 0; i < descriptorSetLayoutCount; i++) {
        layouts[i] = descriptorSetLayouts[i].Create(device.GetDevice());
        descriptorSetAlloc[i] = device.RequestDescriptorSetAllocator(descriptorSetLayouts[i]);
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = NULL;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount = descriptorSetLayoutCount;
	pipelineLayoutInfo.pSetLayouts = layouts;
	pipelineLayoutInfo.pushConstantRangeCount = pushConstantsCount;
	pipelineLayoutInfo.pPushConstantRanges = pushConstantsRanges;

    if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
		ASSERTF(true, "failed to create pipeline layout!");
	}
}

void Renderer::PipelineLayout::Destroy(const Renderer::RenderDevice& renderDevice)
{
    if (pipelineLayout) {
        vkDestroyPipelineLayout(renderDevice.GetDevice(), pipelineLayout, NULL);
        pipelineLayout = VK_NULL_HANDLE;
    }

    if (descriptorSetLayoutCount) {
        for (uint32 i = 0; i < descriptorSetLayoutCount; i++) {
            descriptorSetLayouts[i].Destroy(renderDevice.GetDevice());
            descriptorSetAlloc[i]->Destroy();
        }

        descriptorSetLayoutCount = 0;
    }

    pushConstantsCount = 0;

}

TRE_NS_END
