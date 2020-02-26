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
	World* world = &manager.GetRenderWorld();
	EntityManager& ent_manager = world->GetEntityManager();
	ContextOperationQueue& op_queue = manager.GetContextOperationsQueue();
	VboID last_vbo_id = VboID(-1);
	Commands::BindVBO* cmd = NULL;

	for (const Archetype* arch : m_ComponentGroup.GetArchetypes()) {
		const Archetype& archetype = *arch;

		if (!archetype.IsEmpty() && archetype.HasComponentType<MeshInstanceComponent>() && archetype.HasComponentType<TransformComponent>() &&
			archetype.HasComponentType<SceneTag>() && archetype.HasComponentType<UpdateTag>()) 
		{
			for (const ArchetypeChunk& chunk : archetype) {
				for (uint32 i = 0; i < chunk.GetEntitiesCount(); i++) {
					EntityID ent_id = chunk.GetEntityID(i);
					const MeshInstanceComponent& static_mesh = chunk.GetComponentByInternalID<MeshInstanceComponent>(i);
					const TransformComponent& transform = chunk.GetComponentByInternalID<TransformComponent>(i);
					Entity& instance_ent = ent_manager.GetEntityByID(static_mesh.InstanceModel);
					const InstancedMeshComponent* instance = instance_ent.GetComponent<InstancedMeshComponent>();
					VboID vbo_id = instance->DataVboID;

					if (last_vbo_id != vbo_id || !cmd) {
						cmd = op_queue.SubmitCommand<Commands::BindVBO>();
						cmd->vbo = &manager.Get<VBO>(vbo_id);
						last_vbo_id = vbo_id;
					}

					if (instance) {
						this->UpdateInstanceTransform(manager, op_queue, cmd->vbo, static_mesh.InstanceID, transform.transform_matrix);
					}
					
					// Submit command on the system command buffer to remove the UpdateTag
					auto* rm_comp_cmd = m_CommandRecord.SubmitCommand<ECSCommands::RemoveComponentCmd>();
					rm_comp_cmd->word = world;
					rm_comp_cmd->entity_id = ent_id;
					rm_comp_cmd->component_type_id = UpdateTag::ID;
				}
			}
		}
	}
}

void InstancedTransformSystem::UpdateInstanceTransform(
	ResourcesManager& manager, ContextOperationQueue& op_queue, VBO* vbo, uint32 instance_id, const Mat4f& transform)
{
	Commands::EditSubBufferCmd* cmd = op_queue.SubmitCommand<Commands::EditSubBufferCmd>();

	cmd->data = &transform;
	cmd->offset = sizeof(Mat4f) * instance_id;
	cmd->size = sizeof(Mat4f);
	cmd->vbo = vbo;
}

TRE_NS_END