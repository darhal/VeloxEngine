#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/StagingManager/StagingManager.hpp>

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

		const Internal::ContextFrameResources& GetFrameResource(uint32 i) const;

		Internal::ContextFrameResources& GetFrameResource(uint32 i);

		Internal::ContextFrameResources& GetCurrentFrameResource();

		const Internal::ContextFrameResources& GetCurrentFrameResource() const;

		const Internal::SwapChainData& GetSwapChainData() const;

		FORCEINLINE uint32 GetImagesCount() const { return internal.imagesCount; }

		FORCEINLINE uint32 GetCurrentImageIndex() const { return internal.currentImage; }

		FORCEINLINE uint32 GetCurrentFrame() const { return internal.currentFrame; }
	private:
		void BeginFrame(Internal::RenderDevice& renderDevice, StagingManager& stagingManager);

		void EndFrame(Internal::RenderDevice& renderDevice);
	private:
		Internal::RenderContext	internal;

		friend class RenderBackend;
	};
}


TRE_NS_END