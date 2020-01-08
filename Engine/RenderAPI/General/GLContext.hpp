#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <Core/Misc/Utils/Color.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Maths/Vec4f_simd.hpp>
#include <RenderAPI/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>

TRE_NS_START

CONSTEXPR uint32 g_GL_STATE_PARAMS[] =
{
	// PolygonMode : (Offset: 0)
	GL_POINT,
	GL_LINE,
	GL_FILL,

	// Cull Mode : (Offset: 3)
	GL_FRONT,
	GL_BACK,
	GL_FRONT_AND_BACK,

	// Front Face : (Offset: 6)
	GL_CW,
	GL_CCW,

	// Testing function : (Offset: 8)
	GL_NEVER,
	GL_LESS,
	GL_EQUAL,
	GL_LEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_ALWAYS,

	// Stencil Action :  (Offset: 16)
	GL_ZERO,
	GL_KEEP,
	GL_REPLACE,
	GL_INCR,
	GL_DECR,
	GL_INCR_WRAP,
	GL_DECR_WRAP,
	GL_INVERT,

	// Blending Equation : (Offset: 24)
	GL_FUNC_ADD,
	GL_MIN,
	GL_MAX,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,

	//  Blending function : (Offset: 29)
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA_SATURATE,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,
	GL_ONE_MINUS_CONSTANT_ALPHA,
	GL_SRC1_ALPHA,
	GL_SRC1_COLOR,
	GL_ONE_MINUS_SRC1_COLOR,
	GL_ONE_MINUS_SRC1_ALPHA,
};


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
		POINTS = GL_POINTS,
		LINES = GL_LINES,
		TRIANGLES = GL_TRIANGLES,
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
	CONSTEXPR uint8 ENCODING_BITS = 3;
	CONSTEXPR uint8 OFFSET = 8;

	enum test_function_t
	{
		NEVER = 0,
		LESS = 1,
		EQUAL = 2,
		LESS_EQUAL = 3,
		GREATER = 4,
		NOT_EQUAL = 5,
		GREATER_EQUAL = 6,
		ALWAYS = 7
	};

	// out put 3 bit code
	FORCEINLINE CONSTEXPR uint32 DecodeToGL(test_function_t test_func)
	{
		return g_GL_STATE_PARAMS[test_func + OFFSET];
	}

	FORCEINLINE test_function_t Decode(uint8 code)
	{
		return (test_function_t) (code & 0x7);
	}
}

namespace Stencil
{
	CONSTEXPR uint8 ENCODING_BITS = 3;
	CONSTEXPR uint8 OFFSET = 16;

	enum stencil_action_t
	{
		ZERO = 0,
		KEEP = 1,
		REPLACE = 2,
		INCREASE = 3,
		DECREASE = 4,
		INCREASE_WRAP = 5,
		DECREASE_WRAP = 6,
		INVERT = 7
	};

	FORCEINLINE CONSTEXPR uint32 DecodeToGL(stencil_action_t blend_func)
	{
		return g_GL_STATE_PARAMS[blend_func + OFFSET];
	}

	FORCEINLINE stencil_action_t Decode(uint8 code)
	{
		return (stencil_action_t)(code & 0x7);
	}

	namespace StencilMask
	{
		CONSTEXPR uint32 STENCIL_MASKS[] = { 0x00, 0x0f, 0xF0, 0xFF /*, custom masks*/ };
		
		enum stencil_mask_t
		{
			MASK1 = 0,
			MASK2 = 1,
			MASK3 = 2,
			MASK4 = 3,
			MASK5 = 4,
			MASK6 = 5,
			MASK7 = 6,
			MASK8 = 7,
		};

		FORCEINLINE CONSTEXPR uint32 DecodeToGL(stencil_mask_t blend_func)
		{
			return STENCIL_MASKS[blend_func];
		}

		FORCEINLINE stencil_mask_t Decode(uint8 code)
		{
			return (stencil_mask_t)(code & 0x7);
		}

		CONSTEXPR uint8 ENCODING_BITS = 3;
	}
}

namespace CullMode
{
	CONSTEXPR uint8 ENCODING_BITS = 2;
	CONSTEXPR uint8 OFFSET_BITS = 3;

	enum cull_mode_t
	{
		FRONT = 0,
		BACK = 1,
		FRONT_AND_BACK = 2,
	};

	enum front_face_t
	{
		CW = 3,
		CCW = 4,
	};
	// 1				1
	// ^frontface		^cullmode

