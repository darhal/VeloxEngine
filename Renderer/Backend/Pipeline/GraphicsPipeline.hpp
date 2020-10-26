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
	class RenderContext;

	struct ShaderSpecilization
	{

	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() : shaderProgram{}, renderPass(VK_NULL_HANDLE), pipeline(VK_NULL_HANDLE) {}

		void Create(
			const RenderContext& renderContext,
			const VertexInput& vertexInput, 
			GraphicsState& state);

		void Create(
			const RenderContext& renderContext,
			GraphicsState& state);

		const PipelineLayout& GetPipelineLayout() const { return shaderProgram.GetPipelineLayout(); }

		ShaderProgram& GetShaderProgram() { return shaderProgram; }

		const ShaderProgram& GetShaderProgram() const { return shaderProgram; }

		VkPipeline GetAPIObject() const { return pipeline; }

		VkRenderPass GetRenderPassAPIObject() const { return renderPass; }

		void SetRenderPass(VkRenderPass renderPass) { this->renderPass = renderPass; }
	private:
		ShaderProgram					shaderProgram;
		VkRenderPass					renderPass;
		VkPipeline						pipeline;
		// ShaderSpecilization				shaderConstants[MAX_SHADER_CONSTANTS];
	};
}

TRE_NS_END