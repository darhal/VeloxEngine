#pragma once

#include <Core/Context/GLDefines.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/Utils/Logging.hpp>

#if not defined(_DEBUG) || defined(NDEBUG)

	#define Call_GL(GL_CALL_FUNCTION) \
		GL_CALL_FUNCTION \

#else

	#include <Core/DataStructure/String/String.hpp>

	#define TO_STR(x) #x

	#define Call_GL(GL_CALL_FUNCTION) \
		GL_CALL_FUNCTION; \
		{ \
			GLenum err; \
   	 		while ((err = glGetError()) != GL_NO_ERROR) { \
				String error; \
                switch(err) { \
                    case GL_INVALID_OPERATION:      error = String("INVALID_OPERATION");      break; \
                    case GL_INVALID_ENUM:           error = String("INVALID_ENUM");           break; \
                    case GL_INVALID_VALUE:          error = String("INVALID_VALUE");          break; \
                    case GL_OUT_OF_MEMORY:          error = String("OUT_OF_MEMORY");          break; \
                    case GL_INVALID_FRAMEBUFFER_OPERATION:  error = String("INVALID_FRAMEBUFFER_OPERATION");  break; \
                } \
        		Log::Write(Log::ERR, "(OPEN GL ERROR) {%s} Occured at (%s:%d) after attempting to call %s", error.Buffer(), __FILENAME__, __LINE__, TO_STR(GL_CALL_FUNCTION)); \
    		} \
		} \

#endif

TRE_NS_START

/*******************************************************************************/
/****************************** GENERAL ENUM ***********************************/
/*******************************************************************************/

namespace DataType
{
	enum data_type_t
	{
		BYTE = GL_BYTE,
		UBYTE = GL_UNSIGNED_BYTE,
		SHORT = GL_SHORT,
		USHORT = GL_UNSIGNED_SHORT,
		INT = GL_INT,
		UINT = GL_UNSIGNED_INT,
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE,

		UBYTE332 = GL_UNSIGNED_BYTE_3_3_2,
		UBYTE233REV = GL_UNSIGNED_BYTE_2_3_3_REV,
		USHORT565 = GL_UNSIGNED_SHORT_5_6_5,
		USHORT565REV = GL_UNSIGNED_SHORT_5_6_5,
		USHORT4444 = GL_UNSIGNED_SHORT_4_4_4_4,
		USHORT4444REV = GL_UNSIGNED_SHORT_4_4_4_4_REV,
		USHORT5551 = GL_UNSIGNED_SHORT_5_5_5_1,
		USHORT1555REV = GL_UNSIGNED_SHORT_1_5_5_5_REV,
		UINT8888 = GL_UNSIGNED_INT_8_8_8_8,
		UINT8888REV = GL_UNSIGNED_INT_8_8_8_8_REV,
		UINT101010102 = GL_UNSIGNED_INT_10_10_10_2
	};
}

namespace TargetType
{
	enum target_type_t {
		NONE = 0,

		// VAO:
		VAO = 1,

		// Shader:
		SHADER = 2,

		// Buffers:
		ARRAY_BUFFER = GL_ARRAY_BUFFER,
		ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,
		COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
		COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
		DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,
		DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER,
		ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
		PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
		PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
		QUERY_BUFFER = GL_QUERY_BUFFER,
		SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
		TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
		TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
		UNIFORM_BUFFER = GL_UNIFORM_BUFFER,

		// Textures:
		TEX2D = GL_TEXTURE_2D,
		TEX1D = GL_TEXTURE_1D,
		TEX3D = GL_TEXTURE_3D,
		TEX1D_ARRAY = GL_TEXTURE_1D_ARRAY,
		TEX2D_ARRAY = GL_TEXTURE_2D_ARRAY,
		TEX_RECTANGLE = GL_TEXTURE_RECTANGLE,
		TEX_CUBE = GL_TEXTURE_CUBE_MAP,
		TEX_CUBE_MAP = GL_TEXTURE_CUBE_MAP_ARRAY,
		TEX_BUFFER = GL_TEXTURE_BUFFER,
		TEX2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
		TEX2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,

		// Framebuffers:
		FBO_DRAW = GL_DRAW_FRAMEBUFFER,
		FBO_READ = GL_READ_FRAMEBUFFER,
		FBO = GL_FRAMEBUFFER,

		// Renderbuffers:
		RBO = GL_RENDERBUFFER,
	};
}

/*******************************************************************************/
/******************************** FBO ENUM *************************************/
/*******************************************************************************/

namespace FBOTarget
{
	enum framebuffer_target_t {
		FBO_DRAW = GL_DRAW_FRAMEBUFFER,
		FBO_READ = GL_READ_FRAMEBUFFER,
		FBO = GL_FRAMEBUFFER
	};
};

namespace FBOAttachement
{
	enum framebuffer_attachement_t {
		DEPTH_ATTACH = GL_DEPTH_ATTACHMENT,
		STENCIL_ATTACH = GL_STENCIL_ATTACHMENT,
		DEPTH_STENCIL_ATTACH = GL_DEPTH_STENCIL_ATTACHMENT,
		COLOR_ATTACH = GL_COLOR_ATTACHMENT0,
		NONE = 0,
	};
}

