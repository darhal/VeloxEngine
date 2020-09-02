#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetLayout.hpp>

TRE_NS_START

namespace Renderer
{
	class PipelineLayout
	{
	public:
		PipelineLayout() : descriptorLayoutsCount(0), pushConstantsCount(0), pipelineLayout(VK_NULL_HANDLE) {}

		void Create(const Internal::RenderDevice& renderDevice)
		{
			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.pNext = NULL;
			pipelineLayoutInfo.flags = 0;
			pipelineLayoutInfo.setLayoutCount	= descriptorLayoutsCount;
			pipelineLayoutInfo.pSetLayouts		= descriptorLayouts;
			pipelineLayoutInfo.pushConstantRangeCount = pushConstantsCount;
			pipelineLayoutInfo.pPushConstantRanges = pushConstantsRanges;

			if (vkCreatePipelineLayout(renderDevice.device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
				ASSERTF(true, "failed to create pipeline layout!");
			}
		}

		VkPipelineLayout GetAPIObject() const
		{
			return pipelineLayout;
		}

		void AddDescriptorLayout(const DescriptorSetLayout& descSetLayout)
		{
			descriptorLayouts[descriptorLayoutsCount++] = descSetLayout.descriptorSetLayout;
		}

		void AddPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size)
		{
			pushConstantsRanges[pushConstantsCount++] = { stageFlags, offset, size };
		}
	private:
		VkDescriptorSetLayout descriptorLayouts[MAX_DESCRIPTOR_SET];
		uint32 descriptorLayoutsCount;

		VkPushConstantRange pushConstantsRanges[MAX_DESCRIPTOR_SET * 4];
		uint32 pushConstantsCount;

		VkPipelineLayout pipelineLayout;
	};
}

TRE_NS_END
