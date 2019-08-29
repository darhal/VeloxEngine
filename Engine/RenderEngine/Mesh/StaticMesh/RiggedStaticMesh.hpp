#pragma once

#include <RenderEngine/Renderer/Common/Common.hpp>
#include "RenderEngine/Mesh/IPrimitiveMesh/IPrimitiveMesh.hpp"
#include <RenderEngine/Mesh/RawSubMesh/RiggedRawSubMesh.hpp>

TRE_NS_START

class ShaderProgram;

class RiggedStaticMesh : public IPrimitiveMesh
{
public:
    FORCEINLINE RiggedStaticMesh(VaoID vao) : m_VaoID(vao)
    {}

    FORCEINLINE RiggedStaticMesh() : m_VaoID(0)
    {}

    FORCEINLINE RiggedStaticMesh(RiggedStaticMesh&& other);

    FORCEINLINE void AddSubMesh(PrimitiveGeometry& geo, MaterialID mat, const Mat4f& transform = {});

    FORCEINLINE const RiggedRawSubMesh& GetSubMesh(usize index) const;

    FORCEINLINE RiggedRawSubMesh& GetSubMesh(usize index);

    void Submit(RenderCommandBuffer& CmdBucket, const Vec3f& CameraPosition);

	void Submit(FramebufferCommandBuffer& CmdBucket, RenderTarget* render_target, FramebufferCommandBuffer::FrameBufferPiriority::Piroirty_t piroirty = 0);

private:
    FORCEINLINE void SetVaoID(VaoID vao);

private:
    Vector<RiggedRawSubMesh> m_Meshs;
    VaoID m_VaoID;

    friend class ModelLoader;
};

FORCEINLINE RiggedStaticMesh::RiggedStaticMesh(RiggedStaticMesh&& other) : 
    m_Meshs(std::move(other.m_Meshs)), m_VaoID(std::move(other.m_VaoID))
{}

FORCEINLINE void RiggedStaticMesh::AddSubMesh(PrimitiveGeometry& geo, MaterialID mat, const Mat4f& transform)
{
    m_Meshs.EmplaceBack(geo, mat, transform);
}

FORCEINLINE const RiggedRawSubMesh& RiggedStaticMesh::GetSubMesh(usize index) const
{
    return m_Meshs.At(index);
}

FORCEINLINE RiggedRawSubMesh& RiggedStaticMesh::GetSubMesh(usize index)
{
    return m_Meshs.At(index);
}

FORCEINLINE void RiggedStaticMesh::SetVaoID(VaoID vao)
{
    m_VaoID = vao;
}

TRE_NS_END