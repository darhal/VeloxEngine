#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	Buffer CreateBuffer(const RenderDevice& renderDevice, VkDeviceSize size, uint32 usage, VkMemoryPropertyFlags properties);

	void DestroyBuffer(const RenderDevice& renderDevice, Buffer& buffer)

	void AllocateMemory(const RenderDevice& renderDevice, Buffer& buffer, VkMemoryPropertyFlags properties);

	uint32 FindMemoryType(const RenderDevice& renderDevice, uint32 typeFilter, VkMemoryPropertyFlags properties);
}

TRE_NS_END
