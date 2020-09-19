#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	struct ResourceBinding
	{
		union Resource
		{
			VkDescriptorBufferInfo buffer;
			VkDescriptorImageInfo image;
			// VkBufferView buffer_view;
		} resource;

		uint32 dynamicOffset;
	};

	struct ResouceBindings
	{
		ResourceBinding bindings[MAX_DESCRIPTOR_SET][MAX_DESCRIPTOR_BINDINGS];
	};
}

TRE_NS_END