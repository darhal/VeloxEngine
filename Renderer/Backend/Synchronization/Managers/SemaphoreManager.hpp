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

        VkSemaphore RequestTimelineSemaphore(uint64 value = 1);

        void RecycleTimelineSemaphore(VkSemaphore semaphore);

		void Destroy();
	private:
		RenderDevice* device;
		std::vector<VkSemaphore> semaphores;
        std::vector<VkSemaphore> timelineSemaphore;
	};
}

TRE_NS_END
