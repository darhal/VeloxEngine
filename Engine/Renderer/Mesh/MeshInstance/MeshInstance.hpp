#pragma once

#include <Renderer/Common/Common.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>

#include <Renderer/Mesh/IPrimitiveMesh/IPrimitiveMesh.hpp>
#include <Renderer/Mesh/SubMesh/SubMesh.hpp>
#include <Renderer/Backend/CommandBucket/CommandBucket.hpp>

TRE_NS_START

class MeshInstance : public IPrimitiveMesh
{
public:
	MeshInstance(uint32 instance_count, VaoID vao_id, VboID vbo_id, Mat4f* transform);

	MeshInstance(MeshInstance&& other);

	void Submit(CommandBucket& CmdBucket, ShaderID shader_id) final;

	FORCEINLINE void AddSubMesh(PrimitiveGeometry& geo, MaterialID mat);

	FORCEINLINE Mat4f& GetTransformationMatrix(uint32 instance_id);

	FORCEINLINE const Vector<SubMesh>& GetSubMeshes() const;

	FORCEINLINE VaoID GetVaoID() const { return m_VaoID; }

	void UpdateTransforms(uint32 instance_id);

private:
	Vector<SubMesh> m_Meshs;
	Mat4f* m_ModelTransformations;
	uint32 m_InstanceCount;

	VaoID m_VaoID;
	VboID m_DataVboID;
};

FORCEINLINE MeshInstance::MeshInstance(MeshInstance&& other) :
	m_Meshs(std::move(other.m_Meshs)), m_ModelTransformations(other.m_ModelTransformations),
	m_InstanceCount(other.m_InstanceCount), m_VaoID(other.m_VaoID), m_DataVboID(other.m_DataVboID)
{}

FORCEINLINE void MeshInstance::AddSubMesh(PrimitiveGeometry& geo, MaterialID mat)
{
	m_Meshs.EmplaceBack(geo, mat);
}

FORCEINLINE const Vector<SubMesh>& MeshInstance::GetSubMeshes() const
{
	return m_Meshs;
}

Mat4f& MeshInstance::GetTransformationMatrix(uint32 instance_id)
{
	ASSERTF((instance_id >= m_InstanceCount), "Attempt to call 'GetTransformationMatrix' with invalid instance ID (Instance ID out of bound).");
	return m_ModelTransformations[instance_id];
}

TRE_NS_END