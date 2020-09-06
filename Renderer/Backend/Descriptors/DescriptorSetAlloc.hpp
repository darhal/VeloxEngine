#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetLayout.hpp>
#include <unordered_map>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;

	class DescriptorSetAllocator
	{
	public:
		DescriptorSetAllocator(RenderDevice* renderDevice, const DescriptorSetLayout& layout);

		// Make it easier to deal with emplace ctor later with map
		DescriptorSetAllocator(const std::pair<RenderDevice*, const DescriptorSetLayout&>& renderDeviceLayoutPair);

		void Init();

		VkDescriptorSet Allocate();

		// @return: Descriptor potentially cached, bool: true cache found, false otherwise
		std::pair<VkDescriptorSet, bool> Find(Hash hash);

		void Free(VkDescriptorSet set);
	private:
		void AllocatePool();
	private:
		RenderDevice* renderDevice;
		const DescriptorSetLayout& descriptorSetLayout;

		std::vector<VkDescriptorPool> descriptorSetPools;
		std::vector<VkDescriptorSet> emptyDescriptors;
		std::unordered_map<Hash, VkDescriptorSet> descriptorCache;

		VkDescriptorPoolSize poolSize[MAX_DESCRIPTOR_TYPES];
		uint32 poolSizeCount;
	};
}

TRE_NS_END