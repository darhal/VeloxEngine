#include "Utility.hpp"
#include <cstring>

TRE_NS_START

namespace FileSystem
{
	ssize_t GetLine(char** lineptr, size_t* n, FILE* stream, char delim)
	{
		CONSTEXPR uint32 BUFFER_CHUNK_SIZE = 256;
		size_t pos;
		int c;

		if (lineptr == NULL || stream == NULL || n == NULL) {
			return -1;
		}

#if defined(OS_WINDOWS)
		c = _getc_nolock(stream);
#else
		c = getc(stream);
#endif
		if (c == EOF) {
			return -1;
		}

		if (*lineptr == NULL) {
			*n = BUFFER_CHUNK_SIZE;
			*lineptr = new char[*n];

			if (*lineptr == NULL) {
				return -1;
			}
		}

		pos = 0;
		while (c != EOF) {
			if (pos + 1 >= *n) {
				size_t new_size = *n + (*n >> 2);
				if (new_size < BUFFER_CHUNK_SIZE) {
					new_size = BUFFER_CHUNK_SIZE;
				}
				char* new_ptr = new char[new_size];
				memcpy(new_ptr, lineptr, *n * sizeof(char));
				if (new_ptr == NULL) {
					return -1;
				}
				*n = new_size;
				*lineptr = new_ptr;
			}

			((unsigned char*)(*lineptr))[pos++] = c;
			if (c == delim) {
				break;
			}
#if defined(OS_WINDOWS)
			c = _getc_nolock(stream);
#else
			c = getc(stream);
#endif
		}

		(*lineptr)[pos] = '\0';
		return pos;
	}
}

TRE_NS_END
