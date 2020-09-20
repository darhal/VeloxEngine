#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	struct SamplerInfo
	{
        VkFilter                magFilter;
        VkFilter                minFilter;
        VkSamplerMipmapMode     mipmapMode;
        VkSamplerAddressMode    addressModeU;
        VkSamplerAddressMode    addressModeV;
        VkSamplerAddressMode    addressModeW;
        float                   mipLodBias;
        VkBool32                anisotropyEnable;
        float                   maxAnisotropy;
        VkBool32                compareEnable;
        VkCompareOp             compareOp;
        float                   minLod;
        float                   maxLod;
        VkBorderColor           borderColor;
        VkBool32                unnormalizedCoordinates;

        static FORCEINLINE SamplerInfo Sampler2D()
        {
            SamplerInfo info;
            info.magFilter = VK_FILTER_NEAREST;
            info.minFilter = VK_FILTER_NEAREST;

            info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

            info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            info.mipLodBias = 0.0f;

            info.anisotropyEnable = VK_TRUE;
            info.maxAnisotropy = 16.0f;

            info.compareEnable = VK_FALSE;
            info.compareOp = VK_COMPARE_OP_ALWAYS;

            info.minLod = 0.0f;
            info.maxLod = 0.0f;

            info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            info.unnormalizedCoordinates = VK_FALSE;
            return info;
        }

        static FORCEINLINE void FillVkSamplerCreateInfo(const SamplerInfo& samplerInfo, VkSamplerCreateInfo& createInfo)
        {
            createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            createInfo.pNext = NULL;
            createInfo.flags = 0;
            createInfo.magFilter = samplerInfo.magFilter;
            createInfo.minFilter = samplerInfo.minFilter;
            createInfo.mipmapMode = samplerInfo.mipmapMode;
            createInfo.addressModeU = samplerInfo.addressModeU;
            createInfo.addressModeV = samplerInfo.addressModeV;
            createInfo.addressModeW = samplerInfo.addressModeW;
            createInfo.mipLodBias = samplerInfo.mipLodBias;
            createInfo.anisotropyEnable = samplerInfo.anisotropyEnable;
            createInfo.maxAnisotropy = samplerInfo.maxAnisotropy;
            createInfo.compareEnable = samplerInfo.compareEnable;
            createInfo.compareOp = samplerInfo.compareOp;
            createInfo.minLod = samplerInfo.minLod;
            createInfo.maxLod = samplerInfo.maxLod;
            createInfo.borderColor = samplerInfo.borderColor;
            createInfo.unnormalizedCoordinates = samplerInfo.unnormalizedCoordinates;
        }
	};

    typedef SamplerInfo SamplerCreateInfo;

	class Sampler : public NoRefCount
	{
	public:
        Sampler(VkSampler sampler, const SamplerInfo& info);

        const SamplerInfo& GetInfo() const { return info; }

        VkSampler GetAPIObject() const { return sampler; }
	private:
        SamplerInfo info;
        VkSampler sampler;
	};

    using SamplerHandle = Handle<Sampler>;
}

TRE_NS_END