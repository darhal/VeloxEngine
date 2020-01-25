#include "Model.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/Commands/Command.hpp>
#include <Renderer/Materials/Material.hpp>

TRE_NS_START

Model::Model(const ModelData& data, uint32 processing)
{
	m_CreateVaoCmd = LoadFromSettings(data);
	ResourcesManager& manager = ResourcesManager::Instance();

	if (data.indices && data.indexSize) {
		m_VertexCount = data.indexSize; // Get the vertex Count!
		m_HaveIndexBuffer = true;

		VboID indexVboID = manager.CreateResource<VBO>(ResourcesTypes::VBO);
		Commands::CreateIndexBufferCmd* create_index_cmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::CreateIndexBufferCmd>();
		// m_VboIDs.EmplaceBack(indexVboID);
		create_index_cmd->vao = m_CreateVaoCmd->vao;
		create_index_cmd->settings = VertexSettings::VertexBufferData(data.indices, data.indexSize, &manager.Get<VBO>(ResourcesTypes::VBO, indexVboID));

	} else {
		m_VertexCount = data.vertexSize / 3LLU; // Get the vertex Count!
		m_HaveIndexBuffer = false;
	}

	m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
}

Model::Model(Vector<VertexData>& ver_data, const Vector<ModelMaterialData>& mat_vec, Vector<uint32>* indices, uint32 processing)
{
	ASSERTF(ver_data.Size() == 0, "Attempt to create a ModelLoader with empty vertecies!");
	m_CreateVaoCmd = LoadFromVertexData(processing, ver_data, indices);
	ResourcesManager& manager = ResourcesManager::Instance();

	if (indices) {
		m_VertexCount = (uint32)indices->Size(); // Get the vertex Count!
		m_HaveIndexBuffer = true;

		VboID indexVboID = manager.CreateResource<VBO>(ResourcesTypes::VBO);
		Commands::CreateIndexBufferCmd* create_index_cmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::CreateIndexBufferCmd>();
		// m_VboIDs.EmplaceBack(indexVboID);
		create_index_cmd->vao = m_CreateVaoCmd->vao;
		create_index_cmd->settings = VertexSettings::VertexBufferData(*indices, &manager.Get<VBO>(ResourcesTypes::VBO, indexVboID));
		
		m_Materials = mat_vec;
	} else {
		m_VertexCount = (uint32)ver_data.Size() / 8LLU; // Get the vertex Count!
		m_HaveIndexBuffer = false;
	}

	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
	}
}

Commands::CreateVAOCmd* Model::LoadFromSettings(const ModelData& data)
{
	ASSERTF((data.vertexSize == 0 || data.vertices == NULL), "Attempt to create a ModelLoader with empty vertecies!");
	ResourcesManager& manager = ResourcesManager::Instance();
	Commands::CreateVAOCmd* createVaoCmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::CreateVAOCmd>();

	m_VaoID = manager.CreateResource<VAO>(ResourcesTypes::VAO);
	createVaoCmd->vao = &manager.Get<VAO>(ResourcesTypes::VAO, m_VaoID);
	createVaoCmd->settings = VertexSettings();
	uint8 layout = 0;
	
	// Fill vertex:
	VboID vboID = manager.CreateResource<VBO>(ResourcesTypes::VBO);
	// m_VboIDs.EmplaceBack(vboID);
	createVaoCmd->settings.vertices_data.EmplaceBack(data.vertices, data.vertexSize, &manager.Get<VBO>(ResourcesTypes::VBO, vboID));
	createVaoCmd->settings.attributes.EmplaceBack(createVaoCmd->settings.vertices_data.Size() - 1, layout, 3, 0, 0);

	if (data.normalSize != 0 && data.normals != NULL) { // Fill normals if availble
		vboID = manager.CreateResource<VBO>(ResourcesTypes::VBO);
		// m_VboIDs.EmplaceBack(vboID);

		// Fill normals:
		createVaoCmd->settings.vertices_data.EmplaceBack(data.normals, data.normalSize, &manager.Get<VBO>(ResourcesTypes::VBO, vboID));
		createVaoCmd->settings.attributes.EmplaceBack(createVaoCmd->settings.vertices_data.Size() - 1, ++layout, 3, 0, 0);
	}

	if (data.textureSize != 0 && data.textures != NULL) { // Fill Texture if availble
		vboID = manager.CreateResource<VBO>(ResourcesTypes::VBO);
		// m_VboIDs.EmplaceBack(vboID);

		// Fill textures:
		createVaoCmd->settings.vertices_data.EmplaceBack(data.textures, data.textureSize, &manager.Get<VBO>(ResourcesTypes::VBO, vboID));
		createVaoCmd->settings.attributes.EmplaceBack(createVaoCmd->settings.vertices_data.Size() - 1, ++layout, 2, 0, 0);
	}

	return createVaoCmd;
}

