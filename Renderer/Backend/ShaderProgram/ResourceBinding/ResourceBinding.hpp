#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	struct ResourceBinding
	{
		union
		{
			VkDescriptorBufferInfo buffer;

			struct
			{
				VkDescriptorImageInfo fp;
				VkDescriptorImageInfo integer;
			} image;

			VkBufferView buffer_view;
		};

		VkDeviceSize dynamicOffset;
	};

	struct ResouceBindings
	{
		ResourceBinding bindings[MAX_DESCRIPTOR_SET][MAX_DESCRIPTOR_BINDINGS];
	};
}

TRE_NS_END