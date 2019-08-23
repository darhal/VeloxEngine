#include "RBO.hpp"
#include <Core/Context/Extensions.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <RenderAPI/FrameBuffer/FBO.hpp>

TRE_NS_START

RBO::RBO() : m_ID(0)
{
}

uint32 RBO::Generate()
{
	Call_GL(glGenRenderbuffers(1, &m_ID));
	return m_ID;
}

uint32 RBO::Generate(const RenderbufferSettings& settings)
{
	Call_GL(glGenRenderbuffers(1, &m_ID));
	this->Use();
	Call_GL(glRenderbufferStorage(GL_RENDERBUFFER, settings.internal_format, settings.width, settings.height));
	return m_ID;
}

RBO::RBO(const RenderbufferSettings& settings)
{
	Call_GL(glGenRenderbuffers(1, &m_ID));
	this->Use();
	Call_GL(glRenderbufferStorage(GL_RENDERBUFFER, settings.internal_format, settings.width, settings.height));
}

void RBO::SetStorage(uint32 w, uint32 h, RBOInternal::rbo_internal_format_t internal_format)
{
	this->Use();
	Call_GL(glRenderbufferStorage(GL_RENDERBUFFER, internal_format, w, h));
}

void RBO::AttachToFBO(FBO* fbo, FBOAttachement::framebuffer_attachement_t attchement, uint8 color_index)
{
	if (fbo == NULL)
		return;

	fbo->Use();
	Call_GL(glFramebufferRenderbuffer(fbo->GetTarget(), attchement + color_index, GL_RENDERBUFFER, m_ID));
}

void RBO::Bind() const
{
	Call_GL(glBindRenderbuffer(GetTarget(), m_ID));
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
	if (m_ID) {
		this->Clean();
		m_ID = 0;
	}
}

void RBO::Clean()
{
	Call_GL(glDeleteRenderbuffers(1, &m_ID));
	m_ID = 0;
}

TRE_NS_END