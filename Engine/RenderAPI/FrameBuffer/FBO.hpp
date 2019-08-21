#pragma once

#include <Core/Context/GLDefines.hpp>
#include <Core/Misc/Defines/Common.hpp>
#include <RenderAPI/Common.hpp>
#include <RenderAPI/RenderBuffer/RBO.hpp>

TRE_NS_START

typedef class FBO
{
public:
	FBO(FBOTarget::framebuffer_target_t t = FBOTarget::FBO);
	~FBO();

	void AttachTexture(const Texture& tex, uint8 color_attachement_id = 0, uint8 mipmap_level = 0);
	void AttachTexture(const Texture& tex, FBOAttachement::framebuffer_attachement_t attachement, uint8 mipmap_level = 0);
	void AttachTexture(const Texture& tex, FBOTexTarget::framebuffer_tex_target_t target, uint8 color_attachement_id = 0, uint8 mipmap_level = 0);
	void AttachTexture(const Texture& tex, FBOTexTarget::framebuffer_tex_target_t target, FBOAttachement::framebuffer_attachement_t attachement, uint8 mipmap_level = 0);

	void AttachRenderbuffer(const RBO& rbo, FBOAttachement::framebuffer_attachement_t attchement, uint8 color_index);

	FORCEINLINE void SetDrawBuffer(uint8 color_attachement_id = 0);
	FORCEINLINE void SetDrawBuffer(FBOColourBuffer::colour_buffer_t col_buf);

	FORCEINLINE void SetReadBuffer(uint8 color_attachement_id = 0);
	FORCEINLINE void SetReadBuffer(FBOColourBuffer::colour_buffer_t col_buf);

	template<uint32 S>
	FORCEINLINE void SetDrawBuffers(const uint32(&color_attachement_id)[S]);
	template<uint32 S>
	FORCEINLINE void SetDrawBuffers(const FBOColourBuffer::colour_buffer_t(&col_buf)[S]);

	bool IsComplete() const;

	FORCEINLINE const uint32 GetID() const;
	FORCEINLINE operator uint32() const;
	FORCEINLINE const int32 GetTarget() const;
	FORCEINLINE const TargetType::target_type_t GetBindingTarget() const { return (TargetType::target_type_t)m_target; }

	void Bind() const;
	void Use() const;

	void Unbind() const;
	void Unuse() const;

	uint32 Generate();

	void Clean();

	FORCEINLINE void Invalidate() { m_ID = 0; }

	explicit FORCEINLINE FBO(FBO&& other);
	FORCEINLINE FBO& operator=(FBO&& other);

	explicit FORCEINLINE FBO(const FBO& other) = delete;
	FORCEINLINE FBO& operator=(const FBO& other) = delete;
private:
	uint32 m_ID;
	int32 m_target;
} FrameBuffer;

FORCEINLINE const uint32 FBO::GetID() const
{
	return m_ID;
}

FORCEINLINE FBO::operator uint32() const
{
	return m_ID;
}

FORCEINLINE const int32 FBO::GetTarget() const
{
	return m_target;
}

FORCEINLINE void FBO::SetDrawBuffer(uint8 color_attachement_id)
{
	glDrawBuffer(FBOAttachement::COLOR_ATTACH + color_attachement_id);
}

FORCEINLINE void FBO::SetDrawBuffer(FBOColourBuffer::colour_buffer_t col_buf)
{
	glDrawBuffer(col_buf);
}

FORCEINLINE void FBO::SetReadBuffer(uint8 color_attachement_id)
{
	glReadBuffer(FBOAttachement::COLOR_ATTACH + color_attachement_id);
}

FORCEINLINE void FBO::SetReadBuffer(FBOColourBuffer::colour_buffer_t col_buf)
{
	glReadBuffer(col_buf);
}

template<uint32 S>
FORCEINLINE void FBO::SetDrawBuffers(const uint32(&color_attachement_id)[S])
{
	glDrawBuffers(S, color_attachement_id);
}

template<uint32 S>
FORCEINLINE void FBO::SetDrawBuffers(const FBOColourBuffer::colour_buffer_t(&col_buf)[S])
{
	glDrawBuffers(S, col_buf);
}

FORCEINLINE FBO::FBO(FBO&& other) :
	m_ID(other.m_ID), m_target(other.m_target)
{
	other.m_ID = 0;
}

FORCEINLINE FBO& FBO::operator=(FBO&& other)
{
	m_ID = other.m_ID;
	m_target = other.m_target;
	other.m_ID = 0;
	return *this;
}

// typedef FBO FrameBuffer;

TRE_NS_END