	// out put 2 bit code
	FORCEINLINE uint8 Encode(front_face_t frontface, cull_mode_t cullmode)
	{
		uint8 front_face_bit = frontface == front_face_t::CCW ? 1 : 0;
		uint8 cullmode_bit = cullmode == FRONT ? 1 : 0;					// Its front otherwise its back
		return front_face_bit << 1 | cullmode_bit;
	}

	FORCEINLINE void Decode(uint8 code, front_face_t& frontface, cull_mode_t& cullmode)
	{
		uint8 real_code = code & 0x3;
		frontface = (real_code >> 1) ? front_face_t::CCW : front_face_t::CW;
		cullmode = (real_code & 0x1) ? cull_mode_t::FRONT : cull_mode_t::BACK;
	}
}

namespace Blending
{
	CONSTEXPR uint8 ENCODING_BITS_EQ = 3;
	CONSTEXPR uint8 ENCODING_BITS_FUNC = 5;
	CONSTEXPR uint8 OFFSET_EQ = 24;
	CONSTEXPR uint8 OFFSET_FUNC = 29;

	enum blend_equation_t {
		ADD = 0,
		MIN = 1,
		MAX = 2,
		SUBTRACT  = 3,
		REVERSE_SUBTRACT = 4,
	};

	enum blend_func_t {
		ZERO = 0,
		ONE = 1,

		SRC_COLOR = 2,
		ONE_MINUS_SRC_COLOR = 3,
		SRC_ALPHA = 4,
		ONE_MINUS_SRC_ALPHA = 5,
		DST_ALPHA = 6,
		ONE_MINUS_DST_ALPHA = 7,
		DST_COLOR = 8,
		ONE_MINUS_DST_COLOR = 9,
		SRC_ALPHA_SATURATE = 10,

		CONSTANT_COLOR = 11,
		ONE_MINUS_CONSTANT_COLOR = 12,
		CONSTANT_ALPHA = 13,
		ONE_MINUS_CONSTANT_ALPHA = 14,

		SRC1_ALPHA = 15,

		SRC1_COLOR = 16,
		ONE_MINUS_SRC1_COLOR = 17,
		ONE_MINUS_SRC1_ALPHA = 18,
	};

	FORCEINLINE CONSTEXPR uint32 DecodeToGL(blend_equation_t blend_equation)
	{
		return g_GL_STATE_PARAMS[blend_equation + OFFSET_EQ];
	}

	FORCEINLINE CONSTEXPR uint32 DecodeToGL(blend_func_t blend_func)
	{
		return g_GL_STATE_PARAMS[blend_func + OFFSET_FUNC];
	}

	FORCEINLINE blend_equation_t Decode(uint8 code)
	{
		return (blend_equation_t) (code & 0x7);
	}

	FORCEINLINE blend_func_t DecodeFunc(uint8 code)
	{
		return (blend_func_t)(code & 0x1F);
	}
}

namespace PolygonMode
{
	CONSTEXPR uint8 ENCODING_BITS = 2;
	CONSTEXPR uint8 OFFSET = 0;

	enum polygon_mode_t
	{
		POINT = 0,
		LINE = 1,
		FILL = 2,
	};

	// out put 2 bit code
	FORCEINLINE CONSTEXPR uint8 DecodeToGL(polygon_mode_t mode)
	{
		return g_GL_STATE_PARAMS[mode + OFFSET];
	}

	FORCEINLINE polygon_mode_t Decode(uint8 code)
	{
		return (polygon_mode_t) (code & 0x3);
	}
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

FORCEINLINE static void ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

FORCEINLINE static void Clear(Buffer::buffer_t buffers = Buffer::COLOR | Buffer::DEPTH)
{
	Call_GL(glClear(buffers));
}

FORCEINLINE static void ClearBuffers(Buffer::buffer_t buffers = Buffer::COLOR | Buffer::DEPTH)
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

FORCEINLINE static void StencilMask(Stencil::StencilMask::stencil_mask_t mask)
{
	Call_GL(glStencilMask(Stencil::StencilMask::DecodeToGL(mask)));
}

FORCEINLINE static void StencilFunc(TestFunction::test_function_t function, int32 reference, uint32 mask = ~0)
{
	Call_GL(glStencilFunc(TestFunction::DecodeToGL(function), reference, mask));
}

FORCEINLINE static void StencilOp(Stencil::stencil_action_t fail, Stencil::stencil_action_t zfail, Stencil::stencil_action_t pass)
{
	Call_GL(glStencilOp(Stencil::DecodeToGL(fail), Stencil::DecodeToGL(zfail), Stencil::DecodeToGL(pass)));
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