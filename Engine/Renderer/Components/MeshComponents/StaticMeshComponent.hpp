#pragma once

#include <Core/ECS/Component/BaseComponent.hpp>
#include <Renderer/Common/Common.hpp>
#include <Renderer/Mesh/SubMesh/SubMesh.hpp>

TRE_NS_START

struct StaticMeshComponent : public Component<StaticMeshComponent>
{
	FORCEINLINE StaticMeshComponent(const StaticMeshComponent& other) :
		Submeshs(std::move(other.Submeshs)), VaoID(other.VaoID)
	{}

	FORCEINLINE StaticMeshComponent(VaoID vao_id) :
		Submeshs(), VaoID(vao_id)
	{}

	Vector<SubMesh> Submeshs;
	VaoID VaoID;
};

TRE_NS_END