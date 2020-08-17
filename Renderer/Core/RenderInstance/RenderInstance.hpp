#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Engine/Core/DataStructure/Array/Array.hpp>
#include <initializer_list>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderInstance
	{
	public:
		RenderInstance();

		~RenderInstance();

		int32 CreateRenderInstance();

		void DestroyRenderInstance();

	private:
		static int32 CreateInstance(VkInstance* p_instance);

		static void DestroyInstance(VkInstance p_instance);

		// DEBUGGING SECTION // 

		static int32 SetupDebugMessenger(VkInstance p_instance, VkDebugUtilsMessengerEXT* p_debugMessenger);

		static int32 InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	private:
		Internal::RenderInstance internal;

		friend class RenderEngine;
	};
};

TRE_NS_END