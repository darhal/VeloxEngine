#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Core/BitmapTree/BitmapTree.hpp>
#include <Renderer/Core/BuddyAllocator/BuddyAllocator.hpp>

TRE_NS_START

namespace Renderer
{   
    class RenderDevice;

    struct MemoryAllocation
    {
        VkDeviceMemory memory;
        VkDeviceSize   size;
        VkDeviceSize   offset;
        VkDeviceSize   padding;
        VkDeviceSize   alignment;
        void*		   mappedData;

        // TODO: idea right now the memory type and the buddy allocator index are stored in alloc key
        // We can add both size and offset in terms of power of two that will be 2*log2(64) bits reserved so 8 bits each
        uint32         allocKey;
    };


    class TypedMemoryAllocator
    {
    private:
        struct DeviceBuddyAllocator : public BuddyAllocator
        {
            DeviceBuddyAllocator() = default;

            DeviceBuddyAllocator(DeviceBuddyAllocator&& other) :
                BuddyAllocator(std::move(*(BuddyAllocator*)(&other))),
                gpuMemory(other.gpuMemory), mappedData(other.mappedData)
            {

            }

            DeviceBuddyAllocator(const DeviceBuddyAllocator& other) :
                BuddyAllocator(*(BuddyAllocator*)(&other)),
                gpuMemory(other.gpuMemory), mappedData(other.mappedData)
            {

            }

            void Create(VkDevice dev, uint32 memTypeIndex, uint32 minSize, uint32 maxSize, bool map)
            {
                VkMemoryAllocateInfo info;
                info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                info.pNext = NULL;
                info.allocationSize = maxSize;
                info.memoryTypeIndex = memTypeIndex;
                vkAllocateMemory(dev, &info, NULL, &gpuMemory);

                if (map) {
                    vkMapMemory(dev, gpuMemory, 0, maxSize, 0, &mappedData);
                }else{
                    mappedData = NULL;
                }

                this->Init(minSize, maxSize);
            }

            void Destroy(VkDevice dev)
            {
                vkFreeMemory(dev, gpuMemory, NULL);
            }

            VkDeviceMemory gpuMemory;
            void* mappedData;
        };
    public:
        // These must be a power of 2
        CONSTEXPR static uint32 MIN_SIZE        = 64;
        CONSTEXPR static uint32 NUM_BLOCKS      = 65536; // The intial pool have 16 MB
        CONSTEXPR static uint32 RESIZE_FACTOR   = 4;  // The pool will grow by a factor of 4 giving: 64, 128, 256, 512 MB respectively

        void Init(const RenderDevice& device, uint32 memoryTypeIndex);

        void Destroy();

        MemoryAllocation Allocate(uint64 size, uint64 alignement);

        void Free(const MemoryAllocation& allocation);

    private:
        std::vector<DeviceBuddyAllocator> allocators;
        VkDevice device;
        uint32 memoryTypeIndex;
        bool map;
    };

    class RENDERER_API MemoryAllocator2
    {
    public:
        MemoryAllocator2(RenderDevice& device);

        void Init();

        void Destroy();

        MemoryAllocation Allocate(uint32 indexType, uint64 size, uint64 alignement = 1);

        void Free(const MemoryAllocation& alloc);
    private:
        RenderDevice& renderDevice;
        TypedMemoryAllocator allocators[VK_MAX_MEMORY_TYPES];
    };














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

        MemoryAllocation Allocate(uint32 memoryTypeIndex, DeviceSize size, DeviceSize alignement = 0);

		void Free(AllocKey key);

        MemoryAllocation GetMemoryViewFromAllocKey(AllocKey key);

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

            MemoryAllocation GetMemoryView(AllocKey key);
		};

		static bool FindFreeBinding(const Chunk& chunk, DeviceSize alignment, BindingInfo& bindingOut);
	private:
		AllocPool						 allocatedList[VK_MAX_MEMORY_TYPES];
		const Internal::RenderDevice*    renderDevice;
	};
}

TRE_NS_END
