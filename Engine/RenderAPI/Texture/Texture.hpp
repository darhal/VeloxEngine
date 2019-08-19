#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Utils/Color.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>

#include <RenderAPI/Common.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>

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

struct TexParamConfig
{
	TexParam::tex_param_t param;
	int32 val;
	TexParamConfig(TexParam::tex_param_t p, uint32 v) : param(p), val(v)
	{}
	TexParamConfig(TexParam::tex_param_t p, TexWrapping::tex_wrapping_t v) : param(p), val((int32)v)
	{}
	TexParamConfig(TexParam::tex_param_t p, TexFilter::tex_filter_t v) : param(p), val((int32)v)
	{}
	TexParamConfig(int32 p, int32 v) : param(TexParam::tex_param_t(p)), val(v)
	{}

	TexParamConfig(const TexParamConfig& other) = default;
	TexParamConfig(TexParamConfig&& other) = default;

	TexParamConfig& operator=(const TexParamConfig& other) = default;
	TexParamConfig& operator=(TexParamConfig&& other) = default;
};

struct TextureSettings
{
	TextureSettings(TexTarget::tex_target_t target = TexTarget::TEX2D,
		uint32 width = 0, uint32 height = 0, void* data = NULL,
		const Vector<TexParamConfig>& paramList = {},
		DataType::data_type_t datatype = DataType::UBYTE, int32 lod = 0,
		TexInternalFormat::tex_internal_format_t internalFormat = TexInternalFormat::RGBA,
		TexFormat::tex_format_t format = TexFormat::RGBA) : 
		paramList(std::move(paramList)), img_data(data), width(width), height(height), lod(lod), 
		target(target), datatype(datatype), internalFormat(internalFormat), format(format)
	{}

	Vector<TexParamConfig> paramList = {};
	void* img_data = NULL;
	int32 width, height, lod = 0;
	TexTarget::tex_target_t target = TexTarget::TEX2D;
	DataType::data_type_t datatype = DataType::UBYTE;
	TexInternalFormat::tex_internal_format_t internalFormat = TexInternalFormat::RGBA;
	TexFormat::tex_format_t format = TexFormat::RGBA;

	~TextureSettings() {}
};

class Image;

class Texture
{
public:
public:
	Texture(
		const char* path, TexTarget::tex_target_t target,
		std::initializer_list<TexParamConfig> paramList = {},
		DataType::data_type_t datatype = DataType::UBYTE,
		TexInternalFormat::tex_internal_format_t internalFormat = TexInternalFormat::RGBA,
		TexFormat::tex_format_t format = TexFormat::RGBA
	);

	Texture(const char* path, const TextureSettings& settings);

	Texture(const TextureSettings& settings);

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

	uint32 Generate(const TextureSettings& settings);

	FORCEINLINE operator uint32() const { return m_ID; }
	FORCEINLINE const uint32 GetID() const { return m_ID; }
	FORCEINLINE const TargetType::target_type_t GetBindingTarget() const {  return (TargetType::target_type_t)m_target;	}

	FORCEINLINE void Bind() const;
	FORCEINLINE void Use() const;

	FORCEINLINE void Unbind() const;
	FORCEINLINE void Unuse() const;

	FORCEINLINE void Clean();

	FORCEINLINE uint32 Generate();

	FORCEINLINE void Invalidate() { m_ID = 0; }

	explicit FORCEINLINE Texture(const Texture& other) = delete;
	FORCEINLINE Texture& operator=(const Texture& other) = delete;

	explicit FORCEINLINE Texture(Texture&& other);
	FORCEINLINE Texture& operator=(Texture&& other);
private:
	uint32 m_ID;
	TexTarget::tex_target_t m_target;
};

FORCEINLINE Texture::Texture() : m_ID(0), m_target(TexTarget::TEX2D)
{
}

FORCEINLINE uint32 Texture::Generate()
{
	Call_GL(glGenTextures(1, &m_ID));
	return m_ID;
}

FORCEINLINE void Texture::GenerateMipmaps()
{
	this->Use(); //glBindTexture(target, m_ID);
	Call_GL(glGenerateMipmap(m_target));
}


FORCEINLINE void Texture::SetTextureTarget(TexTarget::tex_target_t target)
{
	m_target = target;
	this->Use(); //glBindTexture(target, m_ID);
}

FORCEINLINE void Texture::EmptyTexture(uint32 width, uint32 height, DataType::data_type_t datatype, TexInternalFormat::tex_internal_format_t internalFormat, TexFormat::tex_format_t format)
{
	this->Use(); //glBindTexture(target, m_ID);
	Call_GL(glTexImage2D(m_target, 0, internalFormat, width, height, 0, format, datatype, NULL));
}

FORCEINLINE void Texture::SetWrapping(TexParam::tex_param_t p, TexWrapping::tex_wrapping_t s)
{
	this->Use(); //glBindTexture(target, m_ID);glBindTexture(m_target, m_ID);
	Call_GL(glTexParameteri(m_target, p, s));
}

FORCEINLINE void Texture::SetFilters(TexParam::tex_param_t p, TexFilter::tex_filter_t filter)
{
	Call_GL(glTexParameteri(m_target, p, filter));
}

FORCEINLINE void Texture::SetBorderColor(const Color& color)
{
	this->Use(); //glBindTexture(target, m_ID);
	float col[4] = { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
	Call_GL(glTexParameterfv(m_target, GL_TEXTURE_BORDER_COLOR, col));
}
 
FORCEINLINE void Texture::Bind() const
{
	Call_GL(glBindTexture(m_target, m_ID));
}

FORCEINLINE void Texture::Use() const
{
	GLState::Bind(this);
}

FORCEINLINE void Texture::Unbind() const
{
	Call_GL(glBindTexture(m_target, 0));
}

FORCEINLINE void Texture::Unuse() const
{
	GLState::Unbind(this);
}

FORCEINLINE Texture::~Texture()
{
	if (m_ID) {
		Clean();
		m_ID = 0;
	}
}

FORCEINLINE void Texture::Clean()
{
	Call_GL(glDeleteTextures(1, &m_ID));
}

FORCEINLINE Texture::Texture(Texture&& other) :
	m_ID(other.m_ID), m_target(other.m_target)
{
	other.m_ID = 0; //  the element copied from becomes invalid !
}

FORCEINLINE Texture& Texture::operator=(Texture&& other)
{
	m_ID = other.m_ID;
	m_target = other.m_target;
	other.m_ID = 0; //  the element copied from becomes invalid !
	return *this;
}

typedef Texture Tex;

TRE_NS_END