#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	void CreateGraphicsPipeline(const RenderDevice& renderDevice, GraphicsPipeline& pipline, const GraphicsPiplineDesc& desc);

	VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

	void CreateRenderPass(const RenderDevice& renderDevice, VkRenderPass* renderPass, const RenderPassDesc& desc);
}

TRE_NS_END