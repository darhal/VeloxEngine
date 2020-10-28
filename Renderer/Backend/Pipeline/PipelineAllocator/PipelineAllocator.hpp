#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Pipeline/GraphicsPipeline.hpp>
#include <Renderer/Core/Hashmap/TemporaryHashmap.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderBackend;
	class GraphicsPipleine;
	class ShaderProgram;
	class RenderPass;

	class RENDERER_API PipelineAllocator
	{
	public:
		PipelineAllocator(RenderBackend* backend);

		GraphicsPipeline& RequestPipline(const RenderPass& rp, const ShaderProgram& program, const GraphicsState& state);

		void BeginFrame();

		void Clear();
	private:
		RenderBackend* renderBackend;
		std::unordered_map<Hash, GraphicsPipeline> pipelineCache;
	};
}

TRE_NS_END