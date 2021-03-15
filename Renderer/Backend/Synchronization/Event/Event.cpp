#include "Event.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::EventDeleter::operator()(PipelineEvent* event)
{
    event->device.GetObjectsPool().events.Free(event);
}

Renderer::PipelineEvent::~PipelineEvent()
{
	if (event != VK_NULL_HANDLE)
        device.DestroryEvent(event);
}

TRE_NS_END
