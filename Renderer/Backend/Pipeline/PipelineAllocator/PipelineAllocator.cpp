#include "PipelineAllocator.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

Renderer::PipelineAllocator::PipelineAllocator(RenderBackend* backend) : renderBackend(backend)
{
}

Renderer::Pipeline& Renderer::PipelineAllocator::RequestPipline(const ShaderProgram& program, const RenderPass& rp, const GraphicsState& state)
{
	// Graphics pipeline:
	Hasher h;
	h.u64(rp.GetHash());
	h.u64(program.GetHash());
	h.u64(state.GetHash());

	auto ret = pipelineCache.emplace(std::piecewise_construct, std::forward_as_tuple(h.Get()), std::forward_as_tuple(PipelineType::GRAPHICS, &program));
	Pipeline& pipline = ret.first->second;

	if (ret.second) {
		pipline.SetRenderPass(&rp);
		// pipline.SetShaderProgram(&program);
		pipline.Create(renderBackend->GetRenderContext(), state);
	}

	return pipline;
}

Renderer::Pipeline& Renderer::PipelineAllocator::RequestPipline(const ShaderProgram& program)
{
	// Compute pipeline:
	Hasher h;
	h.u64(program.GetHash());

	auto ret = pipelineCache.emplace(std::piecewise_construct, std::forward_as_tuple(h.Get()), std::forward_as_tuple(PipelineType::COMPUTE, &program));
	Pipeline& pipline = ret.first->second;

	if (ret.second) {
		pipline.Create(renderBackend->GetRenderDevice());
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

