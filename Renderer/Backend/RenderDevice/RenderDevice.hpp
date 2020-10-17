#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
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

		VkDeviceMemory AllocateDedicatedMemory(VkImage image, MemoryUsage memoryDomain = MemoryUsage::GPU_ONLY) const;

		VkDeviceMemory AllocateDedicatedMemory(VkBuffer buffer, MemoryUsage memoryDomain = MemoryUsage::GPU_ONLY) const;

		void FreeDedicatedMemory(VkDeviceMemory memory) const;

		VkSampleCountFlagBits GetUsableSampleCount(uint32 sampleCount = 4) const;

		VkSampleCountFlagBits GetMaxUsableSampleCount() const;

		FORCEINLINE VkDevice GetDevice() const { return internal.device; }

		FORCEINLINE VkPhysicalDevice GetGPU() const { return internal.gpu; }

		FORCEINLINE const Internal::QueueFamilyIndices& GetQueueFamilyIndices() const { return internal.queueFamilyIndices; }

		FORCEINLINE const VkQueue* const GetQueues() const { return internal.queues; }

		FORCEINLINE VkQueue GetQueue(uint32 i) const { return internal.queues[i]; }

		FORCEINLINE bool IsPresentQueueSeprate() const { return internal.isPresentQueueSeprate; }

		FORCEINLINE bool IsTransferQueueSeprate() const { return internal.isTransferQueueSeprate; }
	private:
		typedef bool(*FPN_RankGPU)(VkPhysicalDevice, VkSurfaceKHR);

		static bool IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface);

		static VkPhysicalDevice PickGPU(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx, FPN_RankGPU p_pick_func = IsDeviceSuitable);

		static Internal::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice p_gpu, VkSurfaceKHR p_surface = NULL); 
	private:
		Internal::RenderDevice internal;

		friend class RenderBackend;
	};
};

TRE_NS_END