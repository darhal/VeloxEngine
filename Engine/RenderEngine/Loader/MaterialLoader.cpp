#include <fstream>
#include <iostream>
#include "MaterialLoader.hpp"
#include <Core/Misc/Defines/Debug.hpp>
#include <RenderEngine/Materials/Material.hpp>
#include <Core/DataStructure/String/String.hpp>

TRE_NS_START

MaterialLoader::MaterialLoader()
{
}

void MaterialLoader::LoadFileMTL(const char* mtrl_path, const char* obj_path)
{
	FILE* file = fopen(mtrl_path, "r");
	ASSERTF(file == NULL, "MaterialLoader couldn't open the file %s.", mtrl_path);
	char buffer[255];
	int64 line_len = 0;
	printf("Parsing the materials: %s\n", mtrl_path);
	uint64 current_line = 1;
	String current_name;
	AbstractMaterial* last_material =  NULL;
	while ((line_len = ReadLine(file, buffer, 255)) != EOF) {
		if (IsEqual(buffer, "newmtl")) {
			char m_Name[25];
			sscanf(buffer, "newmtl %s", m_Name);
			printf("Adding new material: %s\n", m_Name);
			current_name = m_Name;
			last_material = &m_NameToMaterial.Emplace(std::move(String(m_Name)), m_Name); //m_NameToMaterial[m_Name] = Material(m_Name);
		}else if (IsEqual(buffer, "Ka")) {
			vec3 ambient;
			int32 res = sscanf(buffer, "Ka %f %f %f", &ambient.x, &ambient.y, &ambient.z);
			ASSERTF(res != 3, "Attempt to parse a corrupted MTL file 'usemtl', failed while reading the ambient component (Line : %lu).", current_line);
			// m_NameToMaterial[current_name].GetTechnique().SetUniformVec3(); 
			// m_NameToMaterial[current_name].m_Ambient = ambient;
			//printf("Ka %f %f %f\n", ambient.x, ambient.y, ambient.z);
			last_material->GetParametres().AddParameter<vec3>("material.ambient", ambient);
		}else if (IsEqual(buffer, "Kd")) {
			vec3 diffuse;
			int32 res = sscanf(buffer, "Kd %f %f %f", &diffuse.x, &diffuse.y, &diffuse.z);
			ASSERTF(res != 3, "Attempt to parse a corrupted MTL file 'usemtl', failed while reading the diffuse component (Line : %lu).", current_line);
			//m_NameToMaterial[current_name].GetTechnique().SetUniformVec3(); 
			//m_NameToMaterial[current_name].m_Diffuse = diffuse;
			//printf("Kd %f %f %f\n", diffuse.x, diffuse.y, diffuse.z);
			last_material->GetParametres().AddParameter<vec3>("material.diffuse", diffuse);
		}else if (IsEqual(buffer, "Ks")) {
			vec3 specular;
			int32 res = sscanf(buffer, "Ks %f %f %f", &specular.x, &specular.y, &specular.z);
			ASSERTF(res != 3, "Attempt to parse a corrupted MTL file 'usemtl', failed while reading the specular component (Line : %lu).", current_line);
			//m_NameToMaterial[current_name].GetTechnique().SetUniformVec3(); 
			//m_NameToMaterial[current_name].m_Specular = specular;
			//printf("Ks %f %f %f\n", specular.x, specular.y, specular.z);
			last_material->GetParametres().AddParameter<vec3>("material.specular", specular);
		}else if (IsEqual(buffer, "Ns")) {
			float shininess;
			int32 res = sscanf(buffer, "Ns %f", &shininess);
			ASSERTF(res != 1, "Attempt to parse a corrupted MTL file 'usemtl', failed while reading the shininess component (Line : %lu).", current_line);
			//m_NameToMaterial[current_name].GetTechnique().SetUniformVec3(); 
			//m_NameToMaterial[current_name].m_Shininess = shininess;
			//printf("Ks %f %f %f\n", specular.x, specular.y, specular.z);
			last_material->GetParametres().AddParameter<float>("material.shininess", shininess);
		}else if (IsEqual(buffer, "map_Kd")) {
			char tex_file[255];
			char tex_path[512];
			int32 res = sscanf(buffer, "map_Kd %s", tex_file);
			ASSERTF(res != 1, "Attempt to parse a corrupted MTL file 'usemtl', failed while reading the diffuse map component (Line : %lu).", current_line);
			strcpy(tex_path, obj_path);
			strcat(tex_path, tex_file);
			//m_NameToMaterial[current_name].GetTechnique().SetUniformVec3(); 
			//m_NameToMaterial[current_name].AddTexture(Material::DIFFUSE, tex_path);
			last_material->GetParametres().AddParameter<TextureID>("material.diffuse_tex", last_material->AddTexture(tex_path));
		}else if (IsEqual(buffer, "map_Ks")) {
			char tex_file[255];
			char tex_path[512];
			int32 res = sscanf(buffer, "map_Ks %s", tex_file);
			ASSERTF(res != 1, "Attempt to parse a corrupted MTL file 'usemtl', failed while reading the specular map component (Line : %lu).", current_line);
			strcpy(tex_path, obj_path);
			strcat(tex_path, tex_file);
			//m_NameToMaterial[current_name].GetTechnique().SetUniformVec3(); 
			//m_NameToMaterial[current_name].AddTexture(Material::SPECULAR, tex_path);
			last_material->GetParametres().AddParameter<TextureID>("material.specular_tex", last_material->AddTexture(tex_path));
		}
		current_line++;
	}
	/*for (const auto& pair : m_NameToMaterial) {
		printf("Material name = %s | Contain key ? = %d\n", pair.first.Buffer(), m_NameToMaterial.ContainsKey(String(pair.first.Buffer(), 0)));
		auto material = &(pair.second);
		printf("	diffuse = (%f, %f, %f)\n", material->m_Diffuse.x, material->m_Diffuse.y, material->m_Diffuse.z);
		printf("	ambient = (%f, %f, %f)\n", material->m_Ambient.x, material->m_Ambient.y, material->m_Ambient.z);
		printf("	specular = (%f, %f, %f)\n", material->m_Specular.x, material->m_Specular.y, material->m_Specular.z);
	}*/
	fclose(file);
}

AbstractMaterial& MaterialLoader::GetMaterialFromName(const char* name)
{
	return m_NameToMaterial[name];
}

TRE_NS_END