#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Descriptors/DescriptorSetLayout.hpp>
#include <unordered_map>
#include <Renderer/Core/Hashmap/TemporaryHashmap.hpp>

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

		// @return: Descriptor potentially cached, bool: true cache found, false otherwise
		std::pair<VkDescriptorSet, bool> Find(Hash hash);

		void Clear();

		void BeginFrame();

		FORCEINLINE const DescriptorSetLayout& GetDescriptorSetLayout() const { return descriptorSetLayout; }
	private:
		void AllocatePool();

		struct DescriptorSetNode : Utils::HashmapNode<DescriptorSetNode>, Utils::ListNode<DescriptorSetNode>
		{
			explicit DescriptorSetNode(VkDescriptorSet set_)
				: set(set_)
			{
			}

			VkDescriptorSet set;
		};

	private:
		RenderDevice* renderDevice;
		const DescriptorSetLayout& descriptorSetLayout;

		std::vector<VkDescriptorPool> descriptorSetPools;		
		Utils::TemporaryHashmap<DescriptorSetNode, DESCRIPTOR_RING_SIZE, true> descriptorCache;
		VkDescriptorPoolSize poolSize[MAX_DESCRIPTOR_TYPES];
		uint32 poolSizeCount;
		bool shouldBegin;
	};
}

TRE_NS_END