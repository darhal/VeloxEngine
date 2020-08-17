#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Renderer/Core/RenderInstance/RenderInstance.hpp>
#include <Renderer/Core/RenderDevice/RenderDevice.hpp>
#include <Renderer/Core/RenderContext/RenderContext.hpp>

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

		void FlushTransfers();

		FORCEINLINE RenderInstance& GetRenderInstance() { return renderInstance; }
		FORCEINLINE RenderContext& GetRenderContext() { return renderContext; }
		FORCEINLINE RenderDevice& GerRenderDevice() { return renderDevice; }

		Internal::RenderContext& GetCtxInternal() { return renderContext.internal; }
		Internal::RenderDevice& GetDevInternal() { return renderDevice.internal; }
		Internal::RenderInstance& GetInstInternal() { return renderInstance.internal; }
	private:
		RenderInstance	renderInstance;
		RenderDevice	renderDevice;
		RenderContext	renderContext;
	};
};

TRE_NS_END