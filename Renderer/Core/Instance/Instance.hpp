#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Engine/Core/DataStructure/Array/Array.hpp>
#include <initializer_list>

namespace Renderer
{
	const std::initializer_list<const char*> VK_REQ_EXTENSIONS = {
	"VK_KHR_surface",
	"VK_KHR_win32_surface",
	"VK_EXT_debug_utils",
	"VK_EXT_debug_report"
	};

	const std::initializer_list<const char*> VK_REQ_LAYERS = {
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_LUNARG_standard_validation"
	};

	int32 CreateRenderContext(Renderer::RenderContext* p_ctx);

	void DestroyRenderContext(const RenderContext& p_ctx);

	int32 CreateInstance(VkInstance* p_instance);

	void DestroyInstance(VkInstance p_instance);

	// DEBUGGING SECTION // 

	int32 SetupDebugMessenger(VkInstance p_instance, VkDebugUtilsMessengerEXT* p_debugMessenger);

	int32 InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};