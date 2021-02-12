#pragma once

#include <unordered_map>

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetLayout.hpp>

TRE_NS_START

namespace Renderer
{
	class DescriptorSetAllocator;
	class RenderDevice;

	class PipelineLayout
	{
	public:
		PipelineLayout() : descriptorSetLayoutCount(0), pushConstantsCount(0), pipelineLayout(VK_NULL_HANDLE) {}

        void Create(RenderDevice& device);

        void Destroy(const RenderDevice& renderDevice);

		VkPipelineLayout GetApiObject() const
		{
			return pipelineLayout;
		}

        void AddBindingToSet(uint32 set, uint32 binding, uint32 descriptorCount, DescriptorType descriptorType,
                             ShaderStagesFlags shaderStages, const VkSampler* sampler = NULL)
		{
			ASSERT(set >= MAX_DESCRIPTOR_SET);
			descriptorSetLayouts[set].AddBinding(binding, descriptorCount, descriptorType, shaderStages, sampler);
		}

        void AddBindingToSet(uint32 binding, uint32 descriptorCount, DescriptorType descriptorType,
                             ShaderStagesFlags shaderStages, const VkSampler* sampler = NULL)
		{
			ASSERT(descriptorSetLayoutCount <= 0 || descriptorSetLayoutCount >= MAX_DESCRIPTOR_SET);
			this->AddBindingToSet(descriptorSetLayoutCount - 1, binding, descriptorCount, descriptorType, shaderStages, sampler);
		}

		uint32 AddDescriptorSetLayout()
		{
			return descriptorSetLayoutCount++;
		}

		void AddPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size)
		{
			pushConstants.emplace(std::make_pair(stageFlags, pushConstantsCount));
			pushConstantsRanges[pushConstantsCount++] = { stageFlags, offset, size };
		}

		const DescriptorSetLayout& GetDescriptorSetLayout(uint32 set) const { return descriptorSetLayouts[set]; }

		const VkPushConstantRange& GetPushConstantRange(uint32 i) const { return pushConstantsRanges[i]; };

		const VkPushConstantRange& GetPushConstantRangeFromStage(VkShaderStageFlags stage) const { 
			return pushConstantsRanges[pushConstants.at(stage)]; 
		};

		DescriptorSetAllocator* GetAllocator(uint32 set) const { return descriptorSetAlloc[set]; }

		Hash GetHash()
		{
			Hasher h;
			h.u32(descriptorSetLayoutCount);
			for (uint32 i = 0; i < descriptorSetLayoutCount; i++)
				h.u64(descriptorSetLayouts[i].GetHash());

			h.u32(pushConstantsCount);
			h.Data(reinterpret_cast<const uint32*>(pushConstantsRanges), sizeof(VkPushConstantRange) * pushConstantsCount / 4);
			return h.Get();
		}
	private:
		DescriptorSetAllocator* descriptorSetAlloc[MAX_DESCRIPTOR_SET];

		DescriptorSetLayout descriptorSetLayouts[MAX_DESCRIPTOR_SET];
		VkPushConstantRange pushConstantsRanges[MAX_DESCRIPTOR_SET * 4];

		std::unordered_map<VkShaderStageFlags, uint32> pushConstants;

		VkPipelineLayout pipelineLayout;
		
		uint32 descriptorSetLayoutCount;
		uint32 pushConstantsCount;
	};
}

TRE_NS_END
