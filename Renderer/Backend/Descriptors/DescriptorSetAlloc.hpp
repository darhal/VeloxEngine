#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetLayout.hpp>

TRE_NS_START

namespace Renderer
{
	class DescriptorSetAllocator
	{
	public:
		CONSTEXPR static uint32 MAX_DESCRIPTOR_TYPES = 11;
		CONSTEXPR static uint32 MAX_SETS_PER_POOL = 16;
	public:
		DescriptorSetAllocator(Internal::RenderDevice* renderDevice, const DescriptorSetLayout& layout);

		void Init();

		VkDescriptorSet Allocate();

		void Free(VkDescriptorSet set);
	private:
		void AllocatePool();
	private:
		Internal::RenderDevice* renderDevice;
		const DescriptorSetLayout& descriptorSetLayout;

		std::vector<VkDescriptorPool> descriptorSetPools;
		std::vector<VkDescriptorSet> emptyDescriptors;

		VkDescriptorPoolSize poolSize[MAX_DESCRIPTOR_TYPES];
		uint32 poolSizeCount;
	};
}

TRE_NS_END