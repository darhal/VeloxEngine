#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	void CreateRenderContext(RenderContext& ctx, const RenderInstance& instance);

	void InitRenderContext(RenderContext& ctx, const RenderInstance& renderInstance, const RenderDevice& renderDevice);

	void DestroyContext(RenderContext& p_ctx);
}

TRE_NS_END