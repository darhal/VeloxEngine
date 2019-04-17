#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <Core/Misc/Utils/Color.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Maths/Vec4f_simd.hpp>

TRE_NS_START

namespace Buffer
{
	enum buffer_t
	{
		COLOR = GL_COLOR_BUFFER_BIT,
		DEPTH = GL_DEPTH_BUFFER_BIT,
		STENCIL = GL_STENCIL_BUFFER_BIT
	};

	inline buffer_t operator|(buffer_t lft, buffer_t rht)
	{
		return (buffer_t)((int)lft | (int)rht);
	}
}

namespace Primitive
{
	enum primitive_t
	{
		TRIANGLES = GL_TRIANGLES,
		LINES = GL_LINES,
		POINTS = GL_POINTS,
	};
}

namespace Capability
{
	enum capability_t
	{
		DEPTH_TEST = GL_DEPTH_TEST,
		STENCIL_TEST = GL_STENCIL_TEST,
		CULL_FACE = GL_CULL_FACE,
		RASTERIZER_DISCARD = GL_RASTERIZER_DISCARD
	};
}

namespace TestFunction
{
	enum test_function_t
	{
		NEVER = GL_NEVER,
		LESS = GL_LESS,
		LESS_EQUAL = GL_LEQUAL,
		GREATER = GL_GREATER,
		GREATER_EQUAL = GL_GEQUAL,
		EQUAL = GL_EQUAL,
		NOT_EQUAL = GL_NOTEQUAL,
		ALWAYS = GL_ALWAYS
	};
}

namespace StencilAction
{
	enum stencil_action_t
	{
		KEEP = GL_KEEP,
		ZERO = GL_ZERO,
		REPLACE = GL_REPLACE,
		INCREASE = GL_INCR,
		INCREASE_WRAP = GL_INCR_WRAP,
		DECREASE = GL_DECR,
		DECREASE_WRAP = GL_DECR_WRAP,
		INVERT = GL_INVERT
	};
}

FORCEINLINE static void Enable(Capability::capability_t capability)
{
	glEnable(capability);
}

FORCEINLINE static void Disable(Capability::capability_t capability)
{
	glDisable(capability);
}

FORCEINLINE static void ClearColor(const Vec<4, float, SIMD>& col)
{
	auto r = col / 255.f;
	glClearColor(r.x, r.y, r.z, r.w);
}

/*FORCEINLINE void ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}*/

FORCEINLINE static void Clear(Buffer::buffer_t buffers = Buffer::COLOR | Buffer::DEPTH)
{
	glClear(buffers);
}

FORCEINLINE static void DepthMask(bool writeEnabled)
{
	glDepthMask(writeEnabled ? GL_TRUE : GL_FALSE);
}

FORCEINLINE static void StencilMask(bool writeEnabled)
{
	glStencilMask(writeEnabled ? ~0 : 0);
}

FORCEINLINE static void StencilMask(uint32 mask)
{
	glStencilMask(mask);
}

FORCEINLINE static void StencilFunc(TestFunction::test_function_t function, int reference, uint32 mask = ~0)
{
	glStencilFunc(function, reference, mask);
}

FORCEINLINE static void StencilOp(StencilAction::stencil_action_t fail, StencilAction::stencil_action_t zfail, StencilAction::stencil_action_t pass)
{
	glStencilOp(fail, zfail, pass);
}

FORCEINLINE static void DrawArrays(Primitive::primitive_t mode, int32 start, int32 end)
{
	glDrawArrays(mode, start, end);
}

FORCEINLINE static void DrawElements(Primitive::primitive_t mode, DataType::data_type_t type, uint32 count, intptr offset) // only unsigned int, unsinged char and unsigned short allowed here
{
	glDrawElements(mode, count, type, (const void*)offset);
}

FORCEINLINE static void ActivateTexture(uint8 i)
{
	glActiveTexture(GL_TEXTURE0 + i);
}

TRE_NS_END