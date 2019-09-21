#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Utils/Color.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>

#include <RenderAPI/Common.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>

TRE_NS_START

class Color;

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
		TexFormat::tex_format_t format = TexFormat::RGBA, const vec4& border_color = vec4(0.f, 0.f, 0.f, 0.f)) :
		paramList(std::move(paramList)), img_data(data), width(width), height(height), lod(lod), 
		target(target), datatype(datatype), internalFormat(internalFormat), format(format), borderColor(border_color)
	{}

	/*TextureSettings(TextureSettings&& other) :
		paramList(std::move(other.paramList)),
		img_data(other.img_data),
		width(other.width), height(other.height), lod(other.lod),
		target(other.target), datatype(other.datatype), internalFormat(other.internalFormat),
		format(other.format)
	{}

	TextureSettings(const TextureSettings& other) :
		paramList(std::move(other.paramList)),
		img_data(other.img_data),
		width(other.width), height(other.height), lod(other.lod),
		target(other.target), datatype(other.datatype), internalFormat(other.internalFormat),
		format(other.format)
	{}*/

	Vector<TexParamConfig> paramList = {};
	void* img_data = NULL;
	int32 width, height, lod = 0;
	TexTarget::tex_target_t target = TexTarget::TEX2D;
	DataType::data_type_t datatype = DataType::UBYTE;
	TexInternalFormat::tex_internal_format_t internalFormat = TexInternalFormat::RGBA;
	TexFormat::tex_format_t format = TexFormat::RGBA;
	vec4 borderColor = vec4(0.f, 0.f, 0.f, 0.f);

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