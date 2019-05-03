#include "MeshLoader.hpp"
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/Misc/Utils/Logging.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>

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
	ASSERTF(file != NULL, "MeshLoader couldn't open the file %s.", path);
	printf("Parsing the Mesh File : %s\n", path);
	char buffer[255];
	char m_MaterialName[25] = "\0";
	int64 line_len = 0;
	ssize vert_offset = 1;
	ssize tex_offset = 1;
	ssize norm_offset = 1;
	uint64 current_line = 1;
	ssize lastVertexCount = 0;
	while ((line_len = ReadLine(file, buffer, 255)) != EOF) {
		if (line_len == 0) { continue; } // empty line
		if(buffer[0] == 'o'){
			if (m_ObjectCount > 0) {
				vert_offset += m_Verticies[m_ObjectCount].size();
				tex_offset += m_TextureCoord[m_ObjectCount].size();
				norm_offset += m_Normals[m_ObjectCount].size();
			}
			if (m_MaterialName[0] != '\0') { //We have seen usemtl so lets dump it inside the array.
				m_Materials[m_ObjectCount].emplace_back(m_MaterialLoader.GetMaterialFromName(m_MaterialName), (int32)(m_DataIndex[m_ObjectCount].size() - lastVertexCount));
				// Clear it
				m_MaterialName[0] = '\0';
				lastVertexCount = 0;
			}
			m_IndiciesOfVData[m_ObjectCount] = 0;
			m_Objects[m_ObjectCount] = String(buffer);
			m_ObjectCount++;
		}else if (buffer[0] == '#') {// You dont wanna parse this :)
			continue; 
		}else if (buffer[0] == 'v') {
			if (buffer[1] == ' ') {
				float x, y, z, w;
				int32 nargs = sscanf(buffer+2, "%f %f %f %f", &x, &y, &z, &w);
				if (nargs == 3) {
					m_Verticies[m_ObjectCount].emplace_back(x, y, z);
					//printf("v %f %f %f [ObjectCount = %d]", x, y, z, m_ObjectCount);
				}/*else if (nargs == 4) {
					m_Verticies[m_ObjectCount].emplace_back(x, y, z, w);
					printf("v %f %f %f %f", x, y, z, w);
				}*/
			}else if (buffer[1] == 'n') {
				vec3 tempVec;
				int32 nargs = sscanf(buffer + 2, " %f %f %f", &tempVec.x, &tempVec.y, &tempVec.z);
				if (nargs == 3) {
					m_Normals[m_ObjectCount].push_back(tempVec);
					//printf("vn %f %f %f [ObjectCount = %d]", tempVec.x, tempVec.y, tempVec.z, m_ObjectCount);
				}
			}
			else if (buffer[1] == 't') {
				float x, y, z;
				int32 nargs = sscanf(buffer + 2, " %f %f %f", &x, &y, &z);
				/*if (nargs == 3) {
					//m_TextureCoord[m_ObjectCount-1].emplace_back(x, y, z);
					//printf("vt %f %f %f [ObjectCount = %d]", x, y, z, m_ObjectCount);
				}*///if (nargs == 2) {
					m_TextureCoord[m_ObjectCount].emplace_back(x, 1.f-y);
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
								pos = m_Verticies[m_ObjectCount].at(vi);
								break;
							case 1: // Vertex texture.
								//face.y = data;
								ti = data - tex_offset;
								tex = m_TextureCoord[m_ObjectCount].at(ti);
								break;
							case 2: // Vertex normal.
								//face.z = data;
								/*if (std::find(m_NormalIndicies[m_ObjectCount].begin(), m_NormalIndicies[m_ObjectCount].end(), data - norm_offset) == m_NormalIndicies[m_ObjectCount].end()) {
									m_NormalIndicies[m_ObjectCount].emplace_back(data - norm_offset);
									m_OrderedNormals[m_ObjectCount].emplace_back(m_Normals[m_ObjectCount].at(data - norm_offset));
									//printf("I : %d | N (%f, %f, %f)\n", data - norm_offset, m_Normals[m_ObjectCount].at(data - norm_offset).x, m_Normals[m_ObjectCount].at(data - norm_offset).y, m_Normals[m_ObjectCount].at(data - norm_offset).z);
								}*/
								ni = data - norm_offset;
								norm = m_Normals[m_ObjectCount].at(ni);
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
								m_VerteciesData[m_ObjectCount].emplace_back(pos, norm, tex);
								m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)] = m_IndiciesOfVData[m_ObjectCount];
								m_IndiciesOfVData[m_ObjectCount]++;
							}else {
								//printf("Index (%ld, %ld, %ld) already exist and its i = %d\n", (long)vi, (long)ni, (long)ti, m_VertexDataIndex[m_ObjectCount][std::tuple<int64, int64, int64>(vi, ni, ti)]);
							}
							m_DataIndex[m_ObjectCount].emplace_back(m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)]);
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
						m_VerteciesData[m_ObjectCount].emplace_back(pos, norm, tex);
						m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)] = m_IndiciesOfVData[m_ObjectCount];
						m_IndiciesOfVData[m_ObjectCount]++;
					}else {
						//printf("Index (%ld, %ld, %ld) already exist and its i = %d\n", (long)vi, (long)ni, (long)ti, m_VertexDataIndex[m_ObjectCount][std::tuple<int64, int64, int64>(vi, ni, ti)]);
					}
					m_DataIndex[m_ObjectCount].emplace_back(m_VertexDataIndex[m_ObjectCount][std::tuple<ssize, ssize, ssize>(vi, ni, ti)]);
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
			Directory(path, dir, 255);
			strcpy(mtrl_path, dir);
			strcat(mtrl_path, mtrl_name);
			m_MaterialLoader.LoadFileMTL(mtrl_path, dir);
		}else if (IsEqual(buffer, "usemtl")) {
			if (m_MaterialName[0] != '\0') {
				m_Materials[m_ObjectCount].emplace_back(m_MaterialLoader.GetMaterialFromName(m_MaterialName), (int32)(m_DataIndex[m_ObjectCount].size() - lastVertexCount));
				lastVertexCount = m_DataIndex[m_ObjectCount].size();
			}
			int32 res = sscanf(buffer, "usemtl %s", m_MaterialName);
			ASSERTF(res == 1, "Attempt to parse a corrupted OBJ file 'usemtl' is being used without material name (Line : %llu).", current_line);
		}
		//printf("\n");
		current_line++;
	}
	if (m_MaterialName[0] != '\0') {
		m_Materials[m_ObjectCount].emplace_back(m_MaterialLoader.GetMaterialFromName(m_MaterialName), (int32)(m_DataIndex[m_ObjectCount].size() - lastVertexCount));
		lastVertexCount = m_DataIndex[m_ObjectCount].size();
	}
	m_ObjectCount++;
	fclose(file);
}

