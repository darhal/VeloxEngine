#pragma once

#include <Renderer/Backend/Common.hpp>
#include <vector>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;

	class FenceManager
	{
	public:
		void Init(RenderDevice* device);

		~FenceManager();

		VkFence RequestClearedFence();

		void Recycle(VkFence fence);

		void Destroy();
	private:
		RenderDevice* device;
		std::vector<VkFence> fences;
	};
};

TRE_NS_END