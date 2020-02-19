#include "MeshInstance.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Materials/Material.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <Renderer/Backend/Keys/ModelKey.hpp>

TRE_NS_START

MeshInstance::MeshInstance(uint32 instance_count, VaoID vao_id, VboID vbo_id, Mat4f* transform) :
	m_ModelTransformations(transform), m_InstanceCount(instance_count), m_VaoID(vao_id), m_DataVboID(vbo_id)
{
}

void MeshInstance::Submit(CommandBucket& CmdBucket, ShaderID shader_id, MaterialID material_id)
{
	ResourcesManager& manager = ResourcesManager::Instance();
	ModelRenderParams params = { 0, m_VaoID, (uint16)0 };
	Commands::PreInstancedDrawCallCmd* prepare_cmd = NULL;
	Commands::LinkCmd* link_cmd = NULL;
	CommandPacket* packet = NULL;
	BucketKey last_key = -1;

	for (SubMesh& obj : m_Meshs) {
		const Material& material = manager.Get<Material>(ResourcesTypes::MATERIAL, material_id == -1 ? obj.m_MaterialID : material_id);
		const RenderState& state = material.GetRenderStates();
		uint32 blend_dist = 0;
		ShaderID shaderID = shader_id == ShaderID(-1) ? material.GetTechnique().GetShaderID() : shader_id;

		params.material_id = obj.m_MaterialID;
		params.blend_dist = (uint16)blend_dist;
		BucketKey key = state.ToKey(shaderID);

		if (key != last_key) {
			packet = CmdBucket.SubmitCommand<Commands::PreInstancedDrawCallCmd>(&prepare_cmd, key, params.ToKey());
			link_cmd = prepare_cmd;
			prepare_cmd->vao_id = m_VaoID;
			prepare_cmd->shader = &manager.Get<ShaderProgram>(ResourcesTypes::SHADER, shaderID);
			last_key = key;
		}

		if (obj.m_Geometry.m_Indexed) {
			Commands::InstancedDrawIndexedCmd* draw_cmd = packet->CreateCommand<Commands::InstancedDrawIndexedCmd>(params.ToKey());
			draw_cmd->mode = obj.m_Geometry.m_Primitive;
			draw_cmd->type = obj.m_Geometry.m_DataType;
			draw_cmd->count = obj.m_Geometry.m_Count;
			draw_cmd->offset = obj.m_Geometry.m_Offset;
			draw_cmd->instance_count = m_InstanceCount;

			draw_cmd->material = &material;
			draw_cmd->prepare_cmd = prepare_cmd;
			draw_cmd->next_cmd = NULL;

			link_cmd->next_cmd = Command::GetRawCommand(draw_cmd);
			link_cmd = draw_cmd;
		} else {
			Commands::InstancedDrawCmd* draw_cmd = packet->CreateCommand<Commands::InstancedDrawCmd>(params.ToKey());
			draw_cmd->mode = obj.m_Geometry.m_Primitive;
			draw_cmd->start = obj.m_Geometry.m_Start;
			draw_cmd->end = obj.m_Geometry.m_End;
			draw_cmd->instance_count = m_InstanceCount;

			draw_cmd->material = &material;
			draw_cmd->prepare_cmd = prepare_cmd;
			draw_cmd->next_cmd = NULL;

			link_cmd->next_cmd = Command::GetRawCommand(draw_cmd);
			link_cmd = draw_cmd;
		}
	}
}

void MeshInstance::UpdateTransforms(uint32 instance_id)
{
	ResourcesManager& manager = ResourcesManager::Instance();
	auto& op_queue = manager.GetContextOperationsQueue();
	Commands::EditSubBufferCmd* cmd = op_queue.SubmitCommand<Commands::EditSubBufferCmd>();
	
	cmd->data = m_ModelTransformations + instance_id;
	cmd->offset = sizeof(Mat4f) * instance_id;
	cmd->size = sizeof(Mat4f);
	cmd->vbo = &manager.Get<VBO>(ResourcesTypes::VBO, m_DataVboID);
}

TRE_NS_END