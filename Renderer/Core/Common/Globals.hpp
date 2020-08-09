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
		uint32 queueFamilies[QFT_MAX] = { uint32(-1) };

		bool IsComplete() const
		{
			for (uint32 queueFamily : queueFamilies) {
				if (queueFamily != uint32(-1)) {
					return true;
				}
			}

			return false;
		}
	};

	struct Vertex 
	{
		TRE::vec3 pos;
		TRE::vec3 color;

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
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	struct Buffer
	{
		VkBuffer		buffer;
		VkDeviceMemory	bufferMemory;
	};

	struct SwapChainData
	{
		CONSTEXPR static uint32			MAX_FRAMES_IN_FLIGHT = 2;

		VkCommandPool					commandPool;
		TRE::Vector<VkCommandBuffer>	commandBuffers;

		std::vector<VkSemaphore>		imageAvailableSemaphores;
		std::vector<VkSemaphore>		renderFinishedSemaphores;
		std::vector<VkFence>			inFlightFences;
		std::vector<VkFence>			imagesInFlight;

		std::vector<VkImage>			swapChainImages;
		TRE::Vector<VkImageView>		swapChainImageViews;
		VkFormat						swapChainImageFormat;
		VkExtent2D						swapChainExtent;

		uint32							currentFrame;

		std::vector<VkFramebuffer>		swapChainFramebuffers;
		VkPipelineLayout				pipelineLayout;
		VkRenderPass					renderPass;
		VkPipeline						graphicsPipeline;
	};

	struct RenderInstance 
	{
		VkInstance						instance;

		// Debugging
		VkDebugUtilsMessengerEXT		debugMessenger;
	};

	struct RenderDevice
	{
		VkPhysicalDevice				gpu;
		VkDevice						device;

		QueueFamilyIndices				queueFamilyIndices;
		VkQueue							queues[QFT_MAX];

		VkPhysicalDeviceMemoryProperties memoryProperties;
	};

	struct RenderContext 
	{
		::TRE::Window*					window;
		VkSurfaceKHR					surface;

		VkSwapchainKHR					swapChain;
		SwapChainData					swapChainData;

		bool							framebufferResized;
	};

	struct RenderEngine
	{
		RenderInstance*					renderInstance;
		RenderDevice*					renderDevice;
		RenderContext*					renderContext;
	};
};

TRE_NS_END