#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/FileSystem/Utils/Utility.hpp>
#include <Core/DataStructure/String/String.hpp>
#include <stdio.h>
#include <stdarg.h>

TRE_NS_START

class File
{
public:
    enum Options
    {
        OPEN_READ = 0,          // 'r'
        OPEN_READ_WRITE = 1,    // 'r+'
        CREATE_WRITE = 2,       // 'w'
        CREATE_READ_WRITE = 3,  // 'w+'
        APPEND_WRITE = 4,       // 'a'
        APPEND_READ_WRITE = 5   // 'a+'
    };

    enum CursorOrigin
    {
        BEGIN = 0, CURRENT = 1, END = 2
    };

    File() : m_File(NULL) {};

    FORCEINLINE bool LoadFile(String& path, Options options);

    File(const String& path, Options foptions);

    virtual ~File();

    FORCEINLINE void Put(const String& content) const;

    FORCEINLINE void Put(const char* content) const;

    FORCEINLINE void Put(int32 content) const;

    void Write(const char* fmt, ...) const;

    // void Write(const String& fmt, ...) const;

    bool ReadString(String* result, usize sz) const;

    bool ReadString(char* result, usize sz) const;

    FORCEINLINE int32 ReadChar() const;

    int32 ReadFormated(const char* fmt, ...) const;

    // int32 ReadFormated(const String& fmt, ...) const;

    FORCEINLINE int32 ReadBytes(void* read_ptr, usize sz, usize count = 1) const;

    template<typename T>
    FORCEINLINE int32 ReadElement(T* read_ptr, usize count);

    FORCEINLINE bool GetLine(String& line_out) const;

    FORCEINLINE bool GetLine(String* line_out) const;

    usize Size() const;

    FORCEINLINE bool SetCursor(usize cur, CursorOrigin cur_position = BEGIN) const;

    FORCEINLINE bool Rename(const char* newName);

    FORCEINLINE bool Rename(const String& newName);

	FORCEINLINE String ReadAll();

    bool Delete();
private:

    FORCEINLINE int32 ReadFormatedHelper(const char* fmt, va_list ap) const;

    FORCEINLINE void WriteHelper(const char* fmt, va_list ap) const;

	CONSTEXPR static const char* const FILE_PROCESSING_OPTIONS[6] = { "r", "r+", "w", "w+", "a", "a+" };

	CONSTEXPR static int32 SEEK_POSITIONS[3] = { SEEK_SET, SEEK_CUR, SEEK_END };

    FILE* m_File;
    String m_Filename;
};

/**************************************************/
/*               Public Functions                 */
/**************************************************/

FORCEINLINE bool File::LoadFile(String& path, Options options)
{
    m_Filename = path;
    m_File = fopen(path.Buffer(), FILE_PROCESSING_OPTIONS[options]); 
    
    if (m_File != NULL)
        return true;

    return false;
}

template<typename T>
FORCEINLINE int32 File::ReadElement(T* read_ptr, usize count)
{
    return this->ReadBytes((void*)read_ptr, sizeof(T), count);
}

FORCEINLINE void File::Put(const String& content) const
{
    this->Put(content.Buffer());
}

FORCEINLINE void File::Put(const char* content) const
{
    ASSERTF(m_File == NULL, "Can't write to the file (FILE pointer is NULL)");
    int32 opr = fputs(content, m_File);
    ASSERTF(opr <= 0, "Failed to write buffer to the file");
}

FORCEINLINE void File::Put(int32 content) const
{
    ASSERTF(m_File == NULL, "Can't write to the file (FILE pointer is NULL)");
    int32 opr = fputc(content, m_File);
    ASSERTF(opr == EOF, "Failed to write buffer to the file");
}

FORCEINLINE int32 File::ReadChar() const
{
    int32 c = fgetc(m_File);
    return c;
}

FORCEINLINE int32 File::ReadBytes(void* read_ptr, usize sz, usize count) const
{
    return (int32) fread(read_ptr, sz, count, m_File);
}

FORCEINLINE bool File::GetLine(String& line_out) const
{
#if defined(OS_LINUX) || defined(OS_UNIX)
    char* line = NULL;
    usize len = 0;
    ssize res = getline(&line, &len, m_File);

    if (res == -1){
        free(line);
        return false;
    }

    line_out = String(line);
    free(line);
    return true;
#endif
	char* line = NULL;
	usize len = 0;
	ssize res = FileSystem::GetLine(&line, &len, m_File);

	if (res == -1) {
		delete[] line;
		return false;
	}

	line_out = String(line);
	delete[] line;
	return false;
}

FORCEINLINE bool File::GetLine(String* line_out) const
{
    return this->GetLine(*line_out);
}

FORCEINLINE bool File::Rename(const char* newName)
{
    int32 r = rename(m_Filename.Buffer(), newName);
    m_Filename = String(newName);
    return !r;
}

FORCEINLINE bool File::Rename(const String& newName)
{
    int32 r = rename(m_Filename.Buffer(), newName.Buffer());
    m_Filename = newName;
    return !r;
}

FORCEINLINE bool File::SetCursor(usize cur, CursorOrigin cur_position) const
{
    return !fseek(m_File, (long) cur, SEEK_POSITIONS[cur_position]);
}

FORCEINLINE String File::ReadAll()
{
	usize sz = this->Size();
	String data(sz);
	char byte;
	uint64 index = 0;

	while ((byte = fgetc(m_File)) != EOF) {
		// data[index] = byte;
		data.PushBack(byte);
		index++;
	}

	// data[index] = 0;
	data.PushBack(0);
	return data;
}

/**************************************************/
/*               Private Functions                */
/**************************************************/

FORCEINLINE void File::WriteHelper(const char* fmt, va_list ap) const
{
    vfprintf(m_File, fmt, ap);
}

FORCEINLINE int32 File::ReadFormatedHelper(const char* fmt, va_list ap) const
{
    return vfscanf(m_File, fmt, ap);
}

TRE_NS_END