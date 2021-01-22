#include "PipelineLayout.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

void Renderer::PipelineLayout::Create(RenderBackend& backend)
{
	const RenderDevice& renderDevice = backend.GetRenderDevice();

	StackAlloc<VkDescriptorSetLayout, MAX_DESCRIPTOR_SET> vkDescSetAlloc;
	VkDescriptorSetLayout* layouts = vkDescSetAlloc.Allocate(descriptorSetLayoutCount);

	for (uint32 i = 0; i < descriptorSetLayoutCount; i++) {
		layouts[i] = descriptorSetLayouts[i].Create(renderDevice.GetDevice());
		descriptorSetAlloc[i] = backend.RequestDescriptorSetAllocator(descriptorSetLayouts[i]);
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = NULL;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount = descriptorSetLayoutCount;
	pipelineLayoutInfo.pSetLayouts = layouts;
	pipelineLayoutInfo.pushConstantRangeCount = pushConstantsCount;
	pipelineLayoutInfo.pPushConstantRanges = pushConstantsRanges;

	if (vkCreatePipelineLayout(renderDevice.GetDevice(), &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
		ASSERTF(true, "failed to create pipeline layout!");
	}
}

TRE_NS_END