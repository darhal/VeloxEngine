#include "Texture.hpp"
#include <Core/Misc/Utils/Image.hpp>
#include <Core/Misc/Utils/Logging.hpp>

TRE_NS_START

Texture::Texture
(
	const char* path, TexTarget::tex_target_t target, 
	std::initializer_list<TexParamConfig> paramList, DataType::data_type_t datatype,
	TexInternalFormat::tex_internal_format_t internalFormat,
	TexFormat::tex_format_t format
) : m_target(target)
{
	Call_GL(
		glGenTextures(1, &m_ID)
	);
	this->Use(); //glBindTexture(target, m_ID);
	// set the texture parameters
	for (const TexParamConfig& p : paramList) {
		Call_GL(
			glTexParameteri(target, p.param, p.val)
		);
	}
	// load image, create texture and generate mipmaps
	int width, height;
	Image img = Image(path);
	width = img.GetWidth(); height = img.GetHeight();
	unsigned char* data = img.GetBytes();
	if (data) {
		Call_GL(
			glTexImage2D(target, 0, internalFormat, width, height, 0, format, datatype, data)
		);
		Call_GL(
			glGenerateMipmap(target)
		);
	}else{
		Log::Write(Log::ERR, "Failed to load texture\n");
	}
}

Texture::Texture(const char* path, const TextureSettings& settings) : m_ID(0), m_target(settings.target)
{
	Call_GL(glGenTextures(1, &m_ID));
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
		Call_GL(
			glTexImage2D(settings.target, settings.lod, settings.internalFormat, width, height, 0, settings.format, settings.datatype, data)
		);
		Call_GL(
			glGenerateMipmap(settings.target)
		);
	}
	else {
		Log::Write(Log::ERR, "Failed to load texture\n");
	}
}

Texture::Texture(const TextureSettings& settings) : m_ID(0), m_target(settings.target)
{
	Call_GL(
		glGenTextures(1, &m_ID)
	);
	this->Use(); //glBindTexture(settings.target, m_ID);
	// set the texture parameters
	for (const TexParamConfig& p : settings.paramList) {
		Call_GL(glTexParameteri(settings.target, p.param, p.val));
	}
	// load image, create texture and generate mipmaps
	Call_GL(
		glTexImage2D(settings.target, settings.lod, settings.internalFormat, settings.width, settings.height, 0, settings.format, settings.datatype, settings.img_data)
	);
}

void Texture::LoadImg(const Image& img, DataType::data_type_t datatype, TexInternalFormat::tex_internal_format_t internalFormat, TexFormat::tex_format_t format)
{
	// load image, create texture
	this->Use(); //glBindTexture(target, m_ID);
	int width, height;
	width = img.GetWidth(); height = img.GetHeight();
	unsigned char* data = img.GetBytes();
	if (data) {
		Call_GL(glTexImage2D(m_target, 0, internalFormat, width, height, 0, format, datatype, data));
	}else{
		Log::Write(Log::ERR, "Failed to load texture\n");
	}
}

uint32 Texture::Generate(const TextureSettings& settings)
{
	m_target = settings.target;
	Call_GL(
		glGenTextures(1, &m_ID)
	);

	this->Use(); //glBindTexture(settings.target, m_ID);
	// set the texture parameters
	
	for (const TexParamConfig& p : settings.paramList) {
		Call_GL(glTexParameteri(settings.target, p.param, p.val));
	}

	glTexParameterfv(settings.target, GL_TEXTURE_BORDER_COLOR, &settings.borderColor.x);

	// load image, create texture and generate mipmaps
	Call_GL(
		glTexImage2D(settings.target, settings.lod, settings.internalFormat, settings.width, settings.height, 0, settings.format, settings.datatype, settings.img_data)
	);

	if (settings.lod){
		Call_GL(
			glGenerateMipmap(settings.target)
		);
	}

	return m_ID;
}

TRE_NS_END


