#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <Core/Misc/Utils/Color.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Maths/Vec4f_simd.hpp>
#include <RenderAPI/Common.hpp>

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
		BLENDING = GL_BLEND,
		RASTERIZER_DISCARD = GL_RASTERIZER_DISCARD
	};
}

namespace TestFunction
{
	enum test_function_t
	{
		NEVER = GL_NEVER,
		LESS = GL_LESS,
		EQUAL = GL_EQUAL,
		LESS_EQUAL = GL_LEQUAL,
		GREATER = GL_GREATER,
		NOT_EQUAL = GL_NOTEQUAL,
		GREATER_EQUAL = GL_GEQUAL,
		ALWAYS = GL_ALWAYS
	};

	// out put 3 bit code
	FORCEINLINE uint8 Encode(test_function_t test_func)
	{
		return test_func & 0x0007;
	}

	FORCEINLINE test_function_t Decode(uint8 code, test_function_t& func)
	{
		return (test_function_t) ((code & 0x07) | test_function_t::NEVER);
	}

	FORCEINLINE uint8 GetEncodingBits(){ return 3; }
}

namespace Stencil
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

namespace CullMode
{
	enum cull_mode_t
	{
		FRONT = GL_FRONT,
		BACK = GL_BACK,
		FRONT_AND_BACK = GL_FRONT_AND_BACK,
	};

	enum front_face_t
	{
		CW = GL_CW,
		CCW = GL_CCW,
	};
	// 1				1
	// ^frontfaec		^cullmode

	// out put 2 bit code
	FORCEINLINE uint8 Encode(front_face_t frontface, cull_mode_t cullmode)
	{
		uint8 front_face_bit = frontface == front_face_t::CCW ? 1 : 0;
		uint8 cullmode_bit = cullmode == FRONT ? 1 : 0;					// Its front otherwise its back
		return front_face_bit << 1 | cullmode_bit;
	}

	FORCEINLINE void Decode(uint8 code, front_face_t& frontface, cull_mode_t& cullmode)
	{
		uint8 real_code = code & 0x02;
		frontface = (real_code >> 1) ? front_face_t::CCW : front_face_t::CW;
		cullmode = (real_code & 0x01) ? cull_mode_t::FRONT : cull_mode_t::BACK;
	}

	FORCEINLINE uint8 GetEncodingBits(){ return 2; }
}

namespace Blending
{
	enum blend_equation_t {
		ADD = GL_FUNC_ADD,
		MIN = GL_MIN,
		MAX = GL_MAX,
		SUBTRACT  = GL_FUNC_SUBTRACT,
		REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT,
	};

	enum blend_func_t {
		ZERO = GL_ZERO,
		ONE = GL_ONE,
		SRC_COLOR = GL_SRC_COLOR,
		ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
		DST_COLOR = GL_DST_COLOR,
		ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
		SRC_ALPHA = GL_SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
		DST_ALPHA = GL_DST_ALPHA,
		ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
		CONSTANT_COLOR = GL_CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
		SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE,
		SRC1_COLOR = GL_SRC1_COLOR,
		ONE_MINUS_SRC1_COLOR = GL_ONE_MINUS_SRC1_COLOR,
		SRC1_ALPHA = GL_SRC1_ALPHA,
		ONE_MINUS_SRC1_ALPHA = GL_ONE_MINUS_SRC1_ALPHA,
	};

}

namespace PolygonMode
{
	enum polygon_mode_t
	{
		POINT = GL_POINT,
		LINE = GL_LINE,
		FILL = GL_FILL,
	};

	// out put 2 bit code
	FORCEINLINE uint8 Encode(polygon_mode_t mode)
	{
		return mode & 0x0003;
	}

	FORCEINLINE uint8 Decode(uint8 code)
	{
		return code | 0x1B00;
	}

	FORCEINLINE uint8 GetEncodingBits(){ return 2; }
}

FORCEINLINE static void Enable(Capability::capability_t capability)
{
	Call_GL(glEnable(capability));
}

FORCEINLINE static void Enable(int32 capability)
{
	Call_GL(glEnable(capability));
}

FORCEINLINE static void Disable(Capability::capability_t capability)
{
	Call_GL(glDisable(capability));
}

FORCEINLINE static void ClearColor(const Vec<4, float, SIMD>& col)
{
	auto r = col / 255.f;
	Call_GL(glClearColor(r.x, r.y, r.z, r.w));
}

/*FORCEINLINE void ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}*/

FORCEINLINE static void Clear(Buffer::buffer_t buffers = Buffer::COLOR | Buffer::DEPTH)
{
	Call_GL(glClear(buffers));
}

FORCEINLINE static void DepthMask(bool writeEnabled)
{
	Call_GL(glDepthMask(writeEnabled ? GL_TRUE : GL_FALSE));
}

FORCEINLINE static void StencilMask(bool writeEnabled)
{
	Call_GL(glStencilMask(writeEnabled ? ~0 : 0));
}

FORCEINLINE static void StencilMask(uint32 mask)
{
	Call_GL(glStencilMask(mask));
}

FORCEINLINE static void StencilFunc(TestFunction::test_function_t function, int32 reference, uint32 mask = ~0)
{
	Call_GL(glStencilFunc(function, reference, mask));
}

FORCEINLINE static void StencilOp(Stencil::stencil_action_t fail, Stencil::stencil_action_t zfail, Stencil::stencil_action_t pass)
{
	Call_GL(glStencilOp(fail, zfail, pass));
}

FORCEINLINE static void DrawArrays(Primitive::primitive_t mode, int32 start, int32 end)
{
	Call_GL(glDrawArrays(mode, start, end));
}

FORCEINLINE static void DrawElements(Primitive::primitive_t mode, DataType::data_type_t type, int32 count, intptr offset) // only unsigned int, unsinged char and unsigned short allowed here
{
	Call_GL(glDrawElements(mode, count, type, (const void*)offset));
}

FORCEINLINE static void ActivateTexture(uint8 i)
{
	glActiveTexture(GL_TEXTURE0 + i);
}

TRE_NS_END