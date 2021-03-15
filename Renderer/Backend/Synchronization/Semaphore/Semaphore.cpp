#include "Semaphore.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::SemaphoreDeleter::operator()(Semaphore* semaphore)
{
    semaphore->device.GetObjectsPool().semaphores.Free(semaphore);
}

Renderer::Semaphore::~Semaphore()
{
	// Either recycle or destroy ?
    if (clean && semaphore != VK_NULL_HANDLE) {
        if (this->GetType() == BINARY) {
            device.RecycleSemaphore(semaphore);
        }else{
            // We dont reycyle timeline sempahores as there is no way to reset them
            device.DestroySemaphore(semaphore);
        }
    }
}

void Renderer::Semaphore::Signal(uint64 signalValue)
{
    ASSERTF(GetType() != TIMELINE, "Can't perform this operation on a regular semaphore a timeline semaphore is needed");

    VkSemaphoreSignalInfo signalInfo;
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.pNext = NULL;
    signalInfo.semaphore = semaphore;
    signalInfo.value = signalValue;

    vkSignalSemaphore(device.GetDevice(), &signalInfo);
    tempValue = signalValue;
}

void Renderer::Semaphore::Wait(uint64 waitValue)
{
    ASSERTF(GetType() != TIMELINE, "Can't perform this operation on a regular semaphore a timeline semaphore is needed");

    waitValue = waitValue ? waitValue : tempValue;

    VkSemaphoreWaitInfo waitInfo;
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext = NULL;
    waitInfo.flags = 0;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &semaphore;
    waitInfo.pValues = &waitValue;

    vkWaitSemaphores(device.GetDevice(), &waitInfo, UINT64_MAX);
}

uint64 Renderer::Semaphore::GetCurrentCounterValue() const
{
    ASSERTF(GetType() != TIMELINE, "Can't perform this operation on a regular semaphore a timeline semaphore is needed");

    uint64_t value;
    vkGetSemaphoreCounterValue(device.GetDevice(), semaphore, &value);
    return value;
}

void Renderer::Semaphore::Reset()
{
    ASSERTF(GetType() != TIMELINE, "Can't perform this operation on a regular semaphore a timeline semaphore is needed");

    device.ResetTimelineSemaphore(*this);
}


TRE_NS_END
