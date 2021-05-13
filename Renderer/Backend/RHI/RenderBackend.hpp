#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>
#include <Renderer/Backend/RHI/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/RHI/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/RHI/RenderContext/RenderContext.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderBackend
	{
	public:
		RenderBackend(TRE::Window* wnd);

		~RenderBackend();

		void Shutdown();

		void InitInstance(uint32 usage = 0);

        // Frame managment:
		void BeginFrame();

		void EndFrame();

		// ..
		void SetSamplerCount(uint32 msaaSamplerCount = 1);

        // Getters:
        FORCEINLINE RenderInstance& GetRenderInstance() { return renderInstance; }
        FORCEINLINE RenderContext& GetRenderContext() { return renderContext; }
        FORCEINLINE RenderDevice& GetRenderDevice() { return renderDevice; }

        FORCEINLINE const RenderInstance& GetRenderInstance() const { return renderInstance; }
        FORCEINLINE const RenderContext& GetRenderContext() const { return renderContext; }
        FORCEINLINE const RenderDevice& GetRenderDevice() const { return renderDevice; }
	private:
		void ClearFrame();
	private:
		RenderInstance	renderInstance;
		RenderDevice	renderDevice;
		RenderContext	renderContext;
		TRE::Window*	window;

		uint32 msaaSamplerCount;
		uint32 enabledFeatures;
	};
};

TRE_NS_END
