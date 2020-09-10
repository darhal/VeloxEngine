#pragma once

#include <Engine/Core/Misc/Defines/Common.hpp>
#include <Engine/Core/Misc/Defines/Platform.hpp>
#include <vector>
#include <string>
#include <map>
#include <cstring> 
#include <string>

#if defined(COMPILER_MSVC)
#pragma warning(disable:4996)
#endif

template<typename T, typename V>
union UConverter
{
	T first_rep;
	V second_rep;
};

template <usize N>
static FORCEINLINE usize GetArraySize(float(&p_Array)[N]) {
	usize num = (sizeof(p_Array) / sizeof(p_Array[0]));
	return num;
}


static int Basepath(const char* p_pathname, size_t pathname_size, char* p_basepath, size_t basepath_size, char sep)
{
	char*  p_end_of_path;
	size_t path_length;

	// Parameter Validation
	if (p_pathname == NULL || p_basepath == NULL) { return 0; }
	if (pathname_size < 1 || basepath_size < 1) { return 0; }

	// Returns a pointer to the last occurrence of \ in p_pathname or NULL if it is not found
	p_end_of_path = (char*)strrchr(p_pathname, sep);
	if (p_end_of_path == NULL)
	{
		return 0; // There is no path part
	}else {
		path_length = (size_t)(p_end_of_path - p_pathname + 1);

		// Do some sanity checks on the length
		if (path_length < 1) { return 0; }
		if ((path_length + 1) > basepath_size) { return 0; }

		// Copy the base path into the out variable
		strncpy(p_basepath, p_pathname, path_length);
		p_basepath[path_length] = '\0';
	}
	return 1;
}

static void DirectoryToBasePath(const char* path, char* dst, ssize size)
{
	char seprator = '/';
#if defined(OS_WINDOWS)
	seprator = '/';
#else
	seprator = '/';
#endif
	Basepath(path, strlen(path), dst, size, seprator);
}

static int64 ReadLine(FILE* file, char* line, uint32 limit)
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

static bool IsEqual(const char* src, const char* src2)
{
	return (strncmp(src, src2, strlen(src2) * sizeof(char)) == 0);
}