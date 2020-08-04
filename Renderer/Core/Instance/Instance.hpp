#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Engine/Core/DataStructure/Array/Array.hpp>
#include <initializer_list>

TRE_NS_START

namespace Renderer
{
	int32 CreateRenderInstance(Renderer::RenderInstance* p_ctx);

	void DestroyRenderInstance(const RenderInstance& p_ctx);

	int32 CreateInstance(VkInstance* p_instance);

	void DestroyInstance(VkInstance p_instance);

	// DEBUGGING SECTION // 

	int32 SetupDebugMessenger(VkInstance p_instance, VkDebugUtilsMessengerEXT* p_debugMessenger);

	int32 InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};

TRE_NS_END