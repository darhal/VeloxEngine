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

VkDeviceAddress Renderer::Blas::GetAcclAddress() const
{
	VkAccelerationStructureDeviceAddressInfoKHR addressInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR };
	addressInfo.accelerationStructure = apiBlas;
	return vkGetAccelerationStructureDeviceAddressKHR(backend.GetRenderDevice().GetDevice(), &addressInfo);
}

TRE_NS_END

