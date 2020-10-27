#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	// TODO: continue this tommrow..
	/*struct DescriptorSetLayout
	{
		uint32 sampled_image_mask = 0;
		uint32 storage_image_mask = 0;
		uint32 uniform_buffer_mask = 0;
		uint32 storage_buffer_mask = 0;
		uint32 sampled_buffer_mask = 0;
		uint32 input_attachment_mask = 0;
		uint32 sampler_mask = 0;
		uint32 separate_image_mask = 0;
		uint32 fp_mask = 0;
		uint32 immutable_sampler_mask = 0;
		uint64 immutable_samplers = 0;

		uint32 type_mask[10];
		uint8 array_size[MAX_DESCRIPTOR_BINDINGS] = {};
		enum { UNSIZED_ARRAY = 0xff };
	};*/

	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout() : bindingsCount(0), descriptorSetLayout(VK_NULL_HANDLE) {}

		void AddBinding(uint32 binding, uint32 descriptorCount, DescriptorType descriptorType, ShaderStagesFlags shaderStages, const VkSampler* sampler = NULL)
		{
			ASSERT(binding + descriptorCount >= MAX_DESCRIPTOR_BINDINGS);

			layoutBindings[bindingsCount].binding				= binding;
			layoutBindings[bindingsCount].descriptorType		= (VkDescriptorType)descriptorType;
			layoutBindings[bindingsCount].descriptorCount		= descriptorCount;
			layoutBindings[bindingsCount].stageFlags			= shaderStages;
			layoutBindings[bindingsCount].pImmutableSamplers	= sampler;

			bindingsCount++;
		}

		VkDescriptorSetLayout Create(const VkDevice& vkDevice)
		{
			ASSERTF(bindingsCount == 0, "Cant create descriptor set layout with 0 bindings");

			VkDescriptorSetLayoutCreateInfo  layoutInfo;
			layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.pNext		= NULL;
			layoutInfo.flags		= 0;
			layoutInfo.bindingCount = bindingsCount;
			layoutInfo.pBindings	= layoutBindings;

			if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, NULL, &descriptorSetLayout) != VK_SUCCESS) {
				ASSERTF(true, "Failed to create descriptor set layout!");
			}

			return descriptorSetLayout;
		}

		VkDescriptorSetLayout GetAPIObject() const { return descriptorSetLayout; }

		const VkDescriptorSetLayoutBinding* const GetDescriptorSetLayoutBindings() const { return layoutBindings; }

		const VkDescriptorSetLayoutBinding& GetDescriptorSetLayoutBinding(uint32 i) const { return layoutBindings[i]; }

		uint32 GetBindingsCount() const { return bindingsCount; }
	private:
		VkDescriptorSetLayoutBinding layoutBindings[MAX_DESCRIPTOR_BINDINGS];
		VkDescriptorSetLayout descriptorSetLayout;
		uint32 bindingsCount;

		friend class PipelineLayout;
	};
}

TRE_NS_END