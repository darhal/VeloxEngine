#include "FBO.hpp"
#include <Core/Context/Extensions.hpp>
#include <Renderer/GlobalState/GLState.hpp>
#include <Renderer/Texture/Texture.hpp>
#include <Core/Misc/Utils/Logging.hpp>

TRE_NS_START

FBO::FBO(FBOTarget::framebuffer_target_t t) : m_target(t)
{
	glGenFramebuffers(1, &m_ID);
}

FBO::~FBO()
{
	glDeleteFramebuffers(1, &m_ID);
}

bool FBO::IsComplete() const
{
	this->Use();
	if (glCheckFramebufferStatus(m_target) == GL_FRAMEBUFFER_COMPLETE) return true;
	Log(LogType::ERR, "FRAMEBUFFER : Framebuffer is not complete! (ID = %d)", m_ID);
	return false;
}

void FBO::AttachTexture(const Texture& tex, uint8 color_attachement_id, uint8 mipmap_level)
{
	this->Use();
	glFramebufferTexture2D(m_target, GL_COLOR_ATTACHMENT0 + color_attachement_id, tex.GetBindingTarget(), tex, mipmap_level);
}

void FBO::AttachTexture(const Texture & tex, FBOAttachement::framebuffer_attachement_t attachement, uint8 mipmap_level)
{
	this->Use();
	glFramebufferTexture2D(m_target, attachement, tex.GetBindingTarget(), tex, mipmap_level);
}

void FBO::AttachTexture(const Texture& tex, FBOTexTarget::framebuffer_tex_target_t target, uint8 color_attachement_id, uint8 mipmap_level)
{
	this->Use();
	glFramebufferTexture2D(m_target, GL_COLOR_ATTACHMENT0 + color_attachement_id, target, tex, mipmap_level);
}

void FBO::AttachTexture(const Texture& tex, FBOTexTarget::framebuffer_tex_target_t target, FBOAttachement::framebuffer_attachement_t attachement, uint8 mipmap_level)
{
	this->Use();
	glFramebufferTexture2D(m_target, attachement, target, tex, mipmap_level);
}

void FBO::Bind() const
{
	glBindFramebuffer(m_target, m_ID);
}

void FBO::Use() const
{
	GLState::Bind(this);
}

void FBO::Unbind() const
{
	glBindFramebuffer(m_target, 0);
}

void FBO::Unuse() const
{
	GLState::Unbind(this);
}

TRE_NS_END