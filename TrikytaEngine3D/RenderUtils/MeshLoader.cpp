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

int64 pow(int64 base, int64 pow) {
	if (pow == 0) { return 1; }
	if (pow < 0) { return 0; }
	uint64 result = 1;
	for (int i = 0; i < pow; i++) {
		result *= base;
	}
	return result;
}

bool ParseUint64(FILE* f, uint64* d)
{
	int32 c = fgetc(f);
	if (!(c >= '0' && c <= '9')) return false;
	*d = 0;
	Vector<char> arr;
	while ((char)c >= '0' && (char)c <= '9') {
		arr.emplace_back(c - '0');
		c = fgetc(f);
	}
	uint8 i = 0;
	for (char c : arr) {
		*d += c * pow(10, arr.size()-i-1);
		i++;
	}
	return true;
}

void MeshLoader::LoadFile(const char* path)
{
	FILE* file = fopen(path, "r");
	ASSERTF(file != NULL, "MeshLoader couldn't open the file %s.", path);

	fseek(file, 0L, SEEK_END);
	uint64 size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	uint64 lines = 0;
	while (!feof(file)) {
		int firstChar = fgetc(file);
		if (firstChar == EOF) {break;}
		if (firstChar == '#') { // skil comment
			int skipComment = fgetc(file);
			while (skipComment != EOF) {
				if (skipComment == '\n') break;
				skipComment = fgetc(file);
			}
			printf("#Comment bla bla\n");
		}if (firstChar == 'v') {
			int secondChar = fgetc(file);
			if (secondChar == ' ') {
				float x, y, z, w;
				int32 nargs = fscanf(file, "%f %f %f %f", &x, &y, &z, &w);
				if (nargs == 3) {
					m_Verticies.emplace_back(x, y, z, 0.f);
					printf("v %f %f %f %f\n", x, y, z, 0.f);
				}else if (nargs == 4) {
					m_Verticies.emplace_back(x, y, z, w);
					printf("v %f %f %f %f\n", x, y, z, w);
				}
			}else if (secondChar == 'n') {
				vec3 tempVec;
				int32 nargs = fscanf(file, " %f %f %f", &tempVec.x, &tempVec.y, &tempVec.z);
				if (nargs == 3) {
					m_Normals.push_back(tempVec);
					printf("vn %f %f %f\n", tempVec.x, tempVec.y, tempVec.z);
				}
			}else if (secondChar == 't') {
				float x, y, z;
				int32 nargs = fscanf(file, " %f %f %f", &x, &y, &z);
				if (nargs == 3) {
					m_TextureCoord.emplace_back(x, y, z);
					printf("vt %f %f %f\n", x, y, z);
				}if (nargs == 2) {
					m_TextureCoord.emplace_back(x, y, 0.f);
					printf("vt %f %f\n", x, y);
				}
			}
		}else if (firstChar == 'f') {
			int secondChar = fgetc(file);
			if (secondChar == ' ') {
				uint64 data;
				int c = fgetc(file);
				while (c != EOF) {
					if (c == '\n' || c == '#') { 
						printf("\n");
						break; 
					}
					//printf("%c", c);
					int x = fscanf(file, "%llu", &data);
					printf(" %d | %llu ", x, data);
					c = fgetc(file);
				}
				/*printf("f ");
				uint8 indicator = 0; // this what gonna tell us what par belong to what (0 means ref to vertex, 1 : ref to texture, 2 ref to normal)
				uint64 data;
				if (ParseUint64(file, &data)) {
					printf("%llu", data);
					int c = fgetc(file);
					fpos_t current_cursor_pos;
					fgetpos(file, &current_cursor_pos); // save this because later we might come back here
					while (c != '\n' && c != '#') {
						if (c == '/') {
							indicator++;
							printf("/");
						}else if (c == ' ') {
							indicator = 0;
							printf(" ");
						}else if (c >= '0' && c <= '9') {
							fsetpos(file, &--current_cursor_pos);
							ParseUint64(file, &data);
							printf("%llu", data);
						}
						c = fgetc(file);
						fgetpos(file, &current_cursor_pos);
					}
					if (c == '#') {
						fsetpos(file, &--current_cursor_pos);
					}
				}
				printf("\n");*/
			}
		}
	}
	fclose(file);
}

TRE_NS_END