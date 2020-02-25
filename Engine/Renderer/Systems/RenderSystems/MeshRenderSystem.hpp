#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Components/MeshComponents/StaticMeshComponent.hpp>
#include <Renderer/Components/Misc/TransformComponent.hpp>
#include <Renderer/Components/MeshComponents/MeshInstanceComponent.hpp>
#include <Core/ECS/System/BaseSystem.hpp>

TRE_NS_START

class CommandBucket;

class MeshRenderSystem : public BaseSystem
{
public:
	MeshRenderSystem(CommandBucket* cmd_buffer = NULL, ShaderID shader_id = -1, MaterialID mat_id = 0);

	void OnUpdate(float dt) final;

public:
	CommandBucket* m_CommandBucket;
	ShaderID m_ShaderID;
	MaterialID m_MaterialID;
private:
	void SubmitInstanced(const InstancedMeshComponent& instance);
	void SubmitMesh(const StaticMeshComponent& mesh, const Mat4f& transform);
};

TRE_NS_END