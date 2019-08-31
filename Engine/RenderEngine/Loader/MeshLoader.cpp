#include "MeshLoader.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>

#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <Core/Misc/Utils/Logging.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/FileSystem/File/File.hpp>

#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>

TRE_NS_START

MeshLoader::MeshLoader(const char* path)
{
	m_ObjectCount = 0;
	LoadFile(path);
}

void MeshLoader::LoadFile(const char* path)
{
	FILE* file = fopen(path, "r");
	ASSERTF(file == NULL, "MeshLoader couldn't open the file %s.", path);
	printf("Parsing the Mesh File : %s\n", path);
	char buffer[255];
	char m_MaterialName[25] = "\0";
	int64 line_len = 0;
	usize vert_offset = 1;
	usize tex_offset = 1;
	usize norm_offset = 1;
	usize lastVertexCount = 0;
	usize current_line = 1;
	while ((line_len = ReadLine(file, buffer, 255)) != EOF) {
		if (line_len == 0) { continue; } // empty line
		if(buffer[0] == 'o'){
			if (m_ObjectCount > 0) {
				vert_offset += m_Verticies[m_ObjectCount].Length();
				tex_offset += m_TextureCoord[m_ObjectCount].Length();
				norm_offset += m_Normals[m_ObjectCount].Length();
			}
			if (m_MaterialName[0] != '\0') { //We have seen usemtl so lets dump it inside the array.
				m_Materials[m_ObjectCount].EmplaceBack(m_MaterialLoader.GetMaterialFromName(m_MaterialName), int32(m_DataIndex[m_ObjectCount].Size() - lastVertexCount));
				// Clear it
				m_MaterialName[0] = '\0';
				lastVertexCount = 0;
			}
			m_IndiciesOfVData[m_ObjectCount] = 0;
			m_Objects.Emplace(String(buffer), m_ObjectCount); // m_Objects[String(buffer)] = m_ObjectCount;
			m_ObjectCount++;
		}else if (buffer[0] == '#') {// You dont wanna parse this :)
			continue; 
		}else if (buffer[0] == 'v') {
			if (buffer[1] == ' ') {
				float x, y, z, w;
				int32 nargs = sscanf(buffer+2, "%f %f %f %f", &x, &y, &z, &w);
				if (nargs == 3) {
					m_Verticies[m_ObjectCount].EmplaceBack(x, y, z);
					//printf("v %f %f %f [ObjectCount = %d]", x, y, z, m_ObjectCount);
				}/*else if (nargs == 4) {
					m_Verticies[m_ObjectCount].emplace_back(x, y, z, w);
					printf("v %f %f %f %f", x, y, z, w);
				}*/
			}else if (buffer[1] == 'n') {
				vec3 tempVec;
				int32 nargs = sscanf(buffer + 2, " %f %f %f", &tempVec.x, &tempVec.y, &tempVec.z);
				if (nargs == 3) {
					m_Normals[m_ObjectCount].PushBack(tempVec);
					//printf("vn %f %f %f [ObjectCount = %d]", tempVec.x, tempVec.y, tempVec.z, m_ObjectCount);
				}
			}
			else if (buffer[1] == 't') {
				float x, y, z;
				/*int32 nargs = */sscanf(buffer + 2, " %f %f %f", &x, &y, &z);
				/*if (nargs == 3) {
					//m_TextureCoord[m_ObjectCount-1].emplace_back(x, y, z);
					//printf("vt %f %f %f [ObjectCount = %d]", x, y, z, m_ObjectCount);
				}*///if (nargs == 2) {
					m_TextureCoord[m_ObjectCount].EmplaceBack(x, 1.f-y);
					//printf("vt %f %f", x, y);
				//}
			}
		}else if (buffer[0] == 'f') {
			if (buffer[1] == ' ') {
				uint32 data;
				uint8 indicator = 0;
				ssize vi = -1, ni = -1, ti = -1;
				vec3 pos, norm = vec3(0.f, 0.f, 0.f);
				vec2 tex = vec2(0.f, 0.f);
				//printf("f ");
				int64 buffer_index = 2;
				char c = buffer[buffer_index];
				while (c != '\0') {
					if (c == '\n' || c == '#') {
						//printf("\n");
						break;
					}else if (c >= '0' && c <= '9') {
						int32 len = ParseUint64(buffer + buffer_index, &data);
						if (len >= 1) {
							switch (indicator) {
							case 0: // Vertex Position.
								//face.x = data;
								//m_Indicies[m_ObjectCount].emplace_back(data - vert_offset);
								vi = data - vert_offset;
								pos = m_Verticies[m_ObjectCount].At(vi);
								break;
							case 1: // Vertex texture.
								//face.y = data;
								ti = data - tex_offset;
								tex = m_TextureCoord[m_ObjectCount].At(ti);
								break;
							case 2: // Vertex normal.
								//face.z = data;
								/*if (std::find(m_NormalIndicies[m_ObjectCount].begin(), m_NormalIndicies[m_ObjectCount].end(), data - norm_offset) == m_NormalIndicies[m_ObjectCount].end()) {
									m_NormalIndicies[m_ObjectCount].emplace_back(data - norm_offset);
									m_OrderedNormals[m_ObjectCount].emplace_back(m_Normals[m_ObjectCount].at(data - norm_offset));
									//printf("I : %d | N (%f, %f, %f)\n", data - norm_offset, m_Normals[m_ObjectCount].at(data - norm_offset).x, m_Normals[m_ObjectCount].at(data - norm_offset).y, m_Normals[m_ObjectCount].at(data - norm_offset).z);
								}*/
								ni = data - norm_offset;
								norm = m_Normals[m_ObjectCount].At(ni);
								break;
							}
							buffer_index += len;
						}else {
							buffer_index++;
						}
					}else if (c == ' ') {
						indicator = 0;
						if (vi != -1) {
							if (!m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)]) { // if v/vn/vt	isnt already exist and indexed
								m_VerteciesData[m_ObjectCount].EmplaceBack(pos, norm, tex);
								m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)] = m_IndiciesOfVData[m_ObjectCount];
								m_IndiciesOfVData[m_ObjectCount]++;
							}else {
								//printf("Index (%ld, %ld, %ld) already exist and its i = %d\n", (long)vi, (long)ni, (long)ti, m_VertexDataIndex[m_ObjectCount][std::tuple<int64, int64, int64>(vi, ni, ti)]);
							}
							m_DataIndex[m_ObjectCount].EmplaceBack(m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)]);
						}
						buffer_index++;
						vi = -1; ni = -1; ti = -1;
						//printf(" ");
					}else if (c == '/') {
						indicator++;
						buffer_index++;
						//printf("/");
					}
					c = buffer[buffer_index];
				}
				if (vi != -1) {
					if (!m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)]) { // if v/vn/vt	isnt already exist and indexed
						m_VerteciesData[m_ObjectCount].EmplaceBack(pos, norm, tex);
						m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)] = m_IndiciesOfVData[m_ObjectCount];
						m_IndiciesOfVData[m_ObjectCount]++;
					}else {
						//printf("Index (%ld, %ld, %ld) already exist and its i = %d\n", (long)vi, (long)ni, (long)ti, m_VertexDataIndex[m_ObjectCount][std::tuple<int64, int64, int64>(vi, ni, ti)]);
					}
					m_DataIndex[m_ObjectCount].EmplaceBack(m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)]);
				}
			}
		}else if (IsEqual(buffer, "mtllib")) { // Material
			char mtrl_name[64];
			char mtrl_path[512];
			char dir[255];
			ssize buffer_i = 0;
			ssize i = strlen("mtllib")+1;
			while (buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != ' ') {
				mtrl_name[buffer_i] = buffer[i];
				i++; buffer_i++;
			}
			mtrl_name[buffer_i] = '\0';
			DirectoryToBasePath(path, dir, 255);
			strcpy(mtrl_path, dir);
			strcat(mtrl_path, mtrl_name);
			m_MaterialLoader.LoadFileMTL(mtrl_path, dir);
			printf("* Finished loading the material file... Resuming the obj..\n");
		}else if (IsEqual(buffer, "usemtl")) {
			if (m_MaterialName[0] != '\0') {
				m_Materials[m_ObjectCount].EmplaceBack(m_MaterialLoader.GetMaterialFromName(m_MaterialName), int32(m_DataIndex[m_ObjectCount].Size() - lastVertexCount));
				lastVertexCount = m_DataIndex[m_ObjectCount].Size();
			}
			int32 res = sscanf(buffer, "usemtl %s", m_MaterialName);
			ASSERTF(res != 1, "Attempt to parse a corrupted OBJ file 'usemtl' is being used without material name (Line : %" SZu ").", current_line);
		}
		//printf("\n");
		current_line++;
	}
	if (m_MaterialName[0] != '\0') {
		m_Materials[m_ObjectCount].EmplaceBack(m_MaterialLoader.GetMaterialFromName(m_MaterialName), int32(m_DataIndex[m_ObjectCount].Size() - lastVertexCount));
		lastVertexCount = m_DataIndex[m_ObjectCount].Size();
	}
	m_ObjectCount++;
	fclose(file);

	printf("Loading OBJ is done successfully.\n");
}

