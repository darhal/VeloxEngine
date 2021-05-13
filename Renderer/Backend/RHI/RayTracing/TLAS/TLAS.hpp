#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>
#include <Renderer/Backend/RHI/Buffers/Buffer.hpp>
#include <Renderer/Backend/RHI/RayTracing/BLAS/BLAS.hpp>

TRE_NS_START

namespace Renderer
{
    class RenderDevice;

	struct BlasInstance
	{
		BlasHandle blas = BlasHandle{ NULL };
		uint32 hitGroupId = 0;
		uint32 instanceCustomId = 0;
		uint32 mask = 0xFF;
		VkGeometryInstanceFlagsKHR flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		float transform[4][4];
	};

	struct TlasCreateInfo
	{
		std::vector<BlasInstance> blasInstances;
	};

	struct TlasDeleter
	{
		void operator()(class Tlas* tlas);
	};

	class Tlas : public Utils::RefCounterEnabled<Tlas, TlasDeleter, HandleCounter>
	{
	public:
		friend struct TlasDeleter;

        Tlas(RenderDevice& device, const TlasCreateInfo& tlasInfo, VkAccelerationStructureKHR tlas,
			BufferHandle buffer, BufferHandle instancesBuffer);

		FORCEINLINE const TlasCreateInfo& GetInfo() const { return tlasInfo; }

		// FORCEINLINE VkAccelerationStructureKHR GetApiObject() const { return apiTlas; }
		FORCEINLINE const VkAccelerationStructureKHR& GetApiObject() const { return apiTlas; }

		FORCEINLINE BufferHandle GetBuffer() const { return buffer; }
		FORCEINLINE BufferHandle GetInstanceBuffer() const { return instanceBuffer; }
	private:
        RenderDevice& device;
		TlasCreateInfo tlasInfo;
		VkAccelerationStructureKHR apiTlas;
		BufferHandle buffer;
		BufferHandle instanceBuffer;
		

        friend class RenderDevice;
		friend class StagingManager;
	};

	using TlasHandle = Handle<Tlas>;
}

TRE_NS_END
