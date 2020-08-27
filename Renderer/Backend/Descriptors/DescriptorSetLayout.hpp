#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class DescriptorSetLayout
	{
	public:
		CONSTEXPR static uint32 MAX_BINDINGS = 64;

		DescriptorSetLayout() : bindingsCount(0), descriptorSetLayout(VK_NULL_HANDLE){}

		void AddBinding(uint32 binding, uint32 descriptorCount, DescriptorType descriptorType, ShaderStagesFlags shaderStages, const VkSampler* sampelr = NULL)
		{
			layoutBindings[bindingsCount].binding				= binding;
			layoutBindings[bindingsCount].descriptorType		= (VkDescriptorType)descriptorType;
			layoutBindings[bindingsCount].descriptorCount		= descriptorCount;
			layoutBindings[bindingsCount].stageFlags			= shaderStages;
			layoutBindings[bindingsCount].pImmutableSamplers	= sampelr;

			bindingsCount++;
		}

		VkDescriptorSetLayout Create(const Internal::RenderDevice& renderDevice)
		{
			ASSERTF(bindingsCount == 0, "Cant create descriptor set layout with 0 bindings");

			VkDescriptorSetLayoutCreateInfo  layoutInfo{};
			layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.pNext		= NULL;
			layoutInfo.flags		= 0;
			layoutInfo.bindingCount = bindingsCount;
			layoutInfo.pBindings	= layoutBindings;

			if (vkCreateDescriptorSetLayout(renderDevice.device, &layoutInfo, NULL, &descriptorSetLayout) != VK_SUCCESS) {
				ASSERTF(true, "Failed to create descriptor set layout!");
			}

			return descriptorSetLayout;
		}

		VkDescriptorSetLayout GetAPIObject() const { return descriptorSetLayout; }
	private:
		VkDescriptorSetLayoutBinding layoutBindings[MAX_BINDINGS];
		uint32 bindingsCount;

		VkDescriptorSetLayout descriptorSetLayout;

		friend class PipelineLayout;
	};
}

TRE_NS_END