#include "SemaphoreManager.hpp"
#include <Renderer/Backend/RHI/RenderDevice/RenderDevice.hpp>

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

VkSemaphore Renderer::SemaphoreManager::RequestTimelineSemaphore(uint64 value)
{
    if (timelineSemaphore.empty()) {
        VkSemaphoreTypeCreateInfo timelineCreateInfo;
        timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        timelineCreateInfo.pNext = NULL;
        timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        timelineCreateInfo.initialValue = value;

        VkSemaphoreCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = &timelineCreateInfo;
        info.flags = 0;

        VkSemaphore sem;
        vkCreateSemaphore(device->GetDevice(), &info, NULL, &sem);
        return sem;
    } else {
        // TODO: this path wont be hit as we destroy timeline semaphores instead of recycling them
        auto sem = timelineSemaphore.back();
        timelineSemaphore.pop_back();

        VkSemaphoreSignalInfo signalInfo;
        signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
        signalInfo.pNext = NULL;
        signalInfo.semaphore = sem;
        signalInfo.value = 1;
        vkSignalSemaphore(device->GetDevice(), &signalInfo);
        return sem;
    }
}

void Renderer::SemaphoreManager::RecycleTimelineSemaphore(VkSemaphore semaphore)
{
    if (semaphore != VK_NULL_HANDLE)
        timelineSemaphore.push_back(semaphore);
}

void Renderer::SemaphoreManager::Destroy()
{
    for (VkSemaphore sem : semaphores)
        vkDestroySemaphore(device->GetDevice(), sem, NULL);

    for (VkSemaphore sem : timelineSemaphore)
        vkDestroySemaphore(device->GetDevice(), sem, NULL);

    semaphores.clear();
    timelineSemaphore.clear();
}

TRE_NS_END
