#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	struct ResourceBinding
	{
		union Resource
		{
			VkDescriptorBufferInfo buffer;
			VkDescriptorImageInfo image;

#if defined(VK_KHR_acceleration_structure)
			VkWriteDescriptorSetAccelerationStructureKHR accl;
#endif
			// VkBufferView buffer_view;
		} resource;

		uint32 dynamicOffset;
	};

	struct ResouceBindings
	{
		ResourceBinding bindings[MAX_DESCRIPTOR_SET][MAX_DESCRIPTOR_BINDINGS];
		uint64 cache[MAX_DESCRIPTOR_SET][MAX_DESCRIPTOR_BINDINGS];
		uint8 pushConstantData[MAX_PUSH_CONSTANT_SIZE];
	};
}

TRE_NS_END
