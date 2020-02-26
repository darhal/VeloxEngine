#include "MeshRenderSystem.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/CommandBucket/CommandBucket.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <Renderer/Backend/Keys/ModelKey.hpp>
#include <Renderer/Materials/Material.hpp>
#include <Renderer/Components/Misc/SceneTagComponent.hpp>

TRE_NS_START

MeshRenderSystem::MeshRenderSystem(CommandBucket* cmd_buffer, ShaderID shader_id, MaterialID mat_id) :
	m_CommandBucket(cmd_buffer), m_ShaderID(shader_id), m_MaterialID(mat_id)
{
	m_ComponentGroup =
		ComponentGroup(ArchetypeQuerry{
			ECS::GetSignature<SceneTag>(),
			ECS::GetSignature<StaticMeshComponent, InstancedMeshComponent>(),
			ECS::GetEmptySignature()
		}
	);
}

void MeshRenderSystem::OnUpdate(float dt)
{
	for (const Archetype* arch : m_ComponentGroup.GetArchetypes()) {
		const Archetype& archetype = *arch;

		if (archetype.IsEmpty()) {
			continue;
		}else if (archetype.HasComponentType<StaticMeshComponent>() && archetype.HasComponentType<TransformComponent>()) {
			for (const ArchetypeChunk& chunk : archetype) {
				for (uint32 i = 0; i < chunk.GetEntitiesCount(); i++) {
					const TransformComponent& transform = chunk.GetComponentByInternalID<TransformComponent>(i);
					const StaticMeshComponent& static_mesh = chunk.GetComponentByInternalID<StaticMeshComponent>(i);

					this->SubmitMesh(static_mesh, transform.transform_matrix);
				}
			}
		} else if (archetype.HasComponentType<InstancedMeshComponent>()){ // Its InstancedMeshComponent since we want to issue only one draw call
			for (const ArchetypeChunk& chunk : archetype) {
				for (uint32 i = 0; i < chunk.GetEntitiesCount(); i++) {
					const InstancedMeshComponent& instanced_mesh = chunk.GetComponentByInternalID<InstancedMeshComponent>(i);
					this->SubmitInstanced(instanced_mesh);
				}
			}
		}
	}
}

/*
TODO:
- Add tags to instanced meshs that their transform matrix changed
- Make system to update them
- Remove the tag from them
*/
void MeshRenderSystem::SubmitInstanced(const InstancedMeshComponent& mesh)
{
	ResourcesManager& manager = ResourcesManager::Instance();
	ModelRenderParams params = { 0, mesh.VaoID, (uint16)0 };
	Commands::PreInstancedDrawCallCmd* prepare_cmd = NULL;
	Commands::LinkCmd* link_cmd = NULL;
	CommandPacket* packet = NULL;
	BucketKey last_key = -1;

	for (SubMesh& obj : mesh.Submeshs) {
		const Material& material = manager.Get<Material>(m_MaterialID == -1 ? obj.m_MaterialID : m_MaterialID);
		const RenderState& state = material.GetRenderStates();
		uint32 blend_dist = 0;
		ShaderID shaderID = m_ShaderID == ShaderID(-1) ? material.GetTechnique().GetShaderID() : m_ShaderID;

		params.material_id = obj.m_MaterialID;
		params.blend_dist = (uint16)blend_dist;
		BucketKey key = state.ToKey(shaderID);

		if (key != last_key) {
			packet = m_CommandBucket->SubmitCommand<Commands::PreInstancedDrawCallCmd>(&prepare_cmd, key, params.ToKey());
			link_cmd = prepare_cmd;
			prepare_cmd->vao_id = mesh.VaoID;
			prepare_cmd->shader = &manager.Get<ShaderProgram>(shaderID);
			last_key = key;
		}

		if (obj.m_Geometry.m_Indexed) {
			Commands::InstancedDrawIndexedCmd* draw_cmd = packet->CreateCommand<Commands::InstancedDrawIndexedCmd>(params.ToKey());
			draw_cmd->mode = obj.m_Geometry.m_Primitive;
			draw_cmd->type = obj.m_Geometry.m_DataType;
			draw_cmd->count = obj.m_Geometry.m_Count;
			draw_cmd->offset = obj.m_Geometry.m_Offset;
			draw_cmd->instance_count = mesh.InstanceCount;

			draw_cmd->material = m_MaterialID == MaterialID(-1) ? NULL : &material;
			draw_cmd->prepare_cmd = prepare_cmd;
			draw_cmd->next_cmd = NULL;

			link_cmd->next_cmd = Command::GetRawCommand(draw_cmd);
			link_cmd = draw_cmd;
		} else {
			Commands::InstancedDrawCmd* draw_cmd = packet->CreateCommand<Commands::InstancedDrawCmd>(params.ToKey());
			draw_cmd->mode = obj.m_Geometry.m_Primitive;
			draw_cmd->start = obj.m_Geometry.m_Start;
			draw_cmd->end = obj.m_Geometry.m_End;
			draw_cmd->instance_count = mesh.InstanceCount;

			draw_cmd->material = m_MaterialID == MaterialID(-1) ? NULL : &material;
			draw_cmd->prepare_cmd = prepare_cmd;
			draw_cmd->next_cmd = NULL;

			link_cmd->next_cmd = Command::GetRawCommand(draw_cmd);
			link_cmd = draw_cmd;
		}
	}
}

