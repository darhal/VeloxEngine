#include "TLAS.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START


void Renderer::TlasDeleter::operator()(Tlas* tlas)
{
    tlas->device.GetObjectsPool().tlases.Free(tlas);
}

Renderer::Tlas::Tlas(RenderDevice& device, const TlasCreateInfo& tlasInfo,
	VkAccelerationStructureKHR tlas, BufferHandle buffer, BufferHandle instanceBuffer) :
    device(device), tlasInfo(tlasInfo), apiTlas(tlas), buffer(buffer), instanceBuffer(instanceBuffer)
{
}


TRE_NS_END
