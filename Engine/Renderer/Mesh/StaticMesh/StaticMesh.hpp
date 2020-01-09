#pragma once

#include <Renderer/Common/Common.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>

#include <Renderer/Mesh/IPrimitiveMesh/IPrimitiveMesh.hpp>
#include <Renderer/Mesh/SubMesh/SubMesh.hpp>
#include <Renderer/Backend/CommandBucket/CommandBucket.hpp>

TRE_NS_START

class StaticMesh : public IPrimitiveMesh
{
public:
	FORCEINLINE StaticMesh(VaoID vao) : m_VaoID(vao)
	{}

	FORCEINLINE StaticMesh(StaticMesh&& other);

	FORCEINLINE void AddSubMesh(PrimitiveGeometry& geo, MaterialID mat);

	FORCEINLINE Mat4f& GetTransformationMatrix();

	void Submit(CommandBucket& CmdBucket, const Vec3f& CameraPosition);

	FORCEINLINE const Vector<SubMesh>& GetSubMeshes() const;

	FORCEINLINE VaoID GetVaoID() { return m_VaoID; }

private:
	FORCEINLINE void SetVaoID(VaoID vao);

private:
	Vector<SubMesh> m_Meshs;
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

FORCEINLINE const Vector<SubMesh>& StaticMesh::GetSubMeshes() const
{
	return m_Meshs;
}

TRE_NS_END