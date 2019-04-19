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
	while ((line_len = ReadLine(file, buffer, 255)) != EOF) {
		if (line_len == 0) { continue; } // empty line
		if(buffer[0] == 'o'){
			m_Objects[m_ObjectCount] = String(buffer);
			m_ObjectCount++;
		}else if (buffer[0] == '#') {
			printf("# Comment :)");
			continue; // You dont wanna parse this :)
		}else if (buffer[0] == 'v') {
			if (buffer[1] == ' ') {
				float x, y, z, w;
				int32 nargs = sscanf(buffer+2, "%f %f %f %f", &x, &y, &z, &w);
				if (nargs == 3) {
					m_Verticies[m_ObjectCount-1].emplace_back(x, y, z);
					printf("v %f %f %f", x, y, z);
				}/*else if (nargs == 4) {
					m_Verticies[m_ObjectCount].emplace_back(x, y, z, w);
					printf("v %f %f %f %f", x, y, z, w);
				}*/
			}else if (buffer[1] == 'n') {
				vec3 tempVec;
				int32 nargs = sscanf(buffer + 2, " %f %f %f", &tempVec.x, &tempVec.y, &tempVec.z);
				if (nargs == 3) {
					m_Normals[m_ObjectCount-1].push_back(tempVec);
					printf("vn %f %f %f", tempVec.x, tempVec.y, tempVec.z);
				}
			}
			else if (buffer[1] == 't') {
				float x, y, z;
				int32 nargs = sscanf(buffer + 2, " %f %f %f", &x, &y, &z);
				if (nargs == 3) {
					m_TextureCoord[m_ObjectCount-1].emplace_back(x, y, z);
					printf("vt %f %f %f", x, y, z);
				}if (nargs == 2) {
					m_TextureCoord[m_ObjectCount-1].emplace_back(x, y, 0.f);
					printf("vt %f %f", x, y);
				}
			}
		}else if (buffer[0] == 'f') {
			if (buffer[1] == ' ') {
				uint32 data;
				uint8 indicator = 0;
				printf("f ");
				int64 buffer_index = 2;
				char c = buffer[buffer_index];
				//Vec<3, int64, normal> face(-1LL, -1LL, -1LL); // init to -1
				while (c != '\0') {
					if (c == '\n' || c == '#') {
						printf("\n");
						break;
					}else if (c >= '0' && c <= '9') {
						int32 len = ParseUint64(buffer + buffer_index, &data);
						if (len >= 1) {
							printf("%d", data);
							switch (indicator) {
							case 0: // Vertex Position.
								//face.x = data;
								m_Indicies[m_ObjectCount-1].emplace_back(data);
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
						/*if (face != Vec<3, int64, normal>(-1LL, -1LL, -1LL)) {
							printf(" [ Puttin it into the thing ] ");
							m_Faces[m_ObjectCount].emplace_back(face);
						}*/
						//face = Vec<3, int64, normal>(-1LL, -1LL, -1LL);
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
		printf("\n");
	}
	fclose(file);
}

void MeshLoader::ProcessData(VAO* vao, VBO* vbo, VBO* ivbo)
{
	//ASSERTF((vao ~= NULL), "Null pointer of VAO is passed!");
	for (uint8 i = 0; i < m_ObjectCount; i++) {
		vao->Use();
		Vector<vec3> vertices;
		printf("Index = %d", i);
		printf("float verticies[] = {\n");
		for (int32 index : m_Indicies[i]) {
			vertices.push_back(m_Verticies[i].at(index -1));
			printf("%f, %f, %f, // ID = %d\n", m_Verticies[i].at(index - 1).x, m_Verticies[i].at(index - 1).y, m_Verticies[i].at(index - 1).z, index);
		}
		printf("};\n");
		vbo->FillData(&vertices.at(0), vertices.size()*sizeof(vec3));
		vao->BindAttribute<DataType::FLOAT>(0, *vbo, 3, 3, 0);
		printf("vert size = %d\n", vertices.size());
		//vao->Unuse();
		//ivbo->FillData(&m_Indicies[0], m_Indicies[0].size()*sizeof(uint32));
	}
}

TRE_NS_END