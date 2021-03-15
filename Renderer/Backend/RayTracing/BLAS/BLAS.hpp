#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>

TRE_NS_START

namespace Renderer
{
    class RenderDevice;

	typedef VkAccelerationStructureBuildRangeInfoKHR AsOffset;

	struct BlasCreateInfo
	{
		StaticVector<VkAccelerationStructureGeometryKHR>		acclGeo;
		StaticVector<VkAccelerationStructureBuildRangeInfoKHR>	accOffset;

		//VK_GEOMETRY_TYPE_TRIANGLES_KHR = 0,
		//VK_GEOMETRY_TYPE_AABBS_KHR = 1,
		//VK_GEOMETRY_TYPE_INSTANCES_KHR = 2,

		void AddGeometry(VkDeviceAddress vertexData,
			VkDeviceSize vertexStride, uint32 vertexCount,
			VkDeviceAddress transformData = VK_NULL_HANDLE,
			VkDeviceAddress indexData = VK_NULL_HANDLE,
			const AsOffset& offset = {0, 0, 0, 0},
			uint32 flags = VK_GEOMETRY_OPAQUE_BIT_KHR,
			VkFormat vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
			VkIndexType indexType = VK_INDEX_TYPE_UINT32)
		{
			VkAccelerationStructureGeometryTrianglesDataKHR triangles;
			triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
			triangles.pNext = NULL;

			triangles.vertexFormat = vertexFormat;
			triangles.vertexStride = vertexStride;
			triangles.maxVertex = vertexCount;
			triangles.indexType = indexType;

			triangles.vertexData.deviceAddress = vertexData;
			triangles.indexData.deviceAddress = indexData;
			triangles.transformData.deviceAddress = transformData;

			VkAccelerationStructureGeometryKHR geometry;
			geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			geometry.pNext = NULL;
			geometry.flags = flags;
			geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
			geometry.geometry.triangles = triangles;

			acclGeo.PushBack(geometry);
			accOffset.PushBack(offset);
		}
	};

	struct BlasDeleter
	{
		void operator()(class Blas* blas);
	};

	class Blas : public Utils::RefCounterEnabled<Blas, BlasDeleter, HandleCounter>
	{
	public:
		friend struct BlasDeleter;

        Blas(RenderDevice& device, const BlasCreateInfo& blasInfo, VkAccelerationStructureKHR blas, BufferHandle buffer);

		const BlasCreateInfo& GetInfo() const { return blasInfo; }

		FORCEINLINE VkAccelerationStructureKHR GetApiObject() const { return apiBlas; }

		FORCEINLINE BufferHandle GetBuffer() const { return buffer; }

		VkDeviceAddress GetAcclAddress() const;
	private:
        RenderDevice& device;
		BlasCreateInfo blasInfo;
		VkAccelerationStructureKHR apiBlas;
		BufferHandle buffer;
		
        friend class RenderDevice;
		friend class AsBuilder;
	};

	using BlasHandle = Handle<Blas>;
}

TRE_NS_END
