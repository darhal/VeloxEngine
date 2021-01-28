#include "Semaphore.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

void Renderer::SemaphoreDeleter::operator()(Semaphore* semaphore)
{
	if (semaphore->clean)
		semaphore->backend.GetObjectsPool().semaphores.Free(semaphore);
}

Renderer::Semaphore::~Semaphore()
{
	// Either recycle or destroy ?
	if (semaphore != VK_NULL_HANDLE) {
		backend.RecycleSemaphore(semaphore);
	}	
}


TRE_NS_END