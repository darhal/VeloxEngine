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
		GraphicsPipeline() : shaderProgram{}, renderPass(VK_NULL_HANDLE), pipeline(VK_NULL_HANDLE) {}

		void Create(
			const Internal::RenderContext& renderContext,
			const VertexInput& vertexInput, 
			GraphicsState& state);

		void Create(
			const Internal::RenderContext& renderContext,
			GraphicsState& state);

		const PipelineLayout& GetPipelineLayout() const { return shaderProgram.GetPipelineLayout(); }

		ShaderProgram& GetShaderProgram() { return shaderProgram; }

		VkPipeline GetAPIObject() const { return pipeline; }

		VkRenderPass GetRenderPassAPIObject() const { return renderPass; }

		void SetRenderPass(VkRenderPass renderPass) { this->renderPass = renderPass; }
	private:
		ShaderProgram					shaderProgram;
		VkRenderPass					renderPass;
		VkPipeline						pipeline;
	};
}

TRE_NS_END