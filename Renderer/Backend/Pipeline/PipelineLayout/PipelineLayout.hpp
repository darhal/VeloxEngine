#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetLayout.hpp>

TRE_NS_START

namespace Renderer
{
	/*struct ResourceLayout
	{
		DescriptorSetLayout descriptorSetLayouts[MAX_DESCRIPTOR_SET];
		VkPushConstantRange pushConstantsRanges[MAX_DESCRIPTOR_SET * 4];
		
		uint32 descriptorSetLayoutsCount;
		uint32 pushConstantsCount;
	};*/

	class DescriptorSetAllocator;
	class RenderBackend;

	class PipelineLayout
	{
	public:
		PipelineLayout() : descriptorSetLayoutCount(0), pushConstantsCount(0), pipelineLayout(VK_NULL_HANDLE) {}

		void Create(RenderBackend& backend);

		VkPipelineLayout GetAPIObject() const
		{
			return pipelineLayout;
		}

		void AddDescriptorSetLayout(uint32 binding, uint32 descriptorCount, DescriptorType descriptorType, ShaderStagesFlags shaderStages, const VkSampler* sampler = NULL)
		{
			descriptorSetLayouts[descriptorSetLayoutCount++].AddBinding(binding, descriptorCount, descriptorType, shaderStages, sampler);
		}

		void AddPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size)
		{
			pushConstantsRanges[pushConstantsCount++] = { stageFlags, offset, size };
		}

		const DescriptorSetLayout& GetDescriptorSetLayout(uint32 set) const { return descriptorSetLayouts[set]; }

		DescriptorSetAllocator* GetAllocator(uint32 set) const { return descriptorSetAlloc[set]; }
	private:
		DescriptorSetAllocator* descriptorSetAlloc[MAX_DESCRIPTOR_SET];

		DescriptorSetLayout descriptorSetLayouts[MAX_DESCRIPTOR_SET];
		VkPushConstantRange pushConstantsRanges[MAX_DESCRIPTOR_SET * 4];

		VkPipelineLayout pipelineLayout;
		
		uint32 descriptorSetLayoutCount;
		uint32 pushConstantsCount;

		// TODO: this need to know aeverything about the layout, not just VK objects
	};
}

TRE_NS_END
