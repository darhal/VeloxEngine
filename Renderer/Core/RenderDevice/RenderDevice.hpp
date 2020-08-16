#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderDevice
	{
	public:
		RenderDevice();

		int32 CreateRenderDevice(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx);

		void DestroryRenderDevice();

		int32 CreateLogicalDevice(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx);

	private:
		typedef bool(*FPN_RankGPU)(VkPhysicalDevice, VkSurfaceKHR);

		static bool IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface);

		static VkPhysicalDevice PickGPU(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx, FPN_RankGPU p_pick_func = IsDeviceSuitable);

		static Internal::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice p_gpu, VkSurfaceKHR p_surface = NULL);

	private:
		Internal::RenderDevice internal;

		friend class RenderEngine;
	};
};

TRE_NS_END