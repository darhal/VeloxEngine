#include "PipelineAllocator.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

Renderer::PipelineAllocator::PipelineAllocator(RenderBackend* backend) : renderBackend(backend)
{
}

Renderer::GraphicsPipeline& Renderer::PipelineAllocator::RequestPipline(const RenderPass& rp, const ShaderProgram& program, const GraphicsState& state)
{
	Hasher h;
	h.u64(rp.GetHash());
	h.u64(program.GetHash());
	h.u64(state.GetHash());

	auto ret = pipelineCache.emplace(std::make_pair(h.Get(), &program));
	GraphicsPipeline& pipline = ret.first->second;

	if (ret.second) {
		pipline.SetRenderPass(&rp);
		// pipline.SetShaderProgram(&program);
		pipline.Create(renderBackend->GetRenderContext(), state);
	}

	return pipline;
}

void Renderer::PipelineAllocator::BeginFrame()
{
}

void Renderer::PipelineAllocator::Clear()
{
	pipelineCache.clear();
}


TRE_NS_END