namespace FBOTexTarget
{
	enum framebuffer_tex_target_t {
		TEX2D = GL_TEXTURE_2D,
		TEX_CUBE_PX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		TEX_CUBE_NX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		TEX_CUBE_PY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		TEX_CUBE_NY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		TEX_CUBE_PZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		TEX_CUBE_NZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
}

namespace FBOColourBuffer
{
	enum colour_buffer_t {
		NONE = GL_NONE,
		FRONT_LEFT = GL_FRONT_LEFT,
		FRONT_RIGHT = GL_FRONT_RIGHT,
		BACK_LEFT = GL_BACK_LEFT,
		BACK_RIGHT = GL_BACK_RIGHT,
		FRONT = GL_FRONT,
		BACK = GL_BACK,
		LEFT = GL_LEFT,
		RIGHT = GL_RIGHT,
		FRONT_AND_BACK = GL_FRONT_AND_BACK,
	};
}

/*******************************************************************************/
/******************************** RBO ENUM *************************************/
/*******************************************************************************/

namespace RBOInternal
{
	enum rbo_internal_format_t {
		DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8
	};
}

/*******************************************************************************/
/******************************** VBO ENUM *************************************/
/*******************************************************************************/

namespace BufferTarget
{
	enum buffer_target_t {
		ARRAY_BUFFER = GL_ARRAY_BUFFER,
		ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,
		COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
		COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
		DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,
		DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER,
		ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
		PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
		PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
		QUERY_BUFFER = GL_QUERY_BUFFER,
		SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
		TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
		TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
		UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
		UNKNOWN = 0,
	};
}

namespace BufferUsage
{
	enum buffer_usage_t {
		STREAM_DRAW = GL_STREAM_DRAW,
		STREAM_READ = GL_STREAM_READ,
		STREAM_COPY = GL_STREAM_COPY,
		STATIC_DRAW = GL_STATIC_DRAW,
		STATIC_READ = GL_STATIC_READ,
		STATIC_COPY = GL_STATIC_COPY,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		DYNAMIC_READ = GL_DYNAMIC_READ,
		DYNAMIC_COPY = GL_DYNAMIC_COPY
	};
}

/*******************************************************************************/
/****************************** Texture ENUM ***********************************/
/*******************************************************************************/

namespace TexTarget
{
	enum tex_target_t {
		TEX2D = GL_TEXTURE_2D,
		TEX1D = GL_TEXTURE_1D,
		TEX3D = GL_TEXTURE_3D,
		TEX1D_ARRAY = GL_TEXTURE_1D_ARRAY,
		TEX2D_ARRAY = GL_TEXTURE_2D_ARRAY,
		TEX_RECTANGLE = GL_TEXTURE_RECTANGLE,
		TEX_CUBE = GL_TEXTURE_CUBE_MAP,
		TEX_CUBE_MAP = GL_TEXTURE_CUBE_MAP_ARRAY,
		TEX_BUFFER = GL_TEXTURE_BUFFER,
		TEX2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
		TEX2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	};
}

namespace TexParam
{
	enum tex_param_t
	{
		DEPTH_STENCIL_TEX_MODE = GL_DEPTH_STENCIL_TEXTURE_MODE,
		TEX_BASE_LEVEL = GL_TEXTURE_BASE_LEVEL,
		TEX_COMPARE_FUNC = GL_TEXTURE_COMPARE_FUNC,
		TEX_COMPARE_MODE = GL_TEXTURE_COMPARE_MODE,
		TEX_LOD_BIAS = GL_TEXTURE_LOD_BIAS,
		TEX_MIN_FILTER = GL_TEXTURE_MIN_FILTER,
		TEX_MAG_FILTER = GL_TEXTURE_MAG_FILTER,
		TEX_MIN_LOD = GL_TEXTURE_MIN_LOD,
		TEX_MAX_LOD = GL_TEXTURE_MAX_LOD,
		TEX_MAX_LEVEL = GL_TEXTURE_MAX_LEVEL,
		TEX_SWIZZLE_R = GL_TEXTURE_SWIZZLE_R,
		TEX_SWIZZLE_G = GL_TEXTURE_SWIZZLE_G,
		TEX_SWIZZLE_B = GL_TEXTURE_SWIZZLE_B,
		TEX_SWIZZLE_A = GL_TEXTURE_SWIZZLE_A,
		TEX_WRAP_S = GL_TEXTURE_WRAP_S,
		TEX_WRAP_T = GL_TEXTURE_WRAP_T,
		TEX_WRAP_R = GL_TEXTURE_WRAP_R
	};
}


namespace TexWrapping
{
	enum tex_wrapping_t
	{
		CLAMP_EDGE = GL_CLAMP_TO_EDGE,
		CLAMP_BORDER = GL_CLAMP_TO_BORDER,
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT
	};
}

namespace TexFilter
{
	enum tex_filter_t
	{
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
	};
}
namespace TexInternalFormat
{
	enum tex_internal_format_t
	{
		CompressedRed = GL_COMPRESSED_RED,
		CompressedRedRGTC1 = GL_COMPRESSED_RED_RGTC1,
		CompressedRG = GL_COMPRESSED_RG,
		CompressedRGB = GL_COMPRESSED_RGB,
		CompressedRGBA = GL_COMPRESSED_RGBA,
		CompressedRGRGTC2 = GL_COMPRESSED_RG_RGTC2,
		CompressedSignedRedRGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
		CompressedSignedRGRGTC2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
		CompressedSRGB = GL_COMPRESSED_SRGB,
		DepthStencil = GL_DEPTH_STENCIL,
		Depth24Stencil8 = GL_DEPTH24_STENCIL8,
		Depth32FStencil8 = GL_DEPTH32F_STENCIL8,
		DepthComponent = GL_DEPTH_COMPONENT,
		DepthComponent16 = GL_DEPTH_COMPONENT16,
		DepthComponent24 = GL_DEPTH_COMPONENT24,
		DepthComponent32F = GL_DEPTH_COMPONENT32F,
		R16F = GL_R16F,
		R16I = GL_R16I,
		R16SNorm = GL_R16_SNORM,
		R16UI = GL_R16UI,
		R32F = GL_R32F,
		R32I = GL_R32I,
		R32UI = GL_R32UI,
		R3G3B2 = GL_R3_G3_B2,
		R8 = GL_R8,
		R8I = GL_R8I,
		R8SNorm = GL_R8_SNORM,
		R8UI = GL_R8UI,
		Red = GL_RED,
		RG = GL_RG,
		RG16 = GL_RG16,
		RG16F = GL_RG16F,
		RG16SNorm = GL_RG16_SNORM,
		RG32F = GL_RG32F,
		RG32I = GL_RG32I,
		RG32UI = GL_RG32UI,
		RG8 = GL_RG8,
		RG8I = GL_RG8I,
		RG8SNorm = GL_RG8_SNORM,
		RG8UI = GL_RG8UI,
		RGB = GL_RGB,
		RGB10 = GL_RGB10,
		RGB10A2 = GL_RGB10_A2,
		RGB12 = GL_RGB12,
		RGB16 = GL_RGB16,
		RGB16F = GL_RGB16F,
		RGB16I = GL_RGB16I,
		RGB16UI = GL_RGB16UI,
		RGB32F = GL_RGB32F,
		RGB32I = GL_RGB32I,
		RGB32UI = GL_RGB32UI,
		RGB4 = GL_RGB4,
		RGB5 = GL_RGB5,
		RGB5A1 = GL_RGB5_A1,
		RGB8 = GL_RGB8,
		RGB8I = GL_RGB8I,
		RGB8UI = GL_RGB8UI,
		RGB9E5 = GL_RGB9_E5,
		RGBA = GL_RGBA,
		RGBA12 = GL_RGBA12,
		RGBA16 = GL_RGBA16,
		RGBA16F = GL_RGBA16F,
		RGBA16I = GL_RGBA16I,
		RGBA16UI = GL_RGBA16UI,
		RGBA2 = GL_RGBA2,
		RGBA32F = GL_RGBA32F,
		RGBA32I = GL_RGBA32I,
		RGBA32UI = GL_RGBA32UI,
		RGBA4 = GL_RGBA4,
		RGBA8 = GL_RGBA8,
		RGBA8UI = GL_RGBA8UI,
		SRGB8 = GL_SRGB8,
		SRGB8A8 = GL_SRGB8_ALPHA8,
		SRGBA = GL_SRGB_ALPHA
	};
}

namespace TexFormat
{
	enum tex_format_t
	{
		RED = GL_RED,
		RGB = GL_RGB,
		BGR = GL_BGR,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA,
		DepthComponent = GL_DEPTH_COMPONENT,
	};
}


/*******************************************************************************/
/****************************** Shader ENUM ************************************/
/*******************************************************************************/

namespace ShaderType {
	enum shader_type_t
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};

	static FORCEINLINE const char* ToString(shader_type_t v) {
		static const char* shadertype2str[] = { "VERTEX SHADER", " FRAGMENT SHADER", "GEOMETRY SHADER", "COMPUTE SHADER" };
		switch (v) {
		case shader_type_t::VERTEX:
			return shadertype2str[0];
		case shader_type_t::FRAGMENT:
			return shadertype2str[1];
		case shader_type_t::GEOMETRY:
			return shadertype2str[2];
		case shader_type_t::COMPUTE:
			return shadertype2str[3];
		default:
			return "Unknown";
		}
	}
}


/*******************************************************************************/
/***************************** CLASS DECLARATION *******************************/
/*******************************************************************************/

class VAO;
class VBO;
class FBO;
class RBO;
class Texture;
class Shader;
class ShaderProgram;

TRE_NS_END