void MeshLoader::ProcessData(Vector<RawModel<true>>* arrayOfObjects)
{
	ASSERTF(arrayOfObjects != NULL, "Argument passed to MeshLoader::ProcessData is NULL");
	arrayOfObjects->reserve(m_ObjectCount);
	int32 objectIndex = (m_ObjectCount > 1 ? 1 : 0);
	do{
		arrayOfObjects->emplace_back(m_VerteciesData[objectIndex], m_DataIndex[objectIndex], m_Materials[objectIndex]);
		/*printf("index = {");
		for (uint32 i : m_DataIndex[objectIndex]) {
			printf("%d ", i);
		}
		printf("}\n");
		printf("----------------------------\n");
		printf("data = {\n");
		for (const auto& data : m_VerteciesData[objectIndex]) {
			printf("[V(%f, %f, %f) / N(%f, %f, %f) / T(%f, %f)]\n",
				data.pos.x, data.pos.y, data.pos.z,
				data.normal.x, data.normal.y, data.normal.z,
				data.texture.x, data.texture.y
			);
		}
		printf("}\n");
		printf("----------------------------\n");
		for (uint32 i : m_DataIndex[objectIndex]) {
			const auto& data = m_VerteciesData[objectIndex].at(i);
			printf("{V(%f, %f, %f) / N(%f, %f, %f) / T(%f, %f)}\n",
				data.pos.x, data.pos.y, data.pos.z,
				data.normal.x, data.normal.y, data.normal.z,
				data.texture.x, data.texture.y
			);
		}*/
		//arrayOfObjects->emplace_back(m_Verticies[objectIndex], m_Indicies[objectIndex], &m_TextureCoord[objectIndex], &m_OrderedNormals[objectIndex], m_Materials[objectIndex]);
		objectIndex++;
	}while (objectIndex < m_ObjectCount);
}

/*printf("Indexing (%ld, %ld, %ld) DATA=[V(%f, %f, %f) / N(%f, %f, %f) / T(%f, %f)]\n",
	(long)vi, (long)ni, (long)ti,
	m_VerteciesData[m_ObjectCount].back().pos.x, m_VerteciesData[m_ObjectCount].back().pos.y, m_VerteciesData[m_ObjectCount].back().pos.z,
	m_VerteciesData[m_ObjectCount].back().normal.x, m_VerteciesData[m_ObjectCount].back().normal.y, m_VerteciesData[m_ObjectCount].back().normal.z,
	m_VerteciesData[m_ObjectCount].back().texture.x, m_VerteciesData[m_ObjectCount].back().texture.y
);*/

TRE_NS_END