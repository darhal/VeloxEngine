#include "BLAS.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

void Renderer::BlasDeleter::operator()(Blas* blas)
{
	blas->backend.GetObjectsPool().blases.Free(blas);
}

Renderer::Blas::Blas(RenderBackend& backend, const BlasCreateInfo& blasInfo, VkAccelerationStructureKHR blas, BufferHandle buffer) 
	: backend(backend), apiBlas(blas), blasInfo(blasInfo), buffer(buffer)
{
}

TRE_NS_END

