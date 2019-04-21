#include "RBO.hpp"
#include <Core/Context/Extensions.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

RBO::RBO() : m_AutoClean(true)
{
	glGenRenderbuffers(1, &m_ID);
}

RBO::RBO(const RenderbufferSettings& settings)
{
	ASSERTF(settings.fbo == NULL, "Attempt to create RBO with settings without passing a valid FBO (settings.fbo is a nullptr).");
	glGenRenderbuffers(1, &m_ID);
	this->Use();
	settings.fbo->Use();
	glRenderbufferStorage(GL_RENDERBUFFER, settings.internal_format, settings.w, settings.h);
	glFramebufferRenderbuffer(settings.fbo->GetTarget(), settings.attachement + settings.color_index, GL_RENDERBUFFER, m_ID);
}

void RBO::SetStorage(uint32 w, uint32 h, RBOInternal::rbo_internal_format_t internal_format)
{
	this->Use();
	glRenderbufferStorage(GL_RENDERBUFFER, internal_format, w, h);
}

void RBO::AttachToFBO(const FBO& fbo, FBOAttachement::framebuffer_attachement_t attchement, uint8 color_index)
{
	fbo.Use();
	this->Use();
	glFramebufferRenderbuffer(fbo.GetTarget(), attchement + color_index, GL_RENDERBUFFER, m_ID);
}

void RBO::Bind() const
{
	glBindRenderbuffer(GetTarget(), m_ID);
}

void RBO::Use() const
{
	GLState::Bind(this);
}

void RBO::Unbind() const
{
	glBindRenderbuffer(GetTarget(), 0);
}

void RBO::Unuse() const
{
	GLState::Unbind(this);
}

RBO::~RBO()
{
	if (m_AutoClean) {
		Clean();
	}
}

void RBO::Clean()
{
	m_AutoClean = false;
	glDeleteRenderbuffers(1, &m_ID);
}

TRE_NS_END