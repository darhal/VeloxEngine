#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderBackend;
	class PipelineEvent;

	struct EventDeleter
	{
		void operator()(PipelineEvent* event);
	};

	class PipelineEvent : public Utils::RefCounterEnabled<PipelineEvent, EventDeleter, HandleCounter>
	{
	public:
		~PipelineEvent();

		void SetStages(VkPipelineStageFlags stages)
		{
			stages = stages;
		}

		FORCEINLINE VkPipelineStageFlags GetStages() const { return stages; }

		FORCEINLINE VkEvent GetEvent() const { return event; }
	private:
		PipelineEvent(RenderBackend& backend, VkEvent event) :
			backend(backend), event(event), stages(0)
		{
		}

	private:
		RenderBackend& backend;
		VkEvent event;
		VkPipelineStageFlags stages;

		friend struct EventDeleter;
		friend class Utils::ObjectPool<PipelineEvent>;
	};

	using PiplineEventHandle = Handle<PipelineEvent>;
	using EventHandle = PiplineEventHandle;
}

TRE_NS_END