#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

namespace Renderer
{
	typedef bool(*FPN_RankGPU)(VkPhysicalDevice, VkSurfaceKHR);

	bool IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface);

	VkPhysicalDevice PickGPU(const RenderContext& p_ctx, FPN_RankGPU p_pick_func = IsDeviceSuitable);

	Renderer::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice p_gpu, VkSurfaceKHR p_surface = NULL);
};

TRE_NS_END