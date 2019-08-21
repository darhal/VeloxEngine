#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <RenderEngine/Renderer/Common/Common.hpp>

TRE_NS_START

struct RenderTarget
{
	Mat4f m_Projection;
	Mat4f m_View;
	uint16 m_Width, m_Height;
	FboID m_FboID;
};

TRE_NS_END