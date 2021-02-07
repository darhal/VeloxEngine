#include "SemaphoreManager.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

Renderer::SemaphoreManager::~SemaphoreManager()
{
	this->Destroy();
}

void Renderer::SemaphoreManager::Init(RenderDevice* device)
{
	this->device = device;
}

VkSemaphore Renderer::SemaphoreManager::RequestSemaphore()
{
	if (semaphores.empty()) {
		VkSemaphore sem;
		VkSemaphoreCreateInfo info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		vkCreateSemaphore(device->GetDevice(), &info, NULL, &sem);
		return sem;
	} else {
		auto sem = semaphores.back();
		semaphores.pop_back();
		return sem;
	}
}

void Renderer::SemaphoreManager::Recycle(VkSemaphore semaphore)
{
	if (semaphore != VK_NULL_HANDLE)
		semaphores.push_back(semaphore);
}

void Renderer::SemaphoreManager::Destroy()
{
    for (VkSemaphore sem : semaphores)
        vkDestroySemaphore(device->GetDevice(), sem, NULL);

    semaphores.clear();
}

TRE_NS_END