#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include "Buffer.hpp"

TRE_NS_START

namespace Renderer
{
	class RingBuffer : public Buffer
	{
	public:
		void WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset = 0);

		uint32 GetCurrentOffset() const { return bufferIndex * unit_size; }
	private:
		uint32		ring_size;
		uint32		unit_size;
		uint32		bufferIndex;

		friend class RenderContext;
	};
}

TRE_NS_END
