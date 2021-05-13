#include "Sampler.hpp"
#include <Renderer/Backend/RHI/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::SamplerDeleter::operator()(Sampler* sampler)
{
    sampler->device.GetObjectsPool().samplers.Free(sampler);
    // sampler->sampler = VK_NULL_HANDLE;
}

Renderer::Sampler::Sampler(RenderDevice& device, VkSampler sampler, const SamplerInfo& info) :
    device(device), sampler(sampler), info(info)
{
}

Renderer::Sampler::~Sampler()
{
    if (sampler != VK_NULL_HANDLE) {
        device.DestroySampler(sampler);
        sampler = VK_NULL_HANDLE;
    }
}

TRE_NS_END
