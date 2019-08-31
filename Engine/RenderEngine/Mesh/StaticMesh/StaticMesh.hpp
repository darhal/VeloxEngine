#pragma once

#include <RenderEngine/Renderer/Common/Common.hpp>
#include "RenderEngine/Mesh/IPrimitiveMesh/IPrimitiveMesh.hpp"
#include <RenderEngine/Mesh/RawSubMesh/RawSubMesh.hpp>
#include <iostream>

TRE_NS_START

class StaticMesh : public IPrimitiveMesh
{
public:
    FORCEINLINE StaticMesh(VaoID vao) : m_VaoID(vao)
    {}

    FORCEINLINE StaticMesh()  : m_VaoID(0)
    {}

    FORCEINLINE StaticMesh(StaticMesh&& other);

    FORCEINLINE void AddSubMesh(PrimitiveGeometry& geo, MaterialID mat);

    FORCEINLINE Mat4f& GetTransformationMatrix();

    void Submit(RenderCommandBuffer& CmdBucket, const Vec3f& CameraPosition);

	void Submit(FramebufferCommandBuffer& CmdBucket, RenderTargetID render_target_id, FramebufferCommandBuffer::FrameBufferPiriority::Piroirty_t piroirty = 0);

    FORCEINLINE const Vector<RawSubMesh>& GetSubMeshes() const;

	FORCEINLINE VaoID GetVaoID() { return m_VaoID; }

private:
    FORCEINLINE void SetVaoID(VaoID vao);

private:
    Vector<RawSubMesh> m_Meshs;
    VaoID m_VaoID;
    Mat4f m_ModelTransformation;

    friend class ModelLoader;
};

FORCEINLINE StaticMesh::StaticMesh(StaticMesh&& other) : 
    m_Meshs(std::move(other.m_Meshs)), m_VaoID(std::move(other.m_VaoID)), 
    m_ModelTransformation(other.m_ModelTransformation)
{
}

FORCEINLINE void StaticMesh::AddSubMesh(PrimitiveGeometry& geo, MaterialID mat)
{
    m_Meshs.EmplaceBack(geo, mat);
}

FORCEINLINE Mat4f& StaticMesh::GetTransformationMatrix()
{
    return m_ModelTransformation;
}

FORCEINLINE void StaticMesh::SetVaoID(VaoID vao)
{
    m_VaoID = vao;
}

FORCEINLINE const Vector<RawSubMesh>& StaticMesh::GetSubMeshes() const
{
    return m_Meshs;
}

TRE_NS_END