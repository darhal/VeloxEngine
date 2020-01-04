#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>
#include <Renderer/Common/Common.hpp>

TRE_NS_START

struct RenderTarget
{
	RenderTarget() : m_Width(0), m_Height(0), m_FboID(RenderSettings::DEFAULT_FRAMEBUFFER)
	{}

	RenderTarget(FboID id, uint16 width, uint16 height) :
		m_Width(width), m_Height(height), m_FboID(id)
	{}

	uint16 m_Width, m_Height;
	FboID m_FboID;
};

TRE_NS_END