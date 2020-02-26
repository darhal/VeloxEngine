#pragma once

#include <Core/ECS/Component/BaseComponent.hpp>
#include <Renderer/Common/Common.hpp>
#include <Renderer/Mesh/SubMesh/SubMesh.hpp>

TRE_NS_START

struct InstancedMeshComponent : public Component<InstancedMeshComponent>
{
	FORCEINLINE InstancedMeshComponent(const InstancedMeshComponent& other) :
		Submeshs(std::move(other.Submeshs)), VaoID(other.VaoID), 
		InstanceCount(other.InstanceCount), DataVboID(other.DataVboID)
	{
	}

	FORCEINLINE InstancedMeshComponent(uint32 instance_count, VaoID vao_id, VboID vbo_id) : 
		Submeshs(), VaoID(vao_id), InstanceCount(instance_count), DataVboID(vbo_id)
	{
	}

	Vector<SubMesh> Submeshs;
	VaoID VaoID;
	uint32 InstanceCount;
	VboID DataVboID;
};

struct MeshInstanceComponent : public Component<MeshInstanceComponent>
{
	MeshInstanceComponent(EntityID ent_id, uint32 inst_id) :
		InstanceModel(ent_id), InstanceID(inst_id)
	{}

	EntityID InstanceModel;
	uint32 InstanceID;
};

TRE_NS_END
