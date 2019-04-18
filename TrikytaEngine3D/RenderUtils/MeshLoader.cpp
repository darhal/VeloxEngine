#include "MeshLoader.hpp"
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <fstream>
#include <Core/Misc/Utils/Logging.hpp>

TRE_NS_START

MeshLoader::MeshLoader(const char* path)
{
	LoadFile(path);
}

void MeshLoader::LoadFile(const char* path)
{
	FILE* file = fopen(path, "r");
	ASSERTF(file != NULL, "MeshLoader couldn't open the file %s.", path);

	fseek(file, 0L, SEEK_END);
	uint64 size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	while (!feof(file)) {
		char buffer[3];
		fgets(buffer, 3, file);
		if (strncmp(buffer, "v ", sizeof(buffer)) == 0){
			float x, y, z, w;
			int32 nargs = fscanf(file, "%f %f %f %f", &x, &y, &z, &w);
			if (nargs == 3) {
				m_Verticies.emplace_back(x, y, z, w);
			}else if (nargs == 4){
				m_Verticies.emplace_back(x, y, z, w);
			}
		}else if (strncmp(buffer, "vn", sizeof(buffer)) == 0) {
			vec3 tempVec;
			int32 nargs = fscanf(file, "%f %f %f", &tempVec.x, &tempVec.y, &tempVec.z);
			if (nargs == 3) {
				m_Normals.push_back(tempVec);
			}
		}else if (strncmp(buffer, "vt", sizeof(buffer)) == 0) {
			float x, y, z;
			int32 nargs = fscanf(file, "%f %f %f", &x, &y, &z);
			if (nargs == 3) {
				m_TextureCoord.emplace_back(x, y, z);
			}if (nargs == 2) {
				m_TextureCoord.emplace_back(x, y, 0.f);
			}
		}else if (strncmp(buffer, "f ", sizeof(buffer)) == 0) {
			fpos_t current_cursor_pos; 
			fgetpos(file, &current_cursor_pos); // save this because later we might come back here!
			while (fgetc(file) != 'f') {

			}
		}
	}
	fclose(file);
}

TRE_NS_END