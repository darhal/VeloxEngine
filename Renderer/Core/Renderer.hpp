#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Renderer/Core/Instance/Instance.hpp>
#include <Renderer/Core/RenderDevice/RenderDevice.hpp>
#include <Renderer/Core/Context/Context.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderEngine
	{
	public:
		RenderEngine(TRE::Window* wnd);

		~RenderEngine();

		void BeginFrame();

		void EndFrame();

		Internal::RenderContext& GetCtxInternal() { return renderContext.internal; }
		Internal::RenderDevice& GetDevInternal() { return renderDevice.internal; }
		Internal::RenderInstance& GetInstInternal() { return renderInstance.internal; }
	//private:
		// Internal::RenderEngine engine;

		RenderInstance	renderInstance;
		RenderDevice	renderDevice;
		RenderContext	renderContext;
	};
};

TRE_NS_END