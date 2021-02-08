#include "Sampler.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

void Renderer::SamplerDeleter::operator()(Sampler* sampler)
{
	sampler->backend.GetObjectsPool().samplers.Free(sampler);
    // sampler->sampler = VK_NULL_HANDLE;
}

Renderer::Sampler::Sampler(RenderBackend& backend, VkSampler sampler, const SamplerInfo& info) :
	backend(backend), sampler(sampler), info(info)
{
}

Renderer::Sampler::~Sampler()
{
    if (sampler != VK_NULL_HANDLE) {
        backend.DestroySampler(sampler);
        sampler = VK_NULL_HANDLE;
    }
}

TRE_NS_END
