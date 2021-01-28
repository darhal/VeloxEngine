#include "SBT.hpp"
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/ShaderProgram/ShaderProgram.hpp>
#include <Renderer/Core/Alignement/Alignement.hpp>

TRE_NS_START

void Renderer::SBT::Init(RenderBackend& backend, const ShaderProgram& program, Pipeline& pipline)
{
	const RenderDevice& device = backend.GetRenderDevice();
	auto groupCount = program.GetShaderGroupsCount();

	uint32_t groupHandleSize = device.GetRtProperties().shaderGroupHandleSize;  // Size of a program identifier
	uint32_t baseAlignment = device.GetRtProperties().shaderGroupBaseAlignment;  // Size of shader alignment
	// Compute the actual size needed per SBT entry (round-up to alignment needed).
	uint32_t groupSizeAligned = Utils::AlignUp(groupHandleSize, baseAlignment);

	 // Fetch all the shader handles used in the pipeline, so that they can be written in the SBT
	uint32_t sbtSize = groupCount * groupSizeAligned;

	std::vector<uint8_t> shaderHandleStorage(sbtSize);
	vkGetRayTracingShaderGroupHandlesKHR(device.GetDevice(), pipline.GetApiObject(), 0,
		groupCount, sbtSize, shaderHandleStorage.data());

	BufferCreateInfo info;
	info.size = sbtSize;
	info.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::SHADER_BINDING_TABLE;
	info.domain = MemoryUsage::CPU_COHERENT;

	if (!sbtBuffer || sbtBuffer->GetBufferInfo().size != sbtSize) { // Update
		sbtBuffer = backend.CreateBuffer(info);
		address = device.GetBufferAddress(sbtBuffer);
	}

	for (uint32_t g = 0; g < 4; g++) {
		sbtEntries[g] = { 0, 0, 0 };
	}

	for (uint32_t g = 0; g < groupCount; g++) {
		sbtBuffer->WriteToBuffer(groupHandleSize, shaderHandleStorage.data() + g * groupHandleSize, groupSizeAligned * g);
		sbtEntries[g] = { address + g * groupSizeAligned, groupSizeAligned , groupSizeAligned };
	}
}

TRE_NS_END