void MeshRenderSystem::SubmitMesh(const StaticMeshComponent& mesh, const Mat4f& transform)
{
	ResourcesManager& manager = ResourcesManager::Instance();
	ModelRenderParams params = { 0, mesh.VaoID, (uint16)0 };
	Commands::PreDrawCallCmd* prepare_cmd = NULL;
	Commands::LinkCmd* link_cmd = NULL;
	CommandPacket* packet = NULL;
	BucketKey last_key = -1;

	for (SubMesh& obj : mesh.Submeshs) {
		const Material& material = manager.Get<Material>(obj.m_MaterialID);
		const RenderState& state = material.GetRenderStates();
		ShaderID shaderID = m_ShaderID == ShaderID(-1) ? material.GetTechnique().GetShaderID() : m_ShaderID;
		BucketKey key = state.ToKey(shaderID);
		params.material_id = obj.m_MaterialID;

		if (state.blend_enabled) {
			uint8 bits_to_shift = sizeof(float) * BITS_PER_BYTE - RenderSettings::BLEND_DISTANCE_BITS;
			float distance_from_cam = (transform.v3.xyz - m_CommandBucket->GetCamera().Position).length();

			UConverter<float, uint32> compressed_float;
			compressed_float.first_rep = Math::CompressFloat(distance_from_cam, bits_to_shift);
			params.blend_dist = (uint16)(static_cast<uint32>(compressed_float.second_rep) >> bits_to_shift);
		}

		if (key != last_key) {
			packet = m_CommandBucket->SubmitCommand<Commands::PreDrawCallCmd>(&prepare_cmd, key, params.ToKey());
			link_cmd = prepare_cmd;
			prepare_cmd->vao_id = mesh.VaoID;
			prepare_cmd->shader = &manager.Get<ShaderProgram>(shaderID);
			prepare_cmd->model = transform;
			last_key = key;
		}

		if (obj.m_Geometry.m_Indexed) {
			Commands::DrawIndexedCmd* draw_cmd = packet->CreateCommand<Commands::DrawIndexedCmd>(params.ToKey());
			draw_cmd->mode = obj.m_Geometry.m_Primitive;
			draw_cmd->type = obj.m_Geometry.m_DataType;
			draw_cmd->count = obj.m_Geometry.m_Count;
			draw_cmd->offset = obj.m_Geometry.m_Offset;

			draw_cmd->material = m_MaterialID == MaterialID(-1) ? NULL : &material;
			draw_cmd->prepare_cmd = prepare_cmd;
			draw_cmd->next_cmd = NULL;

			link_cmd->next_cmd = Command::GetRawCommand(draw_cmd);
			link_cmd = draw_cmd;
		} else {
			Commands::DrawCmd* draw_cmd = packet->CreateCommand<Commands::DrawCmd>(params.ToKey());
			draw_cmd->mode = obj.m_Geometry.m_Primitive;
			draw_cmd->start = obj.m_Geometry.m_Start;
			draw_cmd->end = obj.m_Geometry.m_End;

			draw_cmd->material = m_MaterialID == MaterialID(-1) ? NULL : &material;
			draw_cmd->prepare_cmd = prepare_cmd;
			draw_cmd->next_cmd = NULL;

			link_cmd->next_cmd = Command::GetRawCommand(draw_cmd);
			link_cmd = draw_cmd;
		}
	}
}

TRE_NS_END
