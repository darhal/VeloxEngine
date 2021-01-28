#pragma once

#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>

TRE_NS_START

namespace Renderer
{
	class ShaderProgram;
	class Pipeline;
	class RenderBackend;

	class SBT
	{
	public:
		SBT() = default;

		void Init(RenderBackend& backend, const ShaderProgram& program, Pipeline& pipline);

		// BufferHandle GetSbtBuffer() const { return sbtBuffer; };

		// VkDeviceAddress GetSbtAddress() const { return address; }

		BufferHandle GetSbtBuffer(uint32 i) const { return sbtBuffer[i]; };

		VkDeviceAddress GetSbtAddress(uint32 i) const { return address[i]; }

		const VkStridedDeviceAddressRegionKHR& GetSbtEntry(uint32 i) const { return sbtEntries[i]; }
	//private:
		BufferHandle sbtBuffer[4];
		VkDeviceAddress address[4];
		VkStridedDeviceAddressRegionKHR sbtEntries[4];
	};
}


TRE_NS_END