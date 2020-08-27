#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Pipeline/GraphicsState/GraphicsState.hpp>
#include <Renderer/Backend/ShaderProgram/ShaderProgram.hpp>
#include <Renderer/Backend/Pipeline/VertexInput/VertexInput.hpp>
#include <Renderer/Backend/Pipeline/PipelineLayout/PipelineLayout.hpp> 

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
		GraphicsPipeline() : pipeline(VK_NULL_HANDLE), renderPass(VK_NULL_HANDLE), pipelineLayout{} {}

		void Create(
			const Internal::RenderContext& renderContext,
			const ShaderProgram& shaderProgram, 
			const VertexInput& vertexInput, 
			GraphicsState& state);

		PipelineLayout& GetPipelineLayout() { return pipelineLayout; }

		VkPipeline GetAPIObject() const { return pipeline; }

		VkRenderPass GetRenderPassAPIObject() const { return renderPass; }

		void SetRenderPass(VkRenderPass renderPass) { this->renderPass = renderPass; }
	private:
		VkPipeline						pipeline;
		VkRenderPass					renderPass;
		PipelineLayout					pipelineLayout;
	};
}

TRE_NS_END