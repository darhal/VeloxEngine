#pragma once

#include "Core/Misc/Defines/Common.hpp"

TRE_NS_START

struct BackendDispatch
{
    static void Draw(const void* data);

    static void DrawIndexed(const void* data);

    static void GenerateVAO(const void* data);

    static void GenerateVAOFromVertexData(const void* data);

    static void GenerateIndex(const void* data);
    
    static void CreateTexture(const void* data);

	static void CreateFrameBuffer(const void* data);

	static void CreateRenderBuffer(const void* data);
};


TRE_NS_END