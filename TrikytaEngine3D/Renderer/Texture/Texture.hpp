#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <initializer_list>

TRE_NS_START

class Color;

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
		BGRA = GL_BGRA
	};
}

struct TexConfig
{
	TexParam::tex_param_t param;
	int32 val;
	TexConfig(TexParam::tex_param_t p, uint32 v) : param(p), val(v)
	{}
	TexConfig(TexParam::tex_param_t p, TexWrapping::tex_wrapping_t v) : param(p), val((int32)v)
	{}
	TexConfig(TexParam::tex_param_t p, TexFilter::tex_filter_t v) : param(p), val((int32)v)
	{}
	TexConfig(int32 p, int32 v) : param(TexParam::tex_param_t(p)), val(v)
	{}
	~TexConfig() {}
};

class Image;

class Texture
{
public:
public:
	Texture(
		const char* path, TexTarget::tex_target_t target, 
		std::initializer_list<TexConfig> paramList = {},
		DataType::data_type_t datatype = DataType::UBYTE,
		TexInternalFormat::tex_internal_format_t internalFormat = TexInternalFormat::RGBA,
		TexFormat::tex_format_t format = TexFormat::RGBA
	);

	Texture();

	~Texture();
	
	void SetTextureTarget(TexTarget::tex_target_t target);
	void LoadImg(const Image& img,
		DataType::data_type_t datatype = DataType::UBYTE,
		TexInternalFormat::tex_internal_format_t internalFormat = TexInternalFormat::RGBA,
		TexFormat::tex_format_t format = TexFormat::RGBA
	);

	void EmptyTexture(uint32 width, uint32 height, DataType::data_type_t datatype = DataType::UBYTE,
		TexInternalFormat::tex_internal_format_t internalFormat = TexInternalFormat::RGBA,
		TexFormat::tex_format_t format = TexFormat::RGBA);

	void SetWrapping(TexParam::tex_param_t p, TexWrapping::tex_wrapping_t s);
	void SetFilters(TexParam::tex_param_t p, TexFilter::tex_filter_t filter);
	void SetBorderColor(const Color& color);

	void GenerateMipmaps();

	FORCEINLINE operator uint32() const {return m_texID;}
	FORCEINLINE const uint32 GetID() const { return m_texID; }
private:
	uint32 m_texID;
	TexTarget::tex_target_t m_target;
};

TRE_NS_END