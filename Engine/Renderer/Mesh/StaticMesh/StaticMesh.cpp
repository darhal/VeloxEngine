#include "StaticMesh.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Materials/Material.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <Renderer/Backend/Keys/ModelKey.hpp>

TRE_NS_START

void StaticMesh::Submit(CommandBucket& CmdBucket)
{
	ModelRenderParams params = { 0, m_VaoID, (uint16)0 };
	Commands::PreDrawCallCmd* prepare_cmd = NULL;
	Commands::LinkCmd* link_cmd = NULL;
	CommandPacket* packet = NULL;
	BucketKey last_key = -1;

	for (SubMesh& obj : m_Meshs) {
		const Material& material = ResourcesManager::Instance().Get<Material>(ResourcesTypes::MATERIAL, obj.m_MaterialID);
		const RenderState& state = material.GetRenderStates();
		uint32 blend_dist = 0;

		if (state.blend_enabled) {
			uint8 bits_to_shift = sizeof(float) * BITS_PER_BYTE - RenderSettings::BLEND_DISTANCE_BITS;
			float distance_from_cam = (m_ModelTransformation.v3.xyz - CmdBucket.GetCamera().Position).length();

			UConverter<float, uint32> compressed_float;
			compressed_float.first_rep = Math::CompressFloat(distance_from_cam, bits_to_shift);

			blend_dist = (static_cast<uint32>(compressed_float.second_rep) >> bits_to_shift);

			/*UConverter<float, uint32> converter1;
			converter1.first_rep = distance_from_cam;
			printf("Real distance : %f (%x) | Compressed distancec = %f (%x)\n", converter1.first_rep, converter1.second_rep, compressed_float.first_rep, compressed_float.second_rep);
			std::cout << "Real distance : " << std::bitset<32>(converter1.second_rep) << " | Compressed distance : " << std::bitset<32>(compressed_float.second_rep) << std::endl;
			std::cout << "BLEND_DIST KEY = : " << std::bitset<32>(blend_dist) << std::endl; */
		}

		params.material_id = obj.m_MaterialID;
		params.blend_dist = (uint16)blend_dist;
		BucketKey key = state.ToKey(material.GetTechnique().GetShaderID());

		if (key != last_key) {
			packet = CmdBucket.SubmitCommand<Commands::PreDrawCallCmd>(&prepare_cmd, key, params.ToKey());
			link_cmd = prepare_cmd;
			prepare_cmd->vao_id = m_VaoID;
			prepare_cmd->shader = &ResourcesManager::Instance().Get<ShaderProgram>(ResourcesTypes::SHADER, material.GetTechnique().GetShaderID());
			prepare_cmd->model = m_ModelTransformation;
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