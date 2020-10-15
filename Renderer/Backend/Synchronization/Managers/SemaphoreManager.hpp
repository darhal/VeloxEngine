#pragma once

#include <Renderer/Common.hpp>
#include <vector>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;

	class SemaphoreManager
	{
	public:
		~SemaphoreManager();

		void Init(RenderDevice* device);

		VkSemaphore RequestSemaphore();
		
		void Recycle(VkSemaphore semaphore);
	private:
		RenderDevice* device;
		std::vector<VkSemaphore> semaphores;
	};
}

TRE_NS_END