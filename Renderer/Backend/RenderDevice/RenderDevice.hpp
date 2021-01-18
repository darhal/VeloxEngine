#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <unordered_map>
#include <unordered_set>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderDevice
	{
	public:
		RenderDevice();

		int32 CreateRenderDevice(
			const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx,
			const char** extensions = NULL, uint32 extCount = 0, const char** layers = NULL, uint32 layerCount = 0);

		void DestroryRenderDevice();

		int32 CreateLogicalDevice(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx, 
			const char** extensions = NULL, uint32 extCount = 0, const char** layers = NULL, uint32 layerCount = 0);

		VkDeviceMemory AllocateDedicatedMemory(VkImage image, MemoryUsage memoryDomain = MemoryUsage::GPU_ONLY) const;

		VkDeviceMemory AllocateDedicatedMemory(VkBuffer buffer, MemoryUsage memoryDomain = MemoryUsage::GPU_ONLY) const;

		void FreeDedicatedMemory(VkDeviceMemory memory) const;

		VkSampleCountFlagBits GetUsableSampleCount(uint32 sampleCount = 4) const;

		VkSampleCountFlagBits GetMaxUsableSampleCount() const;

		uint32 FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) const;

		uint32 FindMemoryTypeIndex(uint32 typeFilter, MemoryUsage usage) const;

		VkBuffer CreateBuffer(const BufferInfo& info) const;

		VkDeviceMemory CreateBufferMemory(const BufferInfo& info, VkBuffer buffer,
			VkDeviceSize* alignedSize = NULL, uint32 multiplier = 1) const;

		VkAccelerationStructureKHR CreateAcceleration(VkAccelerationStructureCreateInfoKHR& info, VkBuffer* buffer) const;

		FORCEINLINE VkDeviceAddress GetBufferAddress(BufferHandle buff) const;

		FORCEINLINE VkDevice GetDevice() const { return internal.device; }

		FORCEINLINE VkPhysicalDevice GetGPU() const { return internal.gpu; }

		FORCEINLINE const Internal::QueueFamilyIndices& GetQueueFamilyIndices() const { return internal.queueFamilyIndices; }

		FORCEINLINE const VkQueue* const GetQueues() const { return internal.queues; }

		FORCEINLINE VkQueue GetQueue(uint32 i) const { return internal.queues[i]; }

		FORCEINLINE bool IsPresentQueueSeprate() const { return internal.isPresentQueueSeprate; }

		FORCEINLINE bool IsTransferQueueSeprate() const { return internal.isTransferQueueSeprate; }

		FORCEINLINE const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return internal.memoryProperties; }
	private:
		void FetchDeviceAvailableExtensions();

		typedef bool(*FPN_RankGPU)(VkPhysicalDevice, VkSurfaceKHR);

		static bool IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface);

		static VkPhysicalDevice PickGPU(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx, FPN_RankGPU p_pick_func = IsDeviceSuitable);

		static Internal::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice p_gpu, VkSurfaceKHR p_surface = NULL); 
	private:
		Internal::RenderDevice internal;
		std::unordered_set<uint64> deviceExtensions;
		std::unordered_set<uint64> availbleDevExtensions;

		friend class RenderBackend;
	};
};

TRE_NS_END