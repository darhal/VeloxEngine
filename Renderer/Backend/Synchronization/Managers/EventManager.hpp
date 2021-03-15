#pragma once

#include <Renderer/Common.hpp>
#include <vector>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;

	class EventManager
	{
	public:
		~EventManager();

		void Init(RenderDevice* device);

		VkEvent RequestEvent();

		void Recycle(VkEvent event);

		void Destroy();
	private:
		RenderDevice* device;
		std::vector<VkEvent> events;
	};
}

TRE_NS_END