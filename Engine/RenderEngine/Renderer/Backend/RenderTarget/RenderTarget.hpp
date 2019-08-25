#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <RenderEngine/Renderer/Common/Common.hpp>

TRE_NS_START

struct RenderTarget
{
	RenderTarget() : m_FboID(RenderSettings::DEFAULT_FRAMEBUFFER)
	{
	}

	RenderTarget(FboID id, uint16 width, uint16 height, Mat4f projection = Mat4f(), Mat4f view = Mat4f()) :
		m_Projection(projection), m_View(view), m_Width(width), m_Height(height), m_FboID(id)
	{}

	Mat4f m_Projection;
	Mat4f m_View;
	uint16 m_Width, m_Height;
	FboID m_FboID;
};

TRE_NS_END