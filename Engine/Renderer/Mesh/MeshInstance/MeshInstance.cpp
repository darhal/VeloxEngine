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

void MeshInstance::Submit(CommandBucket& CmdBucket)
{
	ModelRenderParams params = { 0, m_VaoID, (uint16)0 };
	Commands::PreInstancedDrawCallCmd* prepare_cmd = NULL;
	Commands::LinkCmd* link_cmd = NULL;
	CommandPacket* packet = NULL;
	BucketKey last_key = -1;

	for (SubMesh& obj : m_Meshs) {
		const Material& material = ResourcesManager::Instance().Get<Material>(ResourcesTypes::MATERIAL, obj.m_MaterialID);
		const RenderState& state = material.GetRenderStates();
		uint32 blend_dist = 0;

		params.material_id = obj.m_MaterialID;
		params.blend_dist = (uint16)blend_dist;
		BucketKey key = state.ToKey(material.GetTechnique().GetShaderID());

		if (key != last_key) {
			packet = CmdBucket.SubmitCommand<Commands::PreInstancedDrawCallCmd>(&prepare_cmd, key, params.ToKey());
			link_cmd = prepare_cmd;
			prepare_cmd->vao_id = m_VaoID;
			prepare_cmd->shader = &ResourcesManager::Instance().Get<ShaderProgram>(ResourcesTypes::SHADER, material.GetTechnique().GetShaderID());
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
	Commands::EditSubBuffer* cmd = op_queue.SubmitCommand<Commands::EditSubBuffer>();
	
	cmd->data = m_ModelTransformations + instance_id;
	cmd->offset = sizeof(Mat4f) * instance_id;
	cmd->size = sizeof(Mat4f);
	cmd->vbo = &manager.Get<VBO>(ResourcesTypes::VBO, m_DataVboID);
}

TRE_NS_END