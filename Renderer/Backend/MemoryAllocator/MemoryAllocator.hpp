#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API MemoryAllocator
	{
	public:
		CONSTEXPR static DeviceSize TOTAL_MEM_SIZE = 1024 * 4;

	public:
		MemoryAllocator();

		void Init(const Internal::RenderDevice& renderDevice);

		MemoryView Allocate(uint32 memoryTypeIndex, DeviceSize size, DeviceSize alignement = 0);

		void Free();
	private:
		struct BindingInfo
		{
			VkDeviceSize		offset;
			VkDeviceSize		size;
			VkDeviceSize		padding;
		};
		
		struct Chunk
		{
			VkDeviceMemory			 memory;
			VkDeviceSize			 totalSize;
			void*					 mappedData;

			std::vector<BindingInfo> bindingList;
		};

		struct AllocKey
		{
			CONSTEXPR static uint32 ALLOC_INDEX_BITS		= 12;
			
			CONSTEXPR static uint32 ALLOC_CHUNK_INDEX_BITS	= sizeof(uint32) * BITS_PER_BYTE - ALLOC_INDEX_BITS;

			FORCEINLINE uint32 GetBindingIndex() { return key & (((uint32)1 << ALLOC_CHUNK_INDEX_BITS) - 1); };

			FORCEINLINE uint32 GetChunkIndex() { return key >> ALLOC_CHUNK_INDEX_BITS; };

			AllocKey(uint32 chunk, uint32 binding) : key((chunk << ALLOC_CHUNK_INDEX_BITS) | (binding))
			{

			}

			uint32 key;
		};

		struct AllocPool
		{
			std::vector<Chunk>		chunks;
			uint32					memoryTypeIndex;

			Chunk* GetLatestChunk(VkDevice device, DeviceSize totalSize);

			Chunk* CreateNewChunk(VkDevice device, DeviceSize totalSize);

			AllocKey Allocate(VkDevice device, DeviceSize size, DeviceSize alignment = 0);

			void Free(AllocKey key);

			MemoryView GetMemoryView(AllocKey key);
		};

		static bool FindFreeBinding(const Chunk& chunk, DeviceSize alignment, BindingInfo& bindingOut);
	private:
		AllocPool	allocatedList[VK_MAX_MEMORY_TYPES];
		VkDevice    device;
	};
}

TRE_NS_END