#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderBackend;

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

			acclGeo.PushBack(VkAccelerationStructureGeometryKHR{ 
				VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR, NULL, 
				VK_GEOMETRY_TYPE_TRIANGLES_KHR, triangles, flags 
			});
			accOffset.PushBack(VkAccelerationStructureBuildRangeInfoKHR{ offset });
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

		Blas(RenderBackend& backend, const BlasCreateInfo& blasInfo, VkAccelerationStructureKHR blas, BufferHandle buffer);

		const BlasCreateInfo& GetInfo() const { return blasInfo; }

		FORCEINLINE VkAccelerationStructureKHR GetApiObject() const { return apiBlas; }

		FORCEINLINE const BufferHandle GetBuffer() const { return buffer; }
	private:
		RenderBackend& backend;
		BlasCreateInfo blasInfo;
		VkAccelerationStructureKHR apiBlas;
		BufferHandle buffer;
		
		friend class RenderBackend;
	};

	using BlasHandle = Handle<Blas>;
}

TRE_NS_END