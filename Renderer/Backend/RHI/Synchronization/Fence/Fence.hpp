#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
    class RenderDevice;

	struct FenceDeleter
	{
		void operator()(class Fence* fence);
	};

	class Fence : public Utils::RefCounterEnabled<Fence, FenceDeleter, HandleCounter>
	{
	public:
		~Fence();

		void Wait(uint64_t timeout = UINT64_MAX);

		void Reset();

		FORCEINLINE VkFence GetApiObject() const { return fence; }

		FORCEINLINE bool BeenWaiting() const { return beenWaiting; }
	private:
        Fence(RenderDevice& device, VkFence fence)
            : device(device), fence(fence), beenWaiting(false)
		{}
	private:
        RenderDevice& device;
		VkFence fence;
		bool beenWaiting;

		friend struct FenceDeleter;
		friend class Utils::ObjectPool<Fence>;
	};

	using FenceHandle = Handle<Fence>;
}

TRE_NS_END
