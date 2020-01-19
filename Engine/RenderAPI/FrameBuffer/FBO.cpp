#include "FBO.hpp"

#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Utils/Logging.hpp>

#include <RenderAPI/GlobalState/GLState.hpp>
#include <RenderAPI/Texture/Texture.hpp>

TRE_NS_START

FBO::FBO(FBOTarget::framebuffer_target_t t) : m_ID(0), m_target(t)
{
}

uint32 FBO::Generate()
{
	Call_GL(glGenFramebuffers(1, &m_ID));
	return m_ID;
}

uint32 FBO::Generate(const FramebufferSettings& settings)
{
	Call_GL(glGenFramebuffers(1, &m_ID));
	m_target = settings.target;

	this->Use();

	Call_GL(
		for (const FramebufferSettings::TextureAttachement& tex_attach : settings.texture_attachments) {
			glFramebufferTexture2D(m_target, tex_attach.texture_attachement + tex_attach.texture_attachment_id,
				tex_attach.texture->GetBindingTarget(), tex_attach.texture->GetID(), tex_attach.mipmap_level);
		}
	);

	if (settings.rbo != NULL) {
		Call_GL(
			glFramebufferRenderbuffer(m_target, settings.rbo_attachement, settings.rbo->GetBindingTarget(), settings.rbo->GetID())
		);
	}

	const Vector<uint32>& draw_buffers = settings.draw_buffers;
	if (draw_buffers.IsEmpty())
		SetDrawBuffer(FBOColourBuffer::NONE);
	else
		SetDrawBuffers(draw_buffers.Front(), (uint32)draw_buffers.Length());

	SetReadBuffer(settings.read_buffer);
	
	return m_ID;
}

bool FBO::IsComplete() const
{
	this->Use();
	
	if (glCheckFramebufferStatus(m_target) == GL_FRAMEBUFFER_COMPLETE) 
		return true;

	Log::Write(Log::ERR, "FRAMEBUFFER : Framebuffer is not complete! (ID = %d)", m_ID);
	return false;
}

void FBO::AttachTexture(const Texture& tex, uint8 color_attachement_id, uint8 mipmap_level)
{
	this->Use();
	Call_GL(glFramebufferTexture2D(m_target, GL_COLOR_ATTACHMENT0 + color_attachement_id, tex.GetBindingTarget(), tex, mipmap_level));
}

void FBO::AttachTexture(const Texture & tex, FBOAttachement::framebuffer_attachement_t attachement, uint8 mipmap_level)
{
	this->Use();
	Call_GL(glFramebufferTexture2D(m_target, attachement, tex.GetBindingTarget(), tex, mipmap_level));
}

void FBO::AttachTexture(const Texture& tex, FBOTexTarget::framebuffer_tex_target_t target, uint8 color_attachement_id, uint8 mipmap_level)
{
	this->Use();
	Call_GL(glFramebufferTexture2D(m_target, GL_COLOR_ATTACHMENT0 + color_attachement_id, target, tex, mipmap_level));
}

void FBO::AttachTexture(const Texture& tex, FBOTexTarget::framebuffer_tex_target_t target, FBOAttachement::framebuffer_attachement_t attachement, uint8 mipmap_level)
{
	this->Use();
	Call_GL(glFramebufferTexture2D(m_target, attachement, target, tex, mipmap_level));
}

void FBO::AttachRenderbuffer(const RBO& rbo, FBOAttachement::framebuffer_attachement_t attchement, uint8 color_index)
{
	this->Use();
	Call_GL(glFramebufferRenderbuffer(m_target, attchement + color_index, GL_RENDERBUFFER, rbo.GetID()));
}

void FBO::Bind() const
{
	Call_GL(glBindFramebuffer(m_target, m_ID));
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

FBO::~FBO()
{
	if (m_ID) {
		Clean();
		m_ID = 0;
	}
}

void FBO::Clean()
{
	Call_GL(glDeleteFramebuffers(1, &m_ID));
	m_ID = 0;
}

TRE_NS_END