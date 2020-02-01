#pragma once

#include <Core/ECS/Component/BaseComponent.hpp>
#include <Renderer/Common/Common.hpp>
#include <Renderer/Mesh/SubMesh/SubMesh.hpp>

TRE_NS_START

struct InstancedMeshComponent : public Component<InstancedMeshComponent>
{
	FORCEINLINE InstancedMeshComponent(const InstancedMeshComponent& other) :
		Submeshs(std::move(other.Submeshs)), InstanceCount(other.InstanceCount),
		VaoID(other.VaoID), DataVboID(other.DataVboID)
	{
	}

	FORCEINLINE InstancedMeshComponent(uint32 instance_count, VaoID vao_id, VboID vbo_id)
	{
	}

	Vector<SubMesh> Submeshs;
	uint32 InstanceCount;
	VaoID VaoID;
	VboID DataVboID;
};

struct MeshInstanceComponent : public Component<MeshInstanceComponent>
{
	EntityID InstanceModel;
};

TRE_NS_END
