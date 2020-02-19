#include "MeshSystem.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/CommandBucket/CommandBucket.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <Renderer/Backend/Keys/ModelKey.hpp>
#include <Renderer/Materials/Material.hpp>

TRE_NS_START

MeshSystem::MeshSystem(CommandBucket* cmd_buffer) : m_CommandBucket(cmd_buffer)
{
}

void MeshSystem::UpdateComponents(float dt, Archetype& arche)
{
	for (const ArchetypeChunk& chunk : arche) {
		for (uint32 i = 0; i < chunk.GetEntitiesCount(); i++) {
			TransformComponent& transform = chunk.GetComponentByInternalID<TransformComponent>(i);
			const StaticMeshComponent& static_mesh = chunk.GetComponentByInternalID<StaticMeshComponent>(i);
			
			this->Submit(static_mesh, transform);
		}
	}
}

void MeshSystem::Submit(const StaticMeshComponent& mesh, const TransformComponent& transform)
{
	ResourcesManager& manager = ResourcesManager::Instance();
	ModelRenderParams params = { 0, mesh.vao_id, (uint16)0 };
	Commands::PreDrawCallCmd* prepare_cmd = NULL;
	Commands::LinkCmd* link_cmd = NULL;
	CommandPacket* packet = NULL;
	BucketKey last_key = -1;
	ShaderID shader_id = ShaderID(-1);

	for (SubMesh& obj : mesh.submeshs) {
		const Material& material = manager.Get<Material>(obj.m_MaterialID);
		const RenderState& state = material.GetRenderStates();
		ShaderID shaderID = shader_id == ShaderID(-1) ? material.GetTechnique().GetShaderID() : shader_id;
		BucketKey key = state.ToKey(shaderID);
		params.material_id = obj.m_MaterialID;

		if (state.blend_enabled) {
			uint8 bits_to_shift = sizeof(float) * BITS_PER_BYTE - RenderSettings::BLEND_DISTANCE_BITS;
			float distance_from_cam = (transform.transform_matrix.v3.xyz - m_CommandBucket->GetCamera().Position).length();

			UConverter<float, uint32> compressed_float;
			compressed_float.first_rep = Math::CompressFloat(distance_from_cam, bits_to_shift);
			params.blend_dist = (uint16)(static_cast<uint32>(compressed_float.second_rep) >> bits_to_shift);
		}

		if (key != last_key) {
			packet = m_CommandBucket->SubmitCommand<Commands::PreDrawCallCmd>(&prepare_cmd, key, params.ToKey());
			link_cmd = prepare_cmd;
			prepare_cmd->vao_id = mesh.vao_id;
			prepare_cmd->shader = &manager.Get<ShaderProgram>(shaderID);
			prepare_cmd->model = transform.transform_matrix;
			last_key = key;
		}

		if (obj.m_Geometry.m_Indexed) {
			Commands::DrawIndexedCmd* draw_cmd = packet->CreateCommand<Commands::DrawIndexedCmd>(params.ToKey());
			draw_cmd->mode = obj.m_Geometry.m_Primitive;
			draw_cmd->type = obj.m_Geometry.m_DataType;
			draw_cmd->count = obj.m_Geometry.m_Count;
			draw_cmd->offset = obj.m_Geometry.m_Offset;

			draw_cmd->material = &material;
			draw_cmd->prepare_cmd = prepare_cmd;
			draw_cmd->next_cmd = NULL;

			link_cmd->next_cmd = Command::GetRawCommand(draw_cmd);
			link_cmd = draw_cmd;
		} else {
			Commands::DrawCmd* draw_cmd = packet->CreateCommand<Commands::DrawCmd>(params.ToKey());
			draw_cmd->mode = obj.m_Geometry.m_Primitive;
			draw_cmd->start = obj.m_Geometry.m_Start;
			draw_cmd->end = obj.m_Geometry.m_End;

			draw_cmd->material = &material;
			draw_cmd->prepare_cmd = prepare_cmd;
			draw_cmd->next_cmd = NULL;

			link_cmd->next_cmd = Command::GetRawCommand(draw_cmd);
			link_cmd = draw_cmd;
		}
	}
}

TRE_NS_END
