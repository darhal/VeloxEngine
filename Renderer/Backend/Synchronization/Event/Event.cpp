#include "Event.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

void Renderer::EventDeleter::operator()(PipelineEvent* event)
{
	event->backend.GetObjectsPool().events.Free(event);
}

Renderer::PipelineEvent::~PipelineEvent()
{
	if (event != VK_NULL_HANDLE)
		backend.DestroryEvent(event);
}

TRE_NS_END