#include "Sampler.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

void Renderer::SamplerDeleter::operator()(Sampler* sampler)
{
	sampler->backend.GetObjectsPool().samplers.Free(sampler);
}

Renderer::Sampler::Sampler(RenderBackend& backend, VkSampler sampler, const SamplerInfo& info) :
	backend(backend), sampler(sampler), info(info)
{
}

TRE_NS_END
