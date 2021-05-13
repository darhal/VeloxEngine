
#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <stdio.h>

TRE_NS_START

namespace FileSystem
{
	ssize_t GetLine(char** lineptr, size_t* n, FILE* stream, char delim = '\n');
}

TRE_NS_END