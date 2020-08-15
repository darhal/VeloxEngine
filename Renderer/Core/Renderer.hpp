#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>


TRE_NS_START

namespace Renderer
{
	RENDERER_API int32 Init(RenderEngine& engine, TRE::Window* wnd);

	RENDERER_API void Destrory(RenderEngine& engine);

	/*class RENDERER_API RenderEngine
	{
	public:
		RenderEngine();

		~RenderEngine();
	private:
		Internal::RenderEngine renderEngine;
	};*/
};

TRE_NS_END