#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Pipeline/GraphicsState/GraphicsState.hpp>
#include <Renderer/Backend/ShaderProgram/ShaderProgram.hpp>
#include <Renderer/Backend/Pipeline/VertexInput/VertexInput.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Internal {
		void CreateGraphicsPipeline(const RenderDevice& renderDevice, GraphicsPipeline2& pipline, const GraphicsPiplineDesc& desc);

		VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

		void CreateRenderPass(const RenderDevice& renderDevice, VkRenderPass* renderPass, const RenderPassDesc& desc);
	}

	class GraphicsPipeline
	{
	public:
		void Init(const Internal::RenderDevice& renderDevice, const ShaderProgram& shaderProgram, const VertexInput& vertexInput, const GraphicsState& state);

		VkPipeline						pipeline;
		VkPipelineLayout				pipelineLayout;
		VkRenderPass					renderPass;
	};
}

TRE_NS_END