#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

namespace Renderer
{
	bool IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface);

	typedef bool(*FPN_RankGPU)(VkPhysicalDevice, VkSurfaceKHR);

	int32 CreateRenderDevice(RenderDevice& renderDevice, const RenderInstance& renderInstance, const RenderContext& ctx);

	void DestroryRenderDevice(RenderDevice& renderDevice);

	int32 CreateLogicalDevice(RenderDevice& renderDevice, const RenderInstance& renderInstance, const RenderContext& ctx);

	VkPhysicalDevice PickGPU(const RenderInstance& renderInstance, const RenderContext& ctx, FPN_RankGPU p_pick_func = IsDeviceSuitable);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice p_gpu, VkSurfaceKHR p_surface = NULL);
};

TRE_NS_END