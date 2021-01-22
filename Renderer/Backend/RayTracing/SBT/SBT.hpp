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

		BufferHandle GetSbtBuffer() const { return sbtBuffer; };

		VkDeviceAddress GetSbtAddress() const { return address; }
	private:
		BufferHandle sbtBuffer;
		VkDeviceAddress address;
	};
}


TRE_NS_END