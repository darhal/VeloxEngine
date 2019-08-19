#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderAPI/Common.hpp>
#include <Core/Context/GLDefines.hpp>

TRE_NS_START

namespace FBOTarget
{
	enum framebuffer_target_t {
		FBO_DRAW = GL_DRAW_FRAMEBUFFER,
		FBO_READ = GL_READ_FRAMEBUFFER,
		FBO = GL_FRAMEBUFFER
	};
};

namespace FBOAttachement
{
	enum framebuffer_attachement_t {
		DEPTH_ATTACH = GL_DEPTH_ATTACHMENT,
		STENCIL_ATTACH = GL_STENCIL_ATTACHMENT,
		DEPTH_STENCIL_ATTACHMENT = GL_DEPTH_STENCIL_ATTACHMENT,
		COLOR_ATTACH = GL_COLOR_ATTACHMENT0
	};
}

namespace FBOTexTarget 
{
	enum framebuffer_tex_target_t {
		TEX2D = GL_TEXTURE_2D, 
		TEX_CUBE_PX = GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
		TEX_CUBE_NX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		TEX_CUBE_PY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		TEX_CUBE_NY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		TEX_CUBE_PZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		TEX_CUBE_NZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
}

typedef class FBO
{
public:
	FBO(FBOTarget::framebuffer_target_t t = FBOTarget::FBO);
	~FBO();

	void AttachTexture(const Texture& tex, uint8 color_attachement_id = 0, uint8 mipmap_level = 0);
	void AttachTexture(const Texture& tex, FBOAttachement::framebuffer_attachement_t attachement, uint8 mipmap_level = 0);
	void AttachTexture(const Texture& tex, FBOTexTarget::framebuffer_tex_target_t target, uint8 color_attachement_id = 0, uint8 mipmap_level = 0);
	void AttachTexture(const Texture& tex, FBOTexTarget::framebuffer_tex_target_t target, FBOAttachement::framebuffer_attachement_t attachement, uint8 mipmap_level = 0);

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