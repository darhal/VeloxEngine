#pragma once

#include <Core/ECS/Component/BaseComponent.hpp>
#include <Renderer/Common/Common.hpp>
#include <Renderer/Mesh/SubMesh/SubMesh.hpp>

TRE_NS_START

struct StaticMeshComponent : public Component<StaticMeshComponent>
{
	FORCEINLINE StaticMeshComponent(const StaticMeshComponent& other) :
		submeshs(std::move(other.submeshs)), vao_id(other.vao_id)
	{}

	FORCEINLINE StaticMeshComponent(VaoID vao_id) :
		submeshs(), vao_id(vao_id)
	{}

	Vector<SubMesh> submeshs;
	VaoID vao_id;
};

TRE_NS_END