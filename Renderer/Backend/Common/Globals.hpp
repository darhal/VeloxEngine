#pragma once

#include <Renderer/Common.hpp>
#include <initializer_list>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>
#include <Engine/Core/Misc/Maths/Common.hpp>
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

	CONSTEXPR static uint32			MAX_FRAMES = 3;

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
		USAGE_GPU_ONLY,
		USAGE_CPU_ONLY,
		USAGE_CPU_TO_GPU,
		USAGE_GPU_TO_CPU,
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

	enum QueueFamilyFlag 
	{
		NONE	 = 0x0,
		GRAPHICS = 0x1,
		COMPUTE  = 0x2,
		TRANSFER = 0x4,
		SPARSE	 = 0x8,
		PRESENT  = 0x10,
	};

	struct MemoryView
	{
		VkDeviceMemory  memory;
		DeviceSize	    offset;
		DeviceSize		size;
		uint32		    padding;
	};

	struct RingBuffer
	{
		MemoryView	bufferMemory;
		VkBuffer    buffer;
		uint32		ring_size;
		uint32		unit_size;
	};

	class Buffer;

	namespace Internal {
		CONSTEXPR static QueueFamilyFlag QUEUE_FAMILY_FLAGS[] = { GRAPHICS, COMPUTE, TRANSFER, SPARSE, PRESENT };

		enum QueueFamilyTypes {
			QFT_GRAPHICS = 0,
			QFT_COMPUTE = 1,
			QFT_TRANSFER = 2,
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

		struct Vertex
		{
			TRE::vec3 pos;
			TRE::vec3 color;

			Vertex(const TRE::vec3& pos, const TRE::vec3 color) :
				pos(pos), color(color)
			{
			}

			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				return bindingDescription;
			}

			static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);

				return attributeDescriptions;
			}
		};


		struct VertextAttribDesc
		{
			uint32		location;
			uint32		offset;
			VkFormat	format;
		};

		struct VertexInputDesc
		{
			uint32							binding;
			uint32							stride;
			VkVertexInputRate				inputRate;

			TRE::Vector<VertextAttribDesc>	attribDesc;
		};

		struct InputAssemblyDesc
		{
			VkPrimitiveTopology                        topology;
			VkBool32                                   primitiveRestartEnable;
		};

		struct RasterizationDesc
		{
			VkPolygonMode                              polygonMode;
			VkCullModeFlags                            cullMode;
			VkFrontFace                                frontFace;

			VkBool32                                   depthClampEnable;
			VkBool32                                   rasterizerDiscardEnable;
			VkBool32                                   depthBiasEnable;

			float                                      depthBiasConstantFactor;
			float                                      depthBiasClamp;
			float                                      depthBiasSlopeFactor;
			float                                      lineWidth;
		};

		// struct DynamicStateDesc

		// struct VkPipelineColorBlendStateCreateInfo

		// VkPipelineMultisampleStateCreateInfo

		struct PipelineLayoutDesc
		{
			TRE::Vector<VkDescriptorSetLayout>	descriptorSetLayout;
			TRE::Vector<VkPushConstantRange>	pushConstantRanges;
		};

		typedef TRE::Vector<VkPipelineShaderStageCreateInfo> ShaderStagesDesc;

		struct GraphicsPiplineDesc
		{
			ShaderStagesDesc				shaderStagesDesc;
			TRE::Vector<VertexInputDesc>	vertexInputDesc;
			InputAssemblyDesc				inputAssemblyDesc;
			RasterizationDesc				rasterStateDesc;
			PipelineLayoutDesc				piplineLayoutDesc;

			// VkPipelineColorBlendAttachmentState(*)
			// VkPipelineColorBlendStateCreateInfo(1)

			// VkPipelineMultisampleStateCreateInfo(1)

			// VkPipelineDynamicStateCreateInfo(1)

			VkViewport						viewport;
			VkRect2D						scissor;

			uint32_t						subpass;
			VkPipeline						basePipelineHandle;
			int32_t							basePipelineIndex;
		};

		struct GraphicsPipeline
		{
			VkPipeline						pipeline;
			VkPipelineLayout				pipelineLayout;
			VkRenderPass					renderPass;
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

		struct SwapChainData
		{
			CONSTEXPR static uint32			MAX_FRAMES_IN_FLIGHT	= MAX_FRAMES;
			CONSTEXPR static uint32			MAX_IMAGES_COUNT		= 4;

			VkSemaphore						imageAcquiredSemaphores[MAX_FRAMES_IN_FLIGHT];
			VkSemaphore						drawCompleteSemaphores[MAX_FRAMES_IN_FLIGHT];
			VkFence							fences[MAX_FRAMES_IN_FLIGHT];

			// To use when using seprate presentation queue:
			VkSemaphore						imageOwnershipSemaphores[MAX_FRAMES_IN_FLIGHT];

			// To use when using seprate transfer queue:
			VkSemaphore						transferSemaphores[MAX_FRAMES_IN_FLIGHT];
			VkFence							transferSyncFence;

			// Swap chain images:
			VkImage							swapChainImages[MAX_IMAGES_COUNT];
			VkImageView						swapChainImageViews[MAX_IMAGES_COUNT];
			VkFramebuffer					swapChainFramebuffers[MAX_IMAGES_COUNT];

			// Other misc swapchain data:
			VkFormat						swapChainImageFormat;
			VkExtent2D						swapChainExtent;
		};

		struct ContextFrameResources
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

			VkRenderPass						renderPass;

			uint32								transferRequests;

			ContextFrameResources				contextFrameResources[SwapChainData::MAX_FRAMES_IN_FLIGHT];
		};


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
			VkDevice							device;

			QueueFamilyIndices					queueFamilyIndices;
			VkQueue								queues[QFT_MAX];

			VkPhysicalDeviceMemoryProperties	memoryProperties;

			bool								isPresentQueueSeprate;
			bool								isTransferQueueSeprate;
		};

		struct RenderContext
		{
			RenderDevice*					renderDevice;

			::TRE::Window*					window;
			VkSurfaceKHR					surface;

			VkSwapchainKHR					swapChain;
			SwapChainData					swapChainData;
			RenderContextData				contextData;

			// Swap chain current frame and current buffer and images count
			uint32							imagesCount;
			uint32							currentFrame;
			uint32							currentImage;
			
			bool							framebufferResized;

			VkDevice GetDevice() const { return renderDevice->device; }
		};
	}
};

TRE_NS_END