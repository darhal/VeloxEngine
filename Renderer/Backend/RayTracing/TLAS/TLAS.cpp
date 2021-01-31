#include "TLAS.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

void Renderer::TlasDeleter::operator()(Tlas* tlas)
{
	tlas->backend.GetObjectsPool().tlases.Free(tlas);
}

Renderer::Tlas::Tlas(RenderBackend& backend, const TlasCreateInfo& tlasInfo, 
	VkAccelerationStructureKHR tlas, BufferHandle buffer, BufferHandle instanceBuffer) :
	backend(backend), tlasInfo(tlasInfo), apiTlas(tlas), buffer(buffer), instanceBuffer(instanceBuffer)
{
}


TRE_NS_END