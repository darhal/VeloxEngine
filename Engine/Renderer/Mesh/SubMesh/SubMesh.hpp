#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include <Core/Misc/Maths/Maths.hpp>

#include <Renderer/Common/Common.hpp>
#include <Renderer/Mesh/PrimitiveGeometry/PrimitiveGeometry.hpp>

TRE_NS_START

struct SubMesh
{
	SubMesh(PrimitiveGeometry& geo, MaterialID matID) :
		m_Geometry(geo), m_MaterialID(matID)
	{};

	PrimitiveGeometry m_Geometry;
	MaterialID m_MaterialID;
};

TRE_NS_END