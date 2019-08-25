#pragma once

#include "Core/Misc/Defines/Common.hpp"

TRE_NS_START

struct BackendDispatch
{
    static void Draw(const void* data);

    static void DrawIndexed(const void* data);

	static void CreateVAO(const void* data);

    static void CreateIndexBuffer(const void* data);
    
    static void CreateTexture(const void* data);

	static void CreateFrameBuffer(const void* data);

	static void CreateRenderBuffer(const void* data);
};


TRE_NS_END