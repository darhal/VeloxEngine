#include "InstancedTransformSystem.hpp"
#include <Core/ECS/ArchetypeQuerry/ArchetypeQuerry.hpp>
#include <Core/ECS/ECS/ECS.hpp>
#include <Renderer/Components/Misc/SceneTagComponent.hpp>
#include <Renderer/Components/Misc/TransformComponent.hpp>
#include <Renderer/Components/Misc/UpdateTagComponent.hpp>
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>

TRE_NS_START

InstancedTransformSystem::InstancedTransformSystem()
{
	m_ComponentGroup = ComponentGroup(ArchetypeQuerry{
		ECS::GetSignature<UpdateTag, SceneTag, MeshInstanceComponent, TransformComponent>(),
		ECS::GetEmptySignature(),
		ECS::GetEmptySignature()
	});
}

void InstancedTransformSystem::OnUpdate(float dt)
{
	ResourcesManager& manager  = ResourcesManager::Instance();
	EntityManager& ent_manager = manager.GetRenderWorld().GetEntityManager();
	ContextOperationQueue& op_queue = manager.GetContextOperationsQueue();

	for (const Archetype* arch : m_ComponentGroup.GetArchetypes()) {
		const Archetype& archetype = *arch;

		if (archetype.HasComponentType<MeshInstanceComponent>() && archetype.HasComponentType<TransformComponent>() &&
			archetype.HasComponentType<SceneTag>() && archetype.HasComponentType<UpdateTag>()) 
		{
			for (const ArchetypeChunk& chunk : archetype) {
				for (uint32 i = 0; i < chunk.GetEntitiesCount(); i++) {
					const MeshInstanceComponent& static_mesh = chunk.GetComponentByInternalID<MeshInstanceComponent>(i);
					const TransformComponent& transform = chunk.GetComponentByInternalID<TransformComponent>(i);
					Entity& instance_ent = ent_manager.GetEntityByID(static_mesh.InstanceID);
					const InstancedMeshComponent* instance = instance_ent.GetComponent<InstancedMeshComponent>();

					if (instance) {
						this->UpdateInstanceTransform(manager, op_queue, *instance, static_mesh, transform.transform_matrix);
					}
					
					// TODO: Submit command on the system command buffer to remove the UpdateTag
				}
			}
		}
	}
}

void InstancedTransformSystem::UpdateInstanceTransform(
	ResourcesManager& manager, ContextOperationQueue& op_queue,
	const InstancedMeshComponent& instance, const MeshInstanceComponent& mesh, const Mat4f& transform)
{
	// TODO: Compare Data VBO
	Commands::EditSubBufferCmd* cmd = op_queue.SubmitCommand<Commands::EditSubBufferCmd>();

	cmd->data = &transform;
	cmd->offset = sizeof(Mat4f) * mesh.InstanceID;
	cmd->size = sizeof(Mat4f);
	cmd->vbo = &manager.Get<VBO>(instance.DataVboID);
}

TRE_NS_END