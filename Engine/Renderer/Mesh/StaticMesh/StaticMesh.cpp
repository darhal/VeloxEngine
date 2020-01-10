#include "StaticMesh.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Materials/Material.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <Renderer/Backend/Keys/ModelKey.hpp>

TRE_NS_START

void StaticMesh::Submit(CommandBucket& CmdBucket)
{
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

		ModelRenderParams params = { obj.m_MaterialID, m_VaoID, (uint16)blend_dist };
		Commands::DrawIndexedCmd* draw_cmd =  CmdBucket.SubmitCommand<Commands::DrawIndexedCmd>(state, material.GetTechnique().GetShaderID(), params.ToKey());
		draw_cmd->mode = obj.m_Geometry.m_Primitive;
		draw_cmd->type = obj.m_Geometry.m_DataType;
		draw_cmd->count = obj.m_Geometry.m_Count;
		draw_cmd->offset = obj.m_Geometry.m_Offset;
		draw_cmd->model = &m_ModelTransformation;
	}
}

TRE_NS_END