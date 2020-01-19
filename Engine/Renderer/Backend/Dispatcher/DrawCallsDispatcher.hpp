#pragma once

#include "Core/Misc/Defines/Common.hpp"

TRE_NS_START

struct BackendDispatch
{
	// Single Draw Commands:
	static void PreDrawCall(const void* data);

    static void Draw(const void* data);

    static void DrawIndexed(const void* data);

	// Instance Draw Commands:
	static void InstancedPreDrawCall(const void* data);

	static void InstancedDraw(const void* data);

	static void InstancedDrawIndexed(const void* data);

	// Resources Creation Commands:
	static void CreateVAO(const void* data);

    static void CreateIndexBuffer(const void* data);

	static void CreateVBO(const void* data);
    
    static void CreateTexture(const void* data);

	static void CreateFrameBuffer(const void* data);

	static void CreateRenderBuffer(const void* data);

	// Misc commands:
	static void EditSubBuffer(const void* data);

	static void DispatchCompute(const void* data);

	static void UploadUniforms(const void* data);

	static void BindBufferRange(const void* data);
};


TRE_NS_END