#include "MeshLoader.hpp"
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <fstream>
#include <Core/Misc/Utils/Logging.hpp>
#include <fstream>
#include <iostream>

TRE_NS_START

MeshLoader::MeshLoader(const char* path)
{
	LoadFile(path);
}

uint64 pow(int64 base, uint8 pow) {
	if (pow == 0) { return 1; }
	if (base == 0) { return 0; }
	if (pow < 0) { return 0; }
	uint64 result = 1;
	for (int i = 0; i < pow; i++) {
		result *= base;
	}
	return result;
}

int32 ParseUint64(char* str, uint64* x)
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
			*x += data[i] * pow(10, len - i - 1);
		}
		return len;
	}
	return -1;
}

int64 ReadLine(FILE* file, char* line, int limit)
{
	int32 c = 0;
	int64 line_len = 0;
	c = fgetc(file);
	if (c == EOF) { return EOF; }
	while((c != EOF && line_len < limit && c != '\n')) {
		line[line_len] = c;
		line_len++;
		c = fgetc(file);
	}
	line[line_len] = '\0';
	return line_len;
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
		if (buffer[0] == '#') {
			printf("# Comment :)");
			continue; // You dont wanna parse this :)
		}else if (buffer[0] == 'v') {
			if (buffer[1] == ' ') {
				float x, y, z, w;
				int32 nargs = sscanf(buffer+2, "%f %f %f %f", &x, &y, &z, &w);
				if (nargs == 3) {
					m_Verticies.emplace_back(x, y, z, 0.f);
					printf("v %f %f %f", x, y, z);
				}else if (nargs == 4) {
					m_Verticies.emplace_back(x, y, z, w);
					printf("v %f %f %f %f", x, y, z, w);
				}
			}else if (buffer[1] == 'n') {
				vec3 tempVec;
				int32 nargs = sscanf(buffer + 2, " %f %f %f", &tempVec.x, &tempVec.y, &tempVec.z);
				if (nargs == 3) {
					m_Normals.push_back(tempVec);
					printf("vn %f %f %f", tempVec.x, tempVec.y, tempVec.z);
				}
			}
			else if (buffer[1] == 't') {
				float x, y, z;
				int32 nargs = sscanf(buffer + 2, " %f %f %f", &x, &y, &z);
				if (nargs == 3) {
					m_TextureCoord.emplace_back(x, y, z);
					printf("vt %f %f %f", x, y, z);
				}if (nargs == 2) {
					m_TextureCoord.emplace_back(x, y, 0.f);
					printf("vt %f %f", x, y);
				}
			}
		}else if (buffer[0] == 'f') {
			if (buffer[1] == ' ') {
				uint64 data;
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
							printf("%llu", data);
							buffer_index += len;
						}else {
							buffer_index++;
						}
					}else if (c == ' ') {
						indicator++;
						buffer_index++;
						printf(" ");
					}else if (c == '/') {
						indicator = 0;
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

TRE_NS_END