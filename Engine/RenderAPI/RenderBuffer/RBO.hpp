#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderAPI/Common.hpp>
#include <Core/Context/GLDefines.hpp>

TRE_NS_START


struct RenderbufferSettings
{
	RenderbufferSettings() : width(0), height(0), internal_format(RBOInternal::DEPTH24_STENCIL8)
	{}

	RenderbufferSettings(
		uint32 w, uint32 h, 
		RBOInternal::rbo_internal_format_t internal_format = RBOInternal::DEPTH24_STENCIL8)
		: width(w), height(h), internal_format(internal_format)
	{}

	// RenderbufferSettings(RenderbufferSettings&& other) = default;

	uint32 width, height;
	RBOInternal::rbo_internal_format_t internal_format;
};

class FBO;

class RBO
{
public:
	RBO();

	RBO(const RenderbufferSettings& settings);

	~RBO();

	uint32 Generate();

	uint32 Generate(const RenderbufferSettings& settings);

	void SetStorage(uint32 w, uint32 h, RBOInternal::rbo_internal_format_t internal_format = RBOInternal::rbo_internal_format_t::DEPTH24_STENCIL8);
	void AttachToFBO(FBO* fbo, FBOAttachement::framebuffer_attachement_t attchement = FBOAttachement::DEPTH_STENCIL_ATTACH, uint8 color_index = 0);

	FORCEINLINE const uint32 GetID() const;
	FORCEINLINE operator uint32() const;
	FORCEINLINE const int32 GetTarget() const;
	FORCEINLINE const TargetType::target_type_t GetBindingTarget() const { return TargetType::target_type_t::RBO; }

	void Bind() const;
	void Use() const;

	void Unbind() const;
	void Unuse() const;

	void Clean();

	FORCEINLINE void Invalidate() { m_ID = 0; }

	explicit FORCEINLINE RBO(RBO&& other);
	FORCEINLINE RBO& operator=(RBO&& other);

	explicit FORCEINLINE RBO(const RBO& other) = delete;
	FORCEINLINE RBO& operator=(const RBO& other) = delete;
private:
	uint32 m_ID;
};

FORCEINLINE const uint32 RBO::GetID() const
{
	return m_ID;
}

FORCEINLINE RBO::operator uint32() const
{
	return m_ID;
}

FORCEINLINE const int32 RBO::GetTarget() const
{
	return TargetType::target_type_t::RBO;
}

FORCEINLINE RBO::RBO(RBO&& other) :
	m_ID(other.m_ID)
{
	other.m_ID = 0;
}

FORCEINLINE RBO& RBO::operator=(RBO&& other)
{
	m_ID = other.m_ID;
	other.m_ID = 0;
	return *this;
}

typedef RBO Renderbuffer;

TRE_NS_END