#include "MeshLoader.hpp"
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/Misc/Utils/Logging.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <fstream>
#include <fstream>
#include <iostream>

#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>

TRE_NS_START

//TODO: Move this to its own clas
static int32 ParseUint64(char* str, uint32* x)
{
	*x = 0;
	if (str[0] >= '0' && str[0] <= '9') {
		uint8 data[21];
		int32 len = 0;
		while (str[len] >= '0' && str[len] <= '9' && len < 11) {
			data[len] = str[len] - '0';
			len++;
		}
		for (int32 i = 0; i < len; i++) {
			*x += data[i] * power(10u, len - i - 1);
		}
		return len;
	}
	return -1;
}

static int64 ReadLine(FILE* file, char* line, int limit)
{
	int32 c = 0;
	int64 line_len = 0;
	c = fgetc(file);
	if (c == EOF) { return EOF; }
	while ((c != EOF && line_len < limit && c != '\n')) {
		line[line_len] = c;
		line_len++;
		c = fgetc(file);
	}
	line[line_len] = '\0';
	return line_len;
}

MeshLoader::MeshLoader(const char* path)
{
	m_ObjectCount = 0;
	LoadFile(path);
}

void MeshLoader::LoadFile(const char* path)
{
	FILE* file = fopen(path, "r");
	ASSERTF(file != NULL, "MeshLoader couldn't open the file %s.", path);

	fseek(file, 0L, SEEK_END);
	uint64 size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	char buffer[255];
	int64 line_len = 0;
	ssize vert_offset = 1;
	ssize tex_offset = 1;
	ssize norm_offset = 1;
	while ((line_len = ReadLine(file, buffer, 255)) != EOF) {
		if (line_len == 0) { continue; } // empty line
		if(buffer[0] == 'o'){
			if (m_ObjectCount > 0) {
				vert_offset += m_Verticies[m_ObjectCount].size();
				tex_offset += m_TextureCoord[m_ObjectCount].size();
				norm_offset += m_Normals[m_ObjectCount].size();
			}
			m_Objects[m_ObjectCount++] = String(buffer);
		}else if (buffer[0] == '#') {
			//printf("# Comment :)");
			continue; // You dont wanna parse this :)
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
				if (nargs == 3) {
					//m_TextureCoord[m_ObjectCount-1].emplace_back(x, y, z);
					//printf("vt %f %f %f [ObjectCount = %d]", x, y, z, m_ObjectCount);
				}if (nargs == 2) {
					m_TextureCoord[m_ObjectCount].emplace_back(x, y);
					//printf("vt %f %f", x, y);
				}
			}
		}else if (buffer[0] == 'f') {
			if (buffer[1] == ' ') {
				uint32 data;
				uint8 indicator = 0;
				printf("f ");
				int64 buffer_index = 2;
				char c = buffer[buffer_index];
				while (c != '\0') {
					if (c == '\n' || c == '#') {
						printf("\n");
						break;
					}else if (c >= '0' && c <= '9') {
						int32 len = ParseUint64(buffer + buffer_index, &data);
						if (len >= 1) {
							switch (indicator) {
							case 0: // Vertex Position.
								//face.x = data;
								m_Indicies[m_ObjectCount].emplace_back(data - vert_offset);
								break;
							case 1: // Vertex texture.
								//face.y = data;
								break;
							case 2: // Vertex normal.
								//face.z = data;
								break;
							}
							buffer_index += len;
						}else {
							buffer_index++;
						}
					}else if (c == ' ') {
						indicator = 0;
						buffer_index++;
						printf(" ");
					}else if (c == '/') {
						indicator++;
						buffer_index++;
						printf("/");
					}
					c = buffer[buffer_index];
				}
			}
		}
		//printf("\n");
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
		arrayOfObjects->emplace_back(m_Verticies[objectIndex], &m_TextureCoord[objectIndex], &m_Normals[objectIndex], m_Indicies[objectIndex]);
		objectIndex++;
	}while (objectIndex < m_ObjectCount);
}

TRE_NS_END