#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Components/MeshComponents/StaticMeshComponent.hpp>
#include <Renderer/Components/Misc/TransformComponent.hpp>
#include <Core/ECS/System/BaseSystem.hpp>

TRE_NS_START

class CommandBucket;

class MeshSystem
{
public:
	MeshSystem(CommandBucket* cmd_buffer = NULL);

	void UpdateComponents(float dt, Archetype& arche);

	void SetCommandBucket(CommandBucket* bucket) { m_CommandBucket = bucket; }
private:
	CommandBucket* m_CommandBucket;

	void Submit(const StaticMeshComponent& mesh, const TransformComponent& transform);
};

TRE_NS_END