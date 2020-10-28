#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Pipeline/GraphicsState/GraphicsState.hpp>
#include <Renderer/Backend/ShaderProgram/ShaderProgram.hpp>
#include <Renderer/Backend/Pipeline/VertexInput/VertexInput.hpp>
#include <Renderer/Backend/Pipeline/PipelineLayout/PipelineLayout.hpp> 
#include <Renderer/Backend/RenderPass/RenderPass.hpp>

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
		GraphicsPipeline(const ShaderProgram* program = NULL) : shaderProgram(program), renderPass(NULL), pipeline(VK_NULL_HANDLE), dynamicState(0) 
		{}

		void Create(
			const RenderContext& renderContext,
			const VertexInput& vertexInput, 
			const GraphicsState& state);

		void Create(
			const RenderContext& renderContext,
			const GraphicsState& state);

		FORCEINLINE VkPipeline GetAPIObject() const { return pipeline; }

		const PipelineLayout& GetPipelineLayout() const { return shaderProgram->GetPipelineLayout(); }

		void SetShaderProgram(const ShaderProgram* program) { shaderProgram = program; }

		const ShaderProgram* GetShaderProgram() const { return shaderProgram; }

		void SetRenderPass(const RenderPass* renderPass) { this->renderPass = renderPass; }

		const RenderPass* GetRenderPass() const { return renderPass; }

		bool IsStateDynamic(VkDynamicState state) const { return dynamicState & (1 << state); }
	private:
		const ShaderProgram*			shaderProgram;
		const RenderPass*				renderPass;
		VkPipeline						pipeline;

		uint32 dynamicState;
		// ShaderSpecilization				shaderConstants[MAX_SHADER_CONSTANTS];
	};
}

TRE_NS_END