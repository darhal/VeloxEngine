#include "BLAS.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::BlasDeleter::operator()(Blas* blas)
{
    blas->device.GetObjectsPool().blases.Free(blas);
}

Renderer::Blas::Blas(RenderDevice& device, const BlasCreateInfo& blasInfo, VkAccelerationStructureKHR blas, BufferHandle buffer)
    : device(device), apiBlas(blas), blasInfo(blasInfo), buffer(buffer)
{
}

VkDeviceAddress Renderer::Blas::GetAcclAddress() const
{
	VkAccelerationStructureDeviceAddressInfoKHR addressInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR };
	addressInfo.accelerationStructure = apiBlas;
    return vkGetAccelerationStructureDeviceAddressKHR(device.GetDevice(), &addressInfo);
}

TRE_NS_END

