#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API MemoryAllocator
	{
	public:
        CONSTEXPR static DeviceSize TOTAL_MEM_SIZE = 1024 * 1024 * 4;

		struct AllocKey
		{
			FORCEINLINE uint32 GetBindingIndex() { return key & (((uint32)1 << 15) - 1); };

			FORCEINLINE uint32 GetChunkIndex() { return (key & (((uint32)1 << 27) - 1)) >> 15; };

			FORCEINLINE uint32 GetMemoryTypeIndex() { return key >> 27; };

			AllocKey(uint32 memTypeIndex, uint32 chunk, uint32 binding) : key((memTypeIndex << 27) | (chunk << 15) | (binding)) {}

            AllocKey(uint32 key) : key(key) {}

			uint32 key;
		};
	public:
		MemoryAllocator();

		void Init(const Internal::RenderDevice& renderDevice);

		MemoryView Allocate(uint32 memoryTypeIndex, DeviceSize size, DeviceSize alignement = 0);

		void Free(AllocKey key);

		MemoryView GetMemoryViewFromAllocKey(AllocKey key);

		void Destroy();
	private:
		struct BindingInfo
		{
			VkDeviceSize		offset;
			VkDeviceSize		size;
			VkDeviceSize		padding;
			uint32				alignment;
		};
		
		struct Chunk
		{
			VkDeviceMemory			 memory = VK_NULL_HANDLE;
            VkDeviceSize			 totalSize = 0;
            void*					 mappedData = NULL;

			std::vector<BindingInfo> bindingList;
		};

		struct AllocPool
		{
			std::vector<Chunk>				chunks;
			const Internal::RenderDevice*	renderDevice;
			uint32							memoryTypeIndex;

			Chunk* GetLatestChunk(DeviceSize totalSize);

			Chunk* CreateNewChunk(DeviceSize totalSize);

			AllocKey Allocate(DeviceSize size, DeviceSize alignment = 0);

			void FreeBinding(AllocKey key);

			void FreeChunk(AllocKey key);

			void Free(AllocKey key);

			void DeallocateChunk();

			MemoryView GetMemoryView(AllocKey key);
		};

		static bool FindFreeBinding(const Chunk& chunk, DeviceSize alignment, BindingInfo& bindingOut);
	private:
		AllocPool						 allocatedList[VK_MAX_MEMORY_TYPES];
		const Internal::RenderDevice*    renderDevice;
	};
}

TRE_NS_END
