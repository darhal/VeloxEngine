#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderContext
	{
	public:
		RenderContext();

		void CreateRenderContext(TRE::Window* wnd, const Internal::RenderInstance& instance);

		void InitRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice);

		void DestroyRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice, Internal::RenderContext& renderContext);

	private:
		Internal::RenderContext	internal;

		friend class RenderEngine;
	};
}

TRE_NS_END