#include "StaticMesh.hpp"
#include "Core/Misc/Utils/Common.hpp"

#include <RenderEngine/Managers/RenderManager/RenderManager.hpp>
#include "RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/RenderCommandBucket/RenderCommandBucket.hpp"

TRE_NS_START

void StaticMesh::Submit(RenderCommandBuffer& CmdBucket, const Vec3f& CameraPosition)
{
	for (RawSubMesh& obj : m_Meshs) {  
        const Material& material = ResourcesManager::GetGRM().Get<Material>(obj.m_MaterialID);
		uint32 blend_dist = 0;

		if (material.GetRenderStates().blend_enabled){
			uint8 bits_to_shift =  sizeof(float) * BITS_PER_BYTE - RenderSettings::BLEND_DISTANCE_BITS;
			float distance_from_cam = (m_ModelTransformation.v3.xyz - CameraPosition).length();

			UConverter<float, uint32> compressed_float;
			compressed_float.first_rep = CompressFloat(distance_from_cam, bits_to_shift);

			blend_dist = (1 << RenderSettings::BLEND_DISTANCE_BITS) | (static_cast<uint32>(compressed_float.second_rep) >> bits_to_shift);

			/*UConverter<float, uint32> converter1;
			converter1.first_rep = distance_from_cam;
			printf("Real distance : %f (%x) | Compressed distancec = %f (%x)\n", converter1.first_rep, converter1.second_rep, compressed_float.first_rep, compressed_float.second_rep);
			std::cout << "Real distance : " << std::bitset<32>(converter1.second_rep) << " | Compressed distance : " << std::bitset<32>(compressed_float.second_rep) << std::endl;
			std::cout << "BLEND_DIST KEY = : " << std::bitset<32>(blend_dist) << std::endl; */
		}

		// Submit for shadow mapping
		uint32 cmd_id;
		MaterialID shadow_material_id = RenderManager::GetRenderer().GetShadowMaterialID();
		const Material& shadow_mat = ResourcesManager::GetGRM().Get<Material>(shadow_material_id);
		typename RenderCommandBuffer::Key key = CmdBucket.GenerateKey(shadow_mat.GetTechnique().GetShaderID(), m_VaoID, shadow_material_id, blend_dist);
		Commands::DrawIndexedCmd* draw_cmd = CmdBucket.template CreateCommandOnRenderTarget<Commands::DrawIndexedCmd>(key, 0, 0, &cmd_id);
		draw_cmd->mode = obj.m_Geometry.m_Primitive;
		draw_cmd->type = obj.m_Geometry.m_DataType;
		draw_cmd->count = obj.m_Geometry.m_Count;
		draw_cmd->offset = obj.m_Geometry.m_Offset;
		draw_cmd->model = &m_ModelTransformation;
		
		// Submit for main rendering
        key = CmdBucket.GenerateKey(material.GetTechnique().GetShaderID(), m_VaoID, obj.m_MaterialID, blend_dist);
		CmdBucket.AddCommandToRenderTarget(cmd_id, key, CmdBucket.GetRenderTargetsCount() - 1);
	}
}

void StaticMesh::Submit(FramebufferCommandBuffer& CmdBucket, RenderTargetID render_target_id, FramebufferCommandBuffer::FrameBufferPiriority::Piroirty_t piroirty)
{
	for (RawSubMesh& obj : m_Meshs) {
		const Material& material = ResourcesManager::GetGRM().Get<Material>(obj.m_MaterialID);

		auto key = CmdBucket.GenerateKey({ render_target_id, piroirty }, material.GetTechnique().GetShaderID(), m_VaoID, obj.m_MaterialID);
		auto draw_cmd = CmdBucket.AddCommand<Commands::DrawIndexedCmd>(key);

		draw_cmd->mode = obj.m_Geometry.m_Primitive;
		draw_cmd->type = obj.m_Geometry.m_DataType;
		draw_cmd->count = obj.m_Geometry.m_Count;
		draw_cmd->offset = obj.m_Geometry.m_Offset;
		draw_cmd->model = &m_ModelTransformation;
	}
}

TRE_NS_END