void MeshLoader::ProcessData(Vector<ModelLoader>& arrayOfObjects)
{
	arrayOfObjects.Reserve(m_ObjectCount);
	int32 objectIndex = (m_ObjectCount > 1 ? 1 : 0);
	// File file(String("carrot_box.text"), File::CREATE_READ_WRITE);
	do{
		/*file.Write("Size of vertecies : %lu | Size of indecies = %lu\n", m_VerteciesData[objectIndex].Size(), m_DataIndex[objectIndex].Size());
		file.Write("index = {");
		for (uint32 i : m_DataIndex[objectIndex]) {
			file.Write("%d ", i);
		}
		file.Write("}\n");
		file.Write("----------------------------\n");
		file.Write("data = {\n");
		for (const auto& data : m_VerteciesData[objectIndex]) {
			file.Write("[V(%f, %f, %f) / N(%f, %f, %f) / T(%f, %f)]\n",
				data.pos.x, data.pos.y, data.pos.z,
				data.normal.x, data.normal.y, data.normal.z,
				data.texture.x, data.texture.y
			);
		}
		file.Write("}\n");*/
		/*file.Write("----------------------------\n");
		for (uint32 i : m_DataIndex[objectIndex]) {
			const auto& data = m_VerteciesData[objectIndex].At(i);
			file.Write("{V(%f, %f, %f) / N(%f, %f, %f) / T(%f, %f)}\n",
				data.pos.x, data.pos.y, data.pos.z,
				data.normal.x, data.normal.y, data.normal.z,
				data.texture.x, data.texture.y
			);
		}*/
		arrayOfObjects.EmplaceBack(m_VerteciesData[objectIndex], m_DataIndex[objectIndex], m_Materials[objectIndex]);
		//arrayOfObjects->emplace_back(m_Verticies[objectIndex], m_Indicies[objectIndex], &m_TextureCoord[objectIndex], &m_OrderedNormals[objectIndex], m_Materials[objectIndex]);
		objectIndex++;
	}while (objectIndex < m_ObjectCount);
}

