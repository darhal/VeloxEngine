#include "File.hpp"

#if defined(OS_LINUX) || defined(OS_UNIX)
    #include <sys/stat.h>
#elif defined(OS_WINDOWS)
   // #include <fileapi.h>
#endif

TRE_NS_START

/**************************************************/
/*               Static Members                   */
/**************************************************/

const char* const File::FILE_PROCESSING_OPTIONS[6] = {"r", "r+", "w", "w+", "a", "a+"};

const int32 File::SEEK_POSITIONS[3] = { SEEK_SET, SEEK_CUR, SEEK_END };

/**************************************************/
/*               Public Functions                 */
/**************************************************/

File::File(const String& path, Options foptions) : 
    m_File(fopen(path.Buffer(), FILE_PROCESSING_OPTIONS[foptions])), 
    m_Filename(path)
{
}

void File::Write(const char* fmt, ...) const
{
    va_list arg;
    va_start(arg, fmt);
    this->WriteHelper(fmt, arg);
    va_end(arg);
}

/*void File::Write(const String& fmt, ...) const
{
    va_list arg;
	const char* char_fmt = fmt.Buffer();
    va_start(arg, char_fmt);
    this->WriteHelper(fmt.Buffer(), arg);
    va_end(arg);
}*/


int32 File::ReadFormated(const char* fmt, ...) const
{
    va_list arg;
    va_start(arg, fmt);
    int32 res = this->ReadFormatedHelper(fmt, arg);
    va_end(arg);
    return res;
}

/*int32 File::ReadFormated(const String& fmt, ...) const
{
    va_list arg;
	const char* char_fmt = fmt.Buffer();
	va_start(arg, char_fmt);
    int32 res = this->ReadFormatedHelper(fmt.Buffer(), arg);
    va_end(arg);
    return res;
}*/

bool File::ReadString(String* result, usize sz) const
{
	result->Reserve(sz + 4);

	if (fgets(result->EditableBuffer(), (int32) sz, m_File) != NULL)
		return true;

	return false;
}

bool File::ReadString(char* result, usize sz) const
{
	if (fgets(result, (int32) sz, m_File) != NULL)
		return true;

	return false;
}

bool File::Delete()
{
    int32 r = remove(m_Filename.Buffer());

    if (!r){
        m_Filename = "";
        m_File = NULL;
        fclose(m_File);
    }

    return !r;
}

usize File::Size() const
{   
#if defined(OS_LINUX) || defined(OS_UNIX)

    struct stat st;
    stat(m_Filename.Buffer(), &st);
    return st.st_size;

#elif defined(OS_WINDOWS)

    /*HANDLE hFile = CreateFile(m_Filename.Buffer(), GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, NULL);

    ASSERTF(hFile == INVALID_HANDLE_VALUE, "An error occured while trying to get the file size.");

    LARGE_INTEGER size;

    if (!GetFileSizeEx(hFile, &size)){
        CloseHandle(hFile);
        ASSERTF(true, "An error occured while trying to get the file size.");
    }

    CloseHandle(hFile);
    return size.QuadPart;*/
	
	return 0;
#else

    usize old_pos = ftell(m_File);
    fseek(m_File, 0L, SEEK_END);
    usize sz = ftell(m_File);
    fseek(m_File, old_pos, SEEK_SET);
    return sz;
    
#endif
}

File::~File()
{
    if (m_File)
        fclose(m_File);
}


TRE_NS_END