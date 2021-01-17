#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Pipeline/GraphicsState/GraphicsState.hpp>
#include <Renderer/Backend/ShaderProgram/ShaderProgram.hpp>
#include <Renderer/Backend/Pipeline/VertexInput/VertexInput.hpp>
#include <Renderer/Backend/Pipeline/PipelineLayout/PipelineLayout.hpp> 
#include <Renderer/Backend/RenderPass/RenderPass.hpp>
#include <Renderer/Backend/Pipeline/Pipeline.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderContext;

	enum class PipelineType
	{
		GRAPHICS = 0,
		COMPUTE,
		RAY_TRACE,
	};

	class Pipeline
	{
	public:
		// Generic:
		Pipeline(PipelineType type, const ShaderProgram* program = NULL) :
			pipelineType(type), pipeline(VK_NULL_HANDLE), shaderProgram(program), renderPass(NULL), dynamicState(0)
		{
		}

		FORCEINLINE VkPipeline GetApiObject() const { return pipeline; }

		const PipelineLayout& GetPipelineLayout() const { return shaderProgram->GetPipelineLayout(); }

		void SetShaderProgram(const ShaderProgram* program) { shaderProgram = program; }

		const ShaderProgram* GetShaderProgram() const { return shaderProgram; }

		PipelineType GetPipelineType() const { return pipelineType; }

		// RT:
		void Create(const RenderDevice& device, bool);

		// Compute:
		void Create(const RenderDevice& device);

		// Graphics:
		void Create(
			const RenderContext& renderContext,
			const VertexInput& vertexInput, 
			const GraphicsState& state);

		void Create(
			const RenderContext& renderContext,
			const GraphicsState& state);

		void SetRenderPass(const RenderPass* renderPass) { this->renderPass = renderPass; }

		const RenderPass* GetRenderPass() const { return renderPass; }

		bool IsStateDynamic(VkDynamicState state) const { return dynamicState & (1 << state); }
	protected:
		const ShaderProgram* shaderProgram;
		VkPipeline			 pipeline;
		PipelineType		 pipelineType;
		
		const RenderPass*	renderPass;
		uint32				dynamicState;

		// ShaderSpecilization				shaderConstants[MAX_SHADER_CONSTANTS];
	};
}

TRE_NS_END