ModelLoader MeshLoader::LoadAsOneObject()
{
	int32 objectIndex = (m_ObjectCount > 1 ? 1 : 0);
	Vector<VertexData> vert_data_global = std::move(m_VerteciesData[objectIndex]);
	Vector<uint32> indices_data_global = std::move(m_DataIndex[objectIndex]);
	Vector<MatrialForRawModel> material_data_global = std::move(m_Materials[objectIndex]);
	usize index_offset = vert_data_global.Length();
	objectIndex++;

	do{
		vert_data_global.Append(std::move(m_VerteciesData[objectIndex]));
		material_data_global.Append(std::move(m_Materials[objectIndex]));

		for(uint32 index : m_DataIndex[objectIndex]){
			indices_data_global.EmplaceBack(index + index_offset);
		}

		index_offset = vert_data_global.Length();
		objectIndex++;
	}while (objectIndex < m_ObjectCount);

	ModelLoader object(vert_data_global, indices_data_global, material_data_global);
	return object;
}

ModelLoader MeshLoader::LoadObject(const String& name)
{
	uint8* obj_index = m_Objects.Get(name);
	ASSERTF(obj_index == NULL, "Attempt to load an object which isn't present.");
	uint8 id = *obj_index + 1;
	ModelLoader object(m_VerteciesData[id], m_DataIndex[id], m_Materials[id]);
	return object;
}

TRE_NS_END