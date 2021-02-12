#include "Semaphore.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::SemaphoreDeleter::operator()(Semaphore* semaphore)
{
	if (semaphore->clean)
        semaphore->device.GetObjectsPool().semaphores.Free(semaphore);
}

Renderer::Semaphore::~Semaphore()
{
	// Either recycle or destroy ?
	if (semaphore != VK_NULL_HANDLE) {
        device.RecycleSemaphore(semaphore);
	}	
}


TRE_NS_END
