#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	void InitContext(RenderContext& p_ctx);

	int32 CreateDevice(RenderContext& p_ctx);

	void DestroyContext(RenderContext& p_ctx);
}

TRE_NS_END