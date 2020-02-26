#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderAPI/VertexArray/VAO.hpp>
#include <Renderer/Common/Common.hpp>
#include <Renderer/MeshLoader/ModelData/ModelData.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <Renderer/Materials/AbstractMaterial/AbstractMaterial.hpp>
#include <Renderer/MeshLoader/VertexData/VertexData.hpp>
#include <Renderer/Mesh/StaticMesh/StaticMesh.hpp>
#include <Renderer/Mesh/MeshInstance/MeshInstance.hpp>
#include <Renderer/MeshLoader/ModelData/ModelMaterialData.hpp>

#include <Renderer/Components/MeshComponents/StaticMeshComponent.hpp>
#include <Renderer/Components/MeshComponents/MeshInstanceComponent.hpp>

TRE_NS_START

class Model
{
public:
	enum VertexAttributes
	{
		POSITION = 0, 
		NORMAL = 1, 
		TEXTURE_COORDINATES = 2,
		TANGENT = 3,
		BITANGET = 4,
		TRANSFORM = 5
	};

	enum Processing
	{
		NONE = 0x0,
		CALC_TANGET = 0x1,
		CALC_BITANGET = 0x2,
	};

	Model(const ModelData& data, uint32 processing = 0);

	Model(Vector<VertexData>& ver_data, const Vector<ModelMaterialData>& mat_vec, Vector<uint32>* indices = NULL, uint32 processing = 0);

	StaticMesh LoadMesh(ShaderID shader_id = 0);

	StaticMeshComponent LoadMeshComponent(ShaderID shader_id = 0);

	MeshInstance LoadInstancedMesh(uint32 instance_count, ShaderID shader_id = 0, Mat4f* transforms = NULL);

	EntityID LoadInstancedMeshComponent(uint32 instance_count, ShaderID shader_id = 0, Mat4f* transforms = NULL);

	Vector<SubMesh> LoadSubmeshs(ShaderID shader_id);

	Vector<NormalVertexData> ProcessModel(uint32 processing, const Vector<VertexData>& vertices, Vector<uint32>* indices = NULL);

	Vector<ModelMaterialData>& GetMaterials() { return m_Materials; }
private:
	Commands::CreateVAOCmd* LoadFromSettings(const ModelData& data);
	Commands::CreateVAOCmd* LoadFromVertexData(uint32 processing, Vector<VertexData>& ver_data, Vector<uint32>* indices);

	Commands::CreateVAOCmd* m_CreateVaoCmd;
	Vector<ModelMaterialData> m_Materials;
	uint32 m_VertexCount;
	VaoID m_VaoID;
	bool m_HaveIndexBuffer;
};

TRE_NS_END