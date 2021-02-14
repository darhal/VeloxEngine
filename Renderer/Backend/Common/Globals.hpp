#pragma once

#include <vector>
#include <array>
#include <initializer_list>

#include <Engine/Core/DataStructure/Vector/Vector.hpp>
// #include <Engine/Core/Misc/Maths/Common.hpp>

#include <Renderer/Common.hpp>
#include <Renderer/Core/ObjectPool/ObjectPool.hpp>
#include <Renderer/Core/Allocators/StackAllocator.hpp>
#include <Renderer/Core/Hash/Hash.hpp>
#include <Renderer/Core/Handle/Handle.hpp>
#include <Renderer/Core/Hash/Hashable.hpp>
#include <Renderer/Core/StaticVector/StaticVector.hpp>
#include <Renderer/Core/ArrayView/ArrayView.hpp>

#include "Utils.hpp"
#include "Loader/Extensions.hpp"

TRE_NS_START

class Window;

namespace Renderer
{
#define CALL_VK(cmd) \
		{\
			VkResult r;\
			if ((r = cmd) != VK_SUCCESS) {\
				TRE_LOGE("[VK ERROR]: %s", GetVulkanResultString(r));\
			}\
		}\

	const std::initializer_list<const char*> VK_REQ_EXT = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,

#if defined(OS_WINDOWS)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(OS_LINUX)
		VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#endif
#if defined(DEBUG) && defined(VALIDATION_LAYERS)
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
	};

	const std::initializer_list<const char*> VK_REQ_LAYERS = {
		"VK_LAYER_LUNARG_monitor",
#if defined(DEBUG) && defined(VALIDATION_LAYERS)
		"VK_LAYER_KHRONOS_validation",
#endif
	};

	const std::initializer_list<const char*> VK_REQ_DEVICE_EXT = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	const std::initializer_list<const char*> DEV_EXTENSIONS[] = {
		// Ray-Tracing
		{
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_SPIRV_1_4_EXTENSION_NAME,
			VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,

			
			VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
			VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_KHR_MAINTENANCE3_EXTENSION_NAME,
			VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		},
	};
	
	// Threading related constants:
	CONSTEXPR static uint32 MAX_THREADS				= 1;

	// Frame related constants:
    CONSTEXPR static uint32	MAX_FRAMES				= 2;
    CONSTEXPR static uint32	NUM_FRAMES				= 2;
    CONSTEXPR static uint32	MAX_IMAGES_COUNT		= 8;

	// Descriptor related constants:
	CONSTEXPR static uint32	MAX_DESCRIPTOR_SET		= 4;
	CONSTEXPR static uint32	MAX_DESCRIPTOR_BINDINGS = 16;
	CONSTEXPR static uint32 MAX_DESCRIPTOR_TYPES	= 11;
	CONSTEXPR static uint32 MAX_SETS_PER_POOL		= 1; // this have to be 1 on Intel graphics dunno why!
	CONSTEXPR static uint32 MAX_PUSH_CONSTANT_SIZE  = 128;
	CONSTEXPR static uint32 MAX_SHADER_CONSTANTS    = 16;

	// Renderpass and framebuffers:
	CONSTEXPR static uint32 MAX_ATTACHMENTS			= 8;

	CONSTEXPR static uint32 DESCRIPTOR_RING_SIZE	= 8;
	CONSTEXPR static uint32 FRAMEBUFFER_RING_SIZE	= 8;

	CONSTEXPR static uint32 MAX_CMD_LIST_SUBMISSION			= 32;
	CONSTEXPR static uint32 MAX_WAIT_SEMAPHORE_PER_QUEUE    = 64;

	template<typename T>
	using ObjectPool = Utils::ObjectPool<T>;

	template<typename T>
	using Handle = Utils::Handle<T>;

	using HandleCounter = Utils::SingleThreadRefCounter;

	using NoRefCount = Utils::RefCounterEnabled<void, void, void>;

	template<typename T, size_t S>
	using StackAlloc = Utils::StackAllocator<T, S>;

	using Hash = Utils::Hash;

	using Hasher = Utils::Hasher;

	using Hashable = Utils::Hashable;

	template<typename T, size_t N = 32>
	using StaticVector = Utils::StaticVector<T, N>;

	using ArrayView = Utils::ArrayView;

	template<typename T>
	using ArrayViewItr = Utils::ArrayViewItr<T>;

	typedef VkDeviceSize DeviceSize;

    enum Features
    {
        RAY_TRACING = 0x1,
    };

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
		SHADER_DEVICE_ADDRESS_EXT = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT,
		VSHADER_DEVICE_ADDRESS = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR,

		ACCLS_BUILD_INPUT_READ_ONLY = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
		ACCLS_STORAGE = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
		SHADER_BINDING_TABLE = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
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

	struct MemoryView
	{
		VkDeviceMemory  memory;
		DeviceSize	    offset;
		DeviceSize		size;
		DeviceSize		padding;
		void*			mappedData;
		uint32			alignment;

        uint32 allocKey;
	};

	template<typename T>
	struct VectorView
	{
		const T* data;
		uint32 size;
	};

	enum QueueFamilyFlag 
	{
		NONE	 = 0x0,
		GRAPHICS = 0x1,
		COMPUTE  = 0x2,
		TRANSFER = 0x4,
		SPARSE	 = 0x8,
		PRESENT  = 0x10,

		GENERIC = GRAPHICS | COMPUTE | TRANSFER,
	};

	enum class QueueTypes
	{
		GENERIC = 0,
		TRANSFER_ONLY = 2,
		COMPUTE_ONLY = 3,

		MAX,
	};

	class Buffer;

	namespace Internal {
		CONSTEXPR static QueueFamilyFlag QUEUE_FAMILY_FLAGS[] = { GRAPHICS, TRANSFER, COMPUTE, SPARSE, PRESENT };

		enum QueueFamilyTypes {
			QFT_GRAPHICS = 0,
			QFT_TRANSFER,
			QFT_COMPUTE,
			// QFT_RAY_TRACING,
			QFT_SPARSE,
			QFT_PRESENT,
			
			QFT_MAX
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
			VkPhysicalDeviceProperties2			gpuProperties2;

			// RT
			VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rtProperties;
			VkPhysicalDeviceFeatures2						 deviceFeatures2;
			VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeatures;
			VkPhysicalDeviceRayTracingPipelineFeaturesKHR	 rtPipelineFeatures;
			VkPhysicalDeviceBufferDeviceAddressFeatures		 buffAdrFeatures;

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
			uint32							previousFrame;
			uint32							currentFrame;
			uint32							numFramesInFlight;
			
			bool							framebufferResized;

			VkDevice GetDevice() const { return renderDevice->device; }
		};
	}
};

TRE_NS_END
