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
	private:
		uint32		ring_size;
		uint32		unit_size;
		uint32		bufferIndex;

		friend class RenderContext;
	};
}

TRE_NS_END
