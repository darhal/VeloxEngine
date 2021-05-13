#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>
#include <Renderer/Backend/RHI/Pipeline/GraphicsState/GraphicsState.hpp>
#include <Renderer/Backend/RHI/ShaderProgram/ShaderProgram.hpp>
#include <Renderer/Backend/RHI/Pipeline/VertexInput/VertexInput.hpp>
#include <Renderer/Backend/RHI/Pipeline/PipelineLayout/PipelineLayout.hpp> 
#include <Renderer/Backend/RHI/RenderPass/RenderPass.hpp>
#include <Renderer/Backend/RHI/Pipeline/Pipeline.hpp>
#include <Renderer/Backend/RHI/RayTracing/SBT/SBT.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderContext;
	class RenderBackend;

	enum class PipelineType
	{
		GRAPHICS = VK_PIPELINE_BIND_POINT_GRAPHICS,
		COMPUTE = VK_PIPELINE_BIND_POINT_COMPUTE,
		RAY_TRACE = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
	};

	class Pipeline
	{
	public:
		// Generic:
		Pipeline(PipelineType type, ShaderProgram* program = NULL) :
		    renderDevice(NULL), pipelineType(type), pipeline(VK_NULL_HANDLE), shaderProgram(program), renderPass(NULL), dynamicState(0),
			sbt()
		{
		}

		~Pipeline();

		FORCEINLINE VkPipeline GetApiObject() const { return pipeline; }

		const PipelineLayout& GetPipelineLayout() const { return shaderProgram->GetPipelineLayout(); }

		void SetShaderProgram(ShaderProgram* program) { shaderProgram = program; }

		const ShaderProgram* GetShaderProgram() const { return shaderProgram; }

		PipelineType GetPipelineType() const { return pipelineType; }

		// RT:
        void Create(RenderDevice& device, uint32 maxDepth, uint32 maxRayPayloadSize = 1, uint32 maxRayHitAttribSize = 1);

		const SBT& GetSBT() const { return sbt; }

		// Compute:
        void Create(RenderDevice& device);

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
        RenderDevice*  renderDevice;
        ShaderProgram*       shaderProgram;
		VkPipeline			 pipeline;
		PipelineType		 pipelineType;

		SBT					sbt;
		
		const RenderPass*	renderPass;
		uint32				dynamicState;

		// ShaderSpecilization				shaderConstants[MAX_SHADER_CONSTANTS];
	};
}

TRE_NS_END
