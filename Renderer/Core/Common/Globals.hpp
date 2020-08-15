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

#if defined(DEBUG)
		"VK_EXT_debug_utils",
		"VK_EXT_debug_report"
#endif
	};

	const std::initializer_list<const char*> VK_REQ_LAYERS = {
#if defined(DEBUG)
		"VK_LAYER_KHRONOS_validation"
#endif
	};

	const std::initializer_list<const char*> VK_REQ_DEVICE_EXT = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

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
			bindingDescription.binding	 = 0;
			bindingDescription.stride	 = sizeof(Vertex);
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

	struct Buffer
	{
		VkBuffer		buffer;
		VkDeviceMemory	bufferMemory;
	};

	struct TransferBufferInfo
	{
		Buffer*						srcBuffer;
		Buffer*						dstBuffer; 
		TRE::Vector<VkBufferCopy>	copyRegions;
	};

	// namespace Internal {

		struct SwapChainData
		{
			CONSTEXPR static uint32			MAX_FRAMES_IN_FLIGHT = 2;

			VkSemaphore						imageAcquiredSemaphores[MAX_FRAMES_IN_FLIGHT];
			VkSemaphore						drawCompleteSemaphores[MAX_FRAMES_IN_FLIGHT];
			VkFence							fences[MAX_FRAMES_IN_FLIGHT];

			// To use when using seprate presentation queue:
			VkSemaphore						imageOwnershipSemaphores[MAX_FRAMES_IN_FLIGHT];

			// To use when using seprate transfer queue:
			VkSemaphore						transferSemaphores[MAX_FRAMES_IN_FLIGHT];

			VkFormat						swapChainImageFormat;
			VkExtent2D						swapChainExtent;
		};

		struct ContextFrameResources
		{
			VkCommandBuffer		graphicsCommandBuffer;
			VkCommandBuffer		transferCommandBuffer;
			VkCommandBuffer		presentCommandBuffer;

			VkImage				swapChainImage;
			VkImageView			swapChainImageView;
			VkFramebuffer		swapChainFramebuffer;
		};

		struct RenderContextData
		{
			VkCommandPool						commandPool;
			VkCommandPool						memoryCommandPool;
			VkCommandPool						presentCommandPool;

			VkRenderPass						renderPass;

			uint32								currentFrame;
			uint32								currentBuffer;
			uint32								imagesCount;

			uint32								transferRequests;

			std::vector<ContextFrameResources>	contextFrameResources;
		};


		struct RenderInstance
		{
			VkInstance						instance;

			// Debugging
			VkDebugUtilsMessengerEXT		debugMessenger;
		};

		struct RenderDevice
		{
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
			::TRE::Window* window;
			VkSurfaceKHR					surface;

			VkSwapchainKHR					swapChain;
			SwapChainData					swapChainData;
			RenderContextData				contextData;

			bool							framebufferResized;
		};


	//namespace Internal {
		struct RenderEngine
		{
			RenderInstance* renderInstance;
			RenderDevice* renderDevice;
			RenderContext* renderContext;
		};
	// }
};

TRE_NS_END