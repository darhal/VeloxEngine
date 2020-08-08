#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	void CreateRenderContext(RenderContext& ctx, TRE::Window* wnd, const RenderInstance& instance);

	void InitRenderContext(RenderContext& ctx, const RenderInstance& renderInstance, const RenderDevice& renderDevice);

	void DestroyRenderContext(const RenderEngine& engine);
}

TRE_NS_END