Commands::CreateVAOCmd* Model::LoadFromVertexData(uint32 processing, Vector<VertexData>& ver_data, Vector<uint32>* indices)
{
	ASSERTF((ver_data.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	Vector<NormalVertexData> tangents = ProcessModel(processing, ver_data, indices);

	ResourcesManager& manager = ResourcesManager::Instance();
	Commands::CreateVAOCmd* createVaoCmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::CreateVAOCmd>();
	m_VaoID = manager.CreateResource<VAO>(ResourcesTypes::VAO);

	VboID vboID = manager.CreateResource<VBO>(ResourcesTypes::VBO);
	VBO* vbo = &manager.Get<VBO>(ResourcesTypes::VBO, vboID);

	createVaoCmd->vao = &manager.Get<VAO>(ResourcesTypes::VAO, m_VaoID);
	createVaoCmd->settings = VertexSettings();
	createVaoCmd->settings.vertices_data.EmplaceBack(ver_data, vbo);

	uint32 attrib_data_sizes[] = { 3, 3, 2 };
	uint32 offset = 0;
	uint8 attrib_index = 0;

	for (const uint32 size : attrib_data_sizes) {
		createVaoCmd->settings.attributes.EmplaceBack(createVaoCmd->settings.vertices_data.Size() - 1, attrib_index, size, 8, offset);
		attrib_index++;
		offset += size;
	}

	if (processing) {
		VboID vboID = manager.CreateResource<VBO>(ResourcesTypes::VBO);
		VBO* vbo = &manager.Get<VBO>(ResourcesTypes::VBO, vboID);
		createVaoCmd->settings.vertices_data.EmplaceBack(tangents, vbo);

		if (processing & CALC_TANGET) {
			createVaoCmd->settings.attributes.EmplaceBack(createVaoCmd->settings.vertices_data.Size() - 1, attrib_index++, 3, 6, 0);
		}

		if (processing & CALC_BITANGET) {;
			createVaoCmd->settings.attributes.EmplaceBack(createVaoCmd->settings.vertices_data.Size() - 1, attrib_index++, 3, 6, 3);
		}
	}

	return createVaoCmd;
}

StaticMesh Model::LoadMesh(ShaderID shader_id)
{
	StaticMesh mesh(m_VaoID);
	int32 lastVertexCount = 0;
	ResourcesManager& manager = ResourcesManager::Instance();

	if (m_HaveIndexBuffer) { // Not = to 0 means that its indexed.
		for (const ModelMaterialData& mat : m_Materials) {
			PrimitiveGeometry model_geo(DataType::UINT, mat.vcount, lastVertexCount * sizeof(uint32));
			MaterialID matID = manager.CreateResource<Material>(ResourcesTypes::MATERIAL, mat.material, shader_id);
			mesh.AddSubMesh(model_geo, matID);
			lastVertexCount += mat.vcount;
		}
	} else {
		for (const ModelMaterialData& mat : m_Materials) {
			PrimitiveGeometry model_geo(lastVertexCount, mat.vcount);
			MaterialID matID = manager.CreateResource<Material>(ResourcesTypes::MATERIAL, mat.material, shader_id);
			mesh.AddSubMesh(model_geo, matID);
			lastVertexCount += mat.vcount;
		}
	}

	return mesh;
}

StaticMeshComponent Model::LoadMeshComponent(ShaderID shader_id)
{
	StaticMeshComponent mesh(m_VaoID);
	int32 lastVertexCount = 0;
	ResourcesManager& manager = ResourcesManager::Instance();

	if (m_HaveIndexBuffer) { // Not = to 0 means that its indexed.
		for (const ModelMaterialData& mat : m_Materials) {
			PrimitiveGeometry model_geo(DataType::UINT, mat.vcount, lastVertexCount * sizeof(uint32));
			MaterialID matID = manager.CreateResource<Material>(ResourcesTypes::MATERIAL, mat.material, shader_id);
			mesh.submeshs.EmplaceBack(model_geo, matID);
			lastVertexCount += mat.vcount;
		}
	} else {
		for (const ModelMaterialData& mat : m_Materials) {
			PrimitiveGeometry model_geo(lastVertexCount, mat.vcount);
			MaterialID matID = manager.CreateResource<Material>(ResourcesTypes::MATERIAL, mat.material, shader_id);
			mesh.submeshs.EmplaceBack(model_geo, matID);
			lastVertexCount += mat.vcount;
		}
	}

	return mesh;
}

MeshInstance Model::LoadInstancedMesh(uint32 instance_count, ShaderID shader_id)
{
	ResourcesManager& manager = ResourcesManager::Instance();
	VboID vboID = manager.CreateResource<VBO>(ResourcesTypes::VBO);

	uint32 vbo_index = m_CreateVaoCmd->settings.vertices_data.Size();
	uint32 layout_id = m_CreateVaoCmd->settings.attributes.Size();
	Mat4f* transforms = new Mat4f[instance_count];
	m_CreateVaoCmd->settings.vertices_data.EmplaceBack(transforms, instance_count, &manager.Get<VBO>(ResourcesTypes::VBO, vboID));
	// Size: 4 floats (for the layout) | Stide : 16 -> 16 floats (Mat4f) | Offset: 4 means (4 floats) | Divisor: 1 per instance
	m_CreateVaoCmd->settings.attributes.EmplaceBack(vbo_index, layout_id    , 4, 16,     0, DataType::FLOAT, 1);
	m_CreateVaoCmd->settings.attributes.EmplaceBack(vbo_index, layout_id + 1, 4, 16,     4, DataType::FLOAT, 1);
	m_CreateVaoCmd->settings.attributes.EmplaceBack(vbo_index, layout_id + 2, 4, 16, 2 * 4, DataType::FLOAT, 1);
	m_CreateVaoCmd->settings.attributes.EmplaceBack(vbo_index, layout_id + 3, 4, 16, 3 * 4, DataType::FLOAT, 1);

	MeshInstance mesh(instance_count, m_VaoID, vboID, transforms);
	int32 lastVertexCount = 0;
	
	if (m_HaveIndexBuffer) { // Not = to 0 means that its indexed.
		for (const ModelMaterialData& mat : m_Materials) {
			PrimitiveGeometry model_geo(DataType::UINT, mat.vcount, lastVertexCount * sizeof(uint32));
			MaterialID matID = manager.CreateResource<Material>(ResourcesTypes::MATERIAL, mat.material, shader_id);
			mesh.AddSubMesh(model_geo, matID);
			lastVertexCount += mat.vcount;
		}
	} else {
		for (const ModelMaterialData& mat : m_Materials) {
			PrimitiveGeometry model_geo(lastVertexCount, mat.vcount);
			MaterialID matID = manager.CreateResource<Material>(ResourcesTypes::MATERIAL, mat.material, shader_id);
			mesh.AddSubMesh(model_geo, matID);
			lastVertexCount += mat.vcount;
		}
	}

	return mesh;
}

Vector<NormalVertexData> Model::ProcessModel(uint32 processing, const Vector<VertexData>& vertices, Vector<uint32>* indices)
{
	if (!processing)
		return {};

	usize vtxCount = vertices.Size();
	Vector<NormalVertexData> temp_data(vtxCount);
	Vector<NormalVertexData> result(vtxCount);
	result.Resize(vtxCount);
	temp_data.Resize(vtxCount);

	// (1)
	usize indexCount = indices->Size();
	for (usize i = 0; i < indexCount; i+=3) {
		uint32 i0 = (*indices)[i];
		uint32 i1 = (*indices)[i+1];
		uint32 i2 = (*indices)[i+2];

		const Vec3f& pos0 = vertices[i0].pos;
		const Vec3f& pos1 = vertices[i1].pos;
		const Vec3f& pos2 = vertices[i2].pos;

		const Vec2f& tex0 = vertices[i0].texture;
		const Vec2f& tex1 = vertices[i1].texture;
		const Vec2f& tex2 = vertices[i2].texture;

		Vec3f edge1 = pos1 - pos0;
		Vec3f edge2 = pos2 - pos0;

		Vec2f uv1 = tex1 - tex0;
		Vec2f uv2 = tex2 - tex0;

		float r = 1.0f / (uv1.x * uv2.y - uv1.y * uv2.x);

		Vec3f tangent(
			((edge1.x * uv2.y) - (edge2.x * uv1.y)) * r,
			((edge1.y * uv2.y) - (edge2.y * uv1.y)) * r,
			((edge1.z * uv2.y) - (edge2.z * uv1.y)) * r
		);

		Vec3f bitangent(
			((edge1.x * uv2.x) - (edge2.x * uv1.x)) * r,
			((edge1.y * uv2.x) - (edge2.y * uv1.x)) * r,
			((edge1.z * uv2.x) - (edge2.z * uv1.x)) * r
		);


		temp_data[i0].tanget = tangent;
		temp_data[i0].bitanget = bitangent;

		temp_data[i1].tanget = tangent;
		temp_data[i1].bitanget = bitangent;

		temp_data[i2].tanget = tangent;
		temp_data[i2].bitanget = bitangent;
	}

	// (2)
	for (size_t i = 0; i < vtxCount; i++) {
		const Vec3f& n = vertices[i].normal;
		Vec3f& t0 = temp_data[i].tanget;
		Vec3f& t1 = temp_data[i].bitanget;

		Vec3f t = t0 - (n * n.dot_product(t0));
		t = t.unit();
		t1 = t1.unit();

		// Vec3f c = n.cross_product(t0);
		// float w = (c.dot_product(t1) < 0) ? -1.0f : 1.0f;

		result[i].tanget = t;
		result[i].bitanget = t1;
	}

	return result;
}

TRE_NS_END