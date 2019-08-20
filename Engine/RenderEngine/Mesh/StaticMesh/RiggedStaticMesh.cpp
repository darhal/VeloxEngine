#include "RiggedStaticMesh.hpp"
#include "RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/RenderCommandBucket/RenderCommandBucket.hpp"

TRE_NS_START

void RiggedStaticMesh::Submit(RenderCommandBuffer& CmdBucket, const Vec3f& CameraPosition)
{
	for (RiggedRawSubMesh& obj : m_Meshs) {
        const Material& material = ResourcesManager::GetGRM().Get<Material>(obj.m_MaterialID);
		uint32 blend_dist = 0;

		if (material.GetRenderStates().blend_enabled){
			uint8 bits_to_shift =  sizeof(float) * BITS_PER_BYTE - RenderSettings::BLEND_DISTANCE_BITS;
			float distance_from_cam = (obj.m_ModelTransformation.v3.xyz - CameraPosition).length();

			UConverter<float, uint32> compressed_float;
			compressed_float.first_rep = CompressFloat(distance_from_cam, bits_to_shift);

			blend_dist = (1 << RenderSettings::BLEND_DISTANCE_BITS) | (static_cast<uint32>(compressed_float.second_rep) >> bits_to_shift);
		}

        typename RenderCommandBuffer::Key key = CmdBucket.GenerateKey(material.GetTechnique().GetShaderID(), m_VaoID, obj.m_MaterialID, blend_dist);
		Commands::DrawIndexedCmd* draw_cmd = CmdBucket.template AddCommand<Commands::DrawIndexedCmd>(key);
		draw_cmd->mode = obj.m_Geometry.m_Primitive;
		draw_cmd->type = obj.m_Geometry.m_DataType;
		draw_cmd->count = obj.m_Geometry.m_Count;
		draw_cmd->offset = obj.m_Geometry.m_Offset;
        draw_cmd->model = &obj.m_ModelTransformation;
	}
}

TRE_NS_END