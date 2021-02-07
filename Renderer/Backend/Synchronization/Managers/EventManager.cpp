#include "EventManager.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

Renderer::EventManager::~EventManager()
{
	this->Destroy();
}

void Renderer::EventManager::Init(RenderDevice* device)
{
	this->device = device;
}

VkEvent Renderer::EventManager::RequestEvent()
{
	if (events.empty()) {
		VkEventCreateInfo info = { VK_STRUCTURE_TYPE_EVENT_CREATE_INFO };
		VkEvent event;
		vkCreateEvent(device->GetDevice(), &info, NULL, &event);
		return event;
	} else {
		auto event = events.back();
		events.pop_back();
		return event;
	}
}

void Renderer::EventManager::Recycle(VkEvent event)
{
	events.push_back(event);
}

void Renderer::EventManager::Destroy()
{
    for (VkEvent e : events)
        vkDestroyEvent(device->GetDevice(), e, NULL);

    events.clear();
}

TRE_NS_END