#include "Sampler.hpp"

TRE_NS_START

Renderer::Sampler::Sampler(VkSampler sampler, const SamplerInfo& info) :
	sampler(sampler), info(info)
{
}

TRE_NS_END
