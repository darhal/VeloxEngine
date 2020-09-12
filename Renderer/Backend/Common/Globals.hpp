#pragma once

#include <Renderer/Common.hpp>
#include <initializer_list>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>
#include <Engine/Core/Misc/Maths/Common.hpp>
#include <Renderer/Core/ObjectPool/ObjectPool.hpp>
#include <Renderer/Core/Allocators/StackAllocator.hpp>
#include <Renderer/Core/Hash/Hash.hpp>
#include <Renderer/Core/Handle/Handle.hpp>
#include <vector>
#include <array>

TRE_NS_START

class Window;

namespace Renderer
{
	const std::initializer_list<const char*> VK_REQ_EXT = {
		"VK_KHR_surface",
		"VK_KHR_win32_surface",

#if defined(DEBUG) && defined(VALIDATION_LAYERS)
		"VK_EXT_debug_utils",
		"VK_EXT_debug_report"
#endif
	};

	const std::initializer_list<const char*> VK_REQ_LAYERS = {
#if defined(DEBUG) && defined(VALIDATION_LAYERS)
		"VK_LAYER_KHRONOS_validation"
#endif
	};

	const std::initializer_list<const char*> VK_REQ_DEVICE_EXT = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	// Threading related constants:
	CONSTEXPR static uint32 MAX_THREADS				= 1;

	// Frame related constants:
	CONSTEXPR static uint32	MAX_FRAMES				= 2;
	CONSTEXPR static uint32	NUM_FRAMES				= 2;
	CONSTEXPR static uint32	MAX_IMAGES_COUNT		= 4;

	// Descriptor related constants:
	CONSTEXPR static uint32	MAX_DESCRIPTOR_SET		= 4;
	CONSTEXPR static uint32	MAX_DESCRIPTOR_BINDINGS = 16;
	CONSTEXPR static uint32 MAX_DESCRIPTOR_TYPES	= 11;
	CONSTEXPR static uint32 MAX_SETS_PER_POOL		= 16;

	// Renderpass and framebuffers:
	CONSTEXPR static uint32 MAX_ATTACHMENTS			= 8;

	template<typename T>
	using ObjectPool = Utils::ObjectPool<T>;

	template<typename T>
	using Handle = Utils::Handle<T>;

	template<typename T, size_t S>
	using StackAlloc = Utils::StackAllocator<T, S>;

	using Hash = Utils::Hash;
	using Hasher = Utils::Hasher;

	typedef VkDeviceSize DeviceSize;

	enum MemoryProperty
	{
		DEVICE_LOCAL	= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		HOST_VISIBLE	= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		HOST_COHERENT	= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		HOST_CACHED		= VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
		LAZILY_ALLOCATED = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
		PROTECTED		= VK_MEMORY_PROPERTY_PROTECTED_BIT,
		DEVICE_COHERENT = VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD,
		DEVICE_UNCACHED = VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD,
	};

	enum class MemoryUsage
	{
		USAGE_UNKNOWN,
		GPU_ONLY,			// Device local. Probably not visible from CPU.
		LINKED_GPU_CPU,		// On desktop, directly mapped VRAM over PCI.
		CPU_ONLY,			// Host-only, needs to be synced to GPU. Might be device local as well on iGPUs.
		CPU_CACHED,			// Host and cached
		CPU_COHERENT,		// Host and coherent
	};

	enum BufferUsage 
	{
		TRANSFER_SRC = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		TRANSFER_DST = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		UNIFORM_TEXEL = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
		STORAGE_TEXEL = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
		UNIFORM_BUFFER = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		STORAGE_BUFFER = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		INDEX_BUFFER = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VERTEX_BUFFER = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		INDIRECT_BUFFER = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
		SHADER_DEVICE_ADDRESS = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		TRANSFORM_FEEDBACK_BUFFER = VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT,
		TRANSFORM_FEEDBACK_COUNTER_BUFFER = VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT,
		CONDITIONAL_RENDERING = VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT,
		RAY_TRACING = VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR,
		SHADER_DEVICE_ADDRESS_EXT = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT,
		VSHADER_DEVICE_ADDRESS = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR,
	};

	enum SharingMode
	{
		EXCLUSIVE	= VK_SHARING_MODE_EXCLUSIVE,
		CONCURRENT	= VK_SHARING_MODE_CONCURRENT,
	};

	enum ShaderStagesFlagsBits
	{
		VERTEX_SHADER = VK_SHADER_STAGE_VERTEX_BIT,
		FRAGMENT_SHADER = VK_SHADER_STAGE_FRAGMENT_BIT,
		COMPUTE_SHADER = VK_SHADER_STAGE_COMPUTE_BIT,
		GEOMETRY_SHADER = VK_SHADER_STAGE_GEOMETRY_BIT,
		TESSELLATION_CONTROL_SHADER = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
		TESSELLATION_EVAL_SHADER = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
	};

