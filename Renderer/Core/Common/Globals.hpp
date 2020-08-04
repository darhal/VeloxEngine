#pragma once

#include <Renderer/Common.hpp>
#include <initializer_list>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

class Window;

namespace Renderer
{
	const std::initializer_list<const char*> VK_REQ_EXT = {
		"VK_KHR_surface",
		"VK_KHR_win32_surface",
		"VK_EXT_debug_utils",
		"VK_EXT_debug_report"
	};

	const std::initializer_list<const char*> VK_REQ_LAYERS = {
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_LUNARG_standard_validation"
	};


	const std::initializer_list<const char*> VK_REQ_DEVICE_EXT = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	enum QueueFamilyTypes {
		QFT_GRAPHICS = 0,
		QFT_COMPUTE = 1,
		QFT_TRANSFER = 2,
		QFT_SPARSE = 3,
		QFT_PRESENT = 4,

		QFT_MAX = 5
	};

	struct SwapChain
	{
		CONSTEXPR static uint32		MAX_FRAMES_IN_FLIGHT = 2;

		TRE::Vector<VkSemaphore>	imageAvailableSemaphores;
		TRE::Vector<VkSemaphore>	renderFinishedSemaphores;
		TRE::Vector<VkFence>		inFlightFences;
		TRE::Vector<VkFence>		imagesInFlight;

		TRE::Vector<VkImage>		swapChainImages;
		TRE::Vector<VkImageView>    swapChainImageViews;
		VkSwapchainKHR				swapChain;
		VkFormat					swapChainImageFormat;
		VkExtent2D					swapChainExtent;

		uint32						currentFrame;
	};

	struct RenderInstance {
		VkInstance					instance;

		// Debugging
		VkDebugUtilsMessengerEXT	debugMessenger;
	};

	struct RenderContext
	{
		RenderInstance*		instance;

		VkPhysicalDevice	gpu;
		VkDevice			device;
		::TRE::Window*		window;
		VkSurfaceKHR		surface;

		SwapChain			swapChain;

		VkQueue				queues[QFT_MAX];
	};
};

TRE_NS_END