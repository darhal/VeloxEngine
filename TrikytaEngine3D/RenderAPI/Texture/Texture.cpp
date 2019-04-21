#include "Texture.hpp"
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Utils/Image.hpp>
#include <Core/Misc/Utils/Logging.hpp>
#include <Core/Misc/Utils/Color.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>

TRE_NS_START

Texture::Texture
(
	const char* path, TexTarget::tex_target_t target, 
	std::initializer_list<TexParamConfig> paramList, DataType::data_type_t datatype,
	TexInternalFormat::tex_internal_format_t internalFormat,
	TexFormat::tex_format_t format
) : m_target(target), m_AutoClean(true)
{
	glGenTextures(1, &m_ID);
	this->Use(); //glBindTexture(target, m_ID);
	// set the texture parameters
	for (const TexParamConfig& p : paramList) {
		glTexParameteri(target, p.param, p.val);
	}
	// load image, create texture and generate mipmaps
	int width, height;
	Image img = Image(path);
	width = img.GetWidth(); height = img.GetHeight();
	unsigned char* data = img.GetBytes();
	if (data) {
		glTexImage2D(target, 0, internalFormat, width, height, 0, format, datatype, data);
		glGenerateMipmap(target);
	}else{
		Log(LogType::ERR, "Failed to load texture\n");
	}
}

Texture::Texture(const char* path, const TextureSettings& settings) : m_target(settings.target), m_AutoClean(true)
{
	glGenTextures(1, &m_ID);
	this->Use(); //glBindTexture(settings.target, m_ID);
	// set the texture parameters
	for (const TexParamConfig& p : settings.paramList) {
		glTexParameteri(settings.target, p.param, p.val);
	}
	// load image, create texture and generate mipmaps
	int width, height;
	Image img = Image(path);
	width = img.GetWidth(); height = img.GetHeight();
	unsigned char* data = img.GetBytes();
	if (data) {
		glTexImage2D(settings.target, settings.lod, settings.internalFormat, width, height, 0, settings.format, settings.datatype, data);
		glGenerateMipmap(settings.target);
	}
	else {
		Log(LogType::ERR, "Failed to load texture\n");
	}
}

Texture::Texture(const TextureSettings& settings) : m_target(settings.target), m_AutoClean(true)
{
	glGenTextures(1, &m_ID);
	this->Use(); //glBindTexture(settings.target, m_ID);
	// set the texture parameters
	for (const TexParamConfig& p : settings.paramList) {
		glTexParameteri(settings.target, p.param, p.val);
	}
	// load image, create texture and generate mipmaps
	glTexImage2D(settings.target, settings.lod, settings.internalFormat, settings.width, settings.height, 0, settings.format, settings.datatype, NULL);
}

Texture::Texture() : m_target(TexTarget::TEX2D), m_AutoClean(true)
{
	glGenTextures(1, &m_ID);
}

void Texture::SetTextureTarget(TexTarget::tex_target_t target)
{
	m_target = target;
	this->Use(); //glBindTexture(target, m_ID);
}

void Texture::LoadImg(const Image& img, DataType::data_type_t datatype, TexInternalFormat::tex_internal_format_t internalFormat, TexFormat::tex_format_t format)
{
	// load image, create texture
	this->Use(); //glBindTexture(target, m_ID);
	int width, height;
	width = img.GetWidth(); height = img.GetHeight();
	unsigned char* data = img.GetBytes();
	if (data) {
		glTexImage2D(m_target, 0, internalFormat, width, height, 0, format, datatype, data);
	}else{
		Log(LogType::ERR, "Failed to load texture\n");
	}
}

void Texture::EmptyTexture(uint32 width, uint32 height, DataType::data_type_t datatype, TexInternalFormat::tex_internal_format_t internalFormat, TexFormat::tex_format_t format)
{
	this->Use(); //glBindTexture(target, m_ID);
	glTexImage2D(m_target, 0, internalFormat, width, height, 0, format, datatype, NULL);
}

void Texture::SetWrapping(TexParam::tex_param_t p, TexWrapping::tex_wrapping_t s)
{
	this->Use(); //glBindTexture(target, m_ID);glBindTexture(m_target, m_ID);
	glTexParameteri(m_target, p, s);
}

void Texture::SetFilters(TexParam::tex_param_t p, TexFilter::tex_filter_t filter)
{
	glTexParameteri(m_target, p, filter);
}

void Texture::SetBorderColor(const Color& color)
{
	this->Use(); //glBindTexture(target, m_ID);
	float col[4] = { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
	glTexParameterfv(m_target, GL_TEXTURE_BORDER_COLOR, col);
}

void Texture::GenerateMipmaps()
{
	this->Use(); //glBindTexture(target, m_ID);
	glGenerateMipmap(m_target);
}

void Texture::Bind() const
{
	glBindTexture(m_target, m_ID);
}

void Texture::Use() const
{
	GLState::Bind(this);
}

void Texture::Unbind() const
{
	glBindTexture(m_target, 0);
}

void Texture::Unuse() const
{
	GLState::Unbind(this);
}

Texture::~Texture()
{
	if (m_AutoClean) {
		Clean();
	}
}

void Texture::Clean()
{
	m_AutoClean = false;
	glDeleteTextures(1, &m_ID);
}

TRE_NS_END