	typedef uint32 ShaderStagesFlags;

	enum class DescriptorType
	{
		SAMPLER = VK_DESCRIPTOR_TYPE_SAMPLER,
		COMBINED_IMAGE_SAMPLER = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		SAMPLED_IMAGE = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ,
		STORAGE_IMAGE = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ,
		UNIFORM_TEXEL_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
		STORAGE_TEXEL_BUFFER =VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
		STORAGE_BUFFER = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		UNIFORM_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		UNIFORM_BUFFER_DYNC = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		STORAGE_BUFFER_DYNC = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		INPUT_ATTACHMENT = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
	};

	enum QueueFamilyFlag 
	{
		NONE	 = 0x0,
		GRAPHICS = 0x1,
		COMPUTE  = 0x2,
		TRANSFER = 0x4,
		SPARSE	 = 0x8,
		PRESENT  = 0x10,
	};

	enum class QueueTypes
	{
		GRAPHICS_ONLY = 0,
		TRANSFER_ONLY = 1, 
		// COMPUTE_ONLY = 2,
		
		// GENERIC = 3,

		MAX,
	};

	struct MemoryView
	{
		VkDeviceMemory  memory;
		DeviceSize	    offset;
		DeviceSize		size;
		DeviceSize		padding;
		void*			mappedData;
		uint32			alignment;
	};

	class Buffer;

	namespace Internal {
		CONSTEXPR static QueueFamilyFlag QUEUE_FAMILY_FLAGS[] = { GRAPHICS, TRANSFER, COMPUTE, SPARSE, PRESENT };

		enum QueueFamilyTypes {
			QFT_GRAPHICS = 0,
			QFT_TRANSFER = 1,
			QFT_COMPUTE = 2,
			QFT_SPARSE = 3,
			QFT_PRESENT = 4,

			QFT_MAX = 5
		};

		struct QueueFamilyIndices
		{
			CONSTEXPR static uint32 REQUIRED_QUEUES[] = { QFT_GRAPHICS, QFT_TRANSFER };

			uint32 queueFamilies[QFT_MAX] = { UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX };

			bool IsComplete() const
			{
				for (uint32 queueFamilyIndex : REQUIRED_QUEUES) {
					if (queueFamilies[queueFamilyIndex] == UINT32_MAX) {
						return false;
					}
				}

				return true;
			}
		};

		struct RenderPassDesc
		{
			TRE::Vector<VkSubpassDescription>		subpassesDesc;
			TRE::Vector<VkSubpassDependency >		subpassDependency;
			TRE::Vector<VkAttachmentDescription>	attachments;
		};

		struct TransferBufferInfo
		{
			Buffer* srcBuffer;
			Buffer* dstBuffer;
			TRE::Vector<VkBufferCopy>	copyRegions;
		};

		/*struct ContextFrameResources
		{
			VkCommandBuffer		graphicsCommandBuffer;
			VkCommandBuffer		transferCommandBuffer;
			VkCommandBuffer		presentCommandBuffer;
		};

		struct RenderContextData
		{
			VkCommandPool						commandPool;
			VkCommandPool						memoryCommandPool;
			VkCommandPool						presentCommandPool;

			ContextFrameResources				contextFrameResources[MAX_FRAMES];
		};*/

		struct RenderInstance
		{
			VkInstance						instance;

			// Debugging
			VkDebugUtilsMessengerEXT		debugMessenger;
		};

		struct RenderDevice
		{
			struct RenderContext*				renderContext;

			VkPhysicalDevice					gpu;
			VkPhysicalDeviceProperties			gpuProperties;
			VkPhysicalDeviceFeatures			gpuFeatures;
			VkPhysicalDeviceMemoryProperties	memoryProperties;


			VkDevice							device;
			QueueFamilyIndices					queueFamilyIndices;
			VkQueue								queues[QFT_MAX];
			bool								isPresentQueueSeprate;
			bool								isTransferQueueSeprate;
		};

		struct RenderContext
		{
			RenderDevice*					renderDevice;

			::TRE::Window*					window;
			VkSurfaceKHR					surface;

			// Swap chain current frame and current buffer and images count
			uint32							currentImage;
			uint32							currentFrame;
			uint32							numFramesInFlight;
			
			bool							framebufferResized;

			VkDevice GetDevice() const { return renderDevice->device; }
		};
	}
};

TRE_NS_END