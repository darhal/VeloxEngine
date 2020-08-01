#pragma once

#include <Renderer/Common.hpp>

namespace Renderer
{
	struct RenderContext {
		VkInstance instance;

		// Debugging
		VkDebugUtilsMessengerEXT debugMessenger;
	};
};