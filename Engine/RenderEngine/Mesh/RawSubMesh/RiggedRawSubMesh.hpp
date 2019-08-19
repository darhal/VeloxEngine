#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Mesh/PrimitiveGeometry/PrimitiveGeometry.hpp"
#include <RenderEngine/Renderer/Common/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>

TRE_NS_START

struct RiggedRawSubMesh
{
    RiggedRawSubMesh(PrimitiveGeometry& geo, MaterialID matID, const Mat4f& transform = {}) : 
        m_Geometry(geo), m_MaterialID(matID), m_ModelTransformation(transform)
    {};

    FORCEINLINE Mat4f& GetTransformationMatrix() { return m_ModelTransformation; };

    PrimitiveGeometry m_Geometry;
    MaterialID m_MaterialID;
    Mat4f m_ModelTransformation;
};

TRE_NS_END