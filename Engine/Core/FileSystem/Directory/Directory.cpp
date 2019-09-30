#include "Directory.hpp"
#include "Core/Misc/Defines/Debug.hpp"

/*********************************************/
/******** LINUX/POS X Implementation *********/
/*********************************************/

TRE_NS_START

#if defined(OS_LINUX) || defined(OS_UNIX)


Directory::Directory(const String& path) : m_DirPath(path)
{
    int32 r = stat(m_DirPath.Buffer(), &m_Informations);
    ASSERTF(r == -1, "Failed to load direcoty informations (Linux implementation)");   
}

Directory::~Directory()
{
}

bool Directory::IsRepository() const
{
    return S_ISDIR(m_Informations.st_mode);
}

bool Directory::IsFile() const
{
    return S_ISREG(m_Informations.st_mode);
}

uint32 Directory::GetStatusChangeTime() const
{
    return m_Informations.st_ctim.tv_sec;
}


uint32 Directory::GetLastFileAcessTime() const
{
    return m_Informations.st_atim.tv_sec;
}

uint32 Directory::GetFileModificationTime() const
{
    return m_Informations.st_mtim.tv_sec;
}

uint32 Directory::GetOwnerUID() const
{
    return m_Informations.st_uid;
}

uint32 Directory::GetOwnerGID() const
{
    return m_Informations.st_gid;
}

int64 Directory::GetSize() const
{
    return m_Informations.st_size;
}

void Directory::GetContent(Vector<String>& sub_dirs) const
{
    DIR* dir = opendir(m_DirPath.Buffer());

    if (dir == NULL)
        return;

    struct dirent* dir_info = readdir(dir);

    while (dir_info != NULL){
        if (dir_info->d_name[0] != '.') {
            String sub_path(dir_info->d_name);
            sub_path += (dir_info->d_type == DT_DIR ? "/" : "");
            sub_dirs.EmplaceBack(sub_path);
        }

        dir_info = readdir(dir);
    }

    closedir(dir);
}

void Directory::GetContentEx(Vector<Directory>& sub_dirs) const
{
    DIR* dir = opendir(m_DirPath.Buffer());

    if (dir == NULL)
        return;

    struct dirent* dir_info = readdir(dir);

    while (dir_info != NULL){
        if (dir_info->d_name[0] != '.') {
            String sub_path(m_DirPath);
            sub_path += dir_info->d_name;
            sub_path += String(dir_info->d_type == DT_DIR ? "/" : "");
            sub_dirs.EmplaceBack(sub_path);
        } 

        dir_info = readdir(dir);
    }

    closedir(dir);
}

Directory Directory::GetParent() const
{
    String parent_path = m_DirPath + "/..";
    DIR* d = opendir(parent_path.Buffer());
    
    if (d == NULL)
        return *this;

    struct dirent* dir_info = readdir(d);
    String parent_name(dir_info->d_name);
    return Directory(parent_name);
}

const String& Directory::GetPath() const
{
    return m_DirPath;
}

bool Directory::Search(const String& target, String& out_path) const
{
    DIR* dir = opendir(m_DirPath.Buffer());

    if (dir == NULL)
        return false;

    struct dirent* dir_info = readdir(dir);
    out_path = m_DirPath;

    while (dir_info != NULL){
        if (strcmp(dir_info->d_name, target.Buffer()) == 0){
            out_path += (dir_info->d_name);
            out_path += (dir_info->d_type == DT_DIR ? "/" : "");
            closedir(dir);
            return true;
        }

        dir_info = readdir(dir);
    }

    out_path.Clear();
    closedir(dir);
    return false;
}

bool Directory::Search(const String& target) const
{
    DIR* dir = opendir(m_DirPath.Buffer());

    if (dir == NULL)
        return false;

    struct dirent* dir_info = readdir(dir);

    while (dir_info != NULL){
        if (strcmp(dir_info->d_name, target.Buffer()) == 0){
            closedir(dir);
            return true;
        }

        dir_info = readdir(dir);
    }

    closedir(dir);
    return false;
}

bool Directory::SearchRecursive(const String& target, String& out_path) const
{
    return SearchRecursiveHelper(target, out_path, m_DirPath);
}

bool Directory::SearchRecursiveHelper(const String& target, String& out_path, String temp_path) const
{
    DIR* dir = opendir(temp_path.Buffer());

    if (dir == NULL)
        return false;

    struct dirent* dir_info = readdir(dir);
    bool is_found = false;

    while (dir_info != NULL && !is_found){
        bool is_dir = dir_info->d_type == DT_DIR;

        if (strcmp(dir_info->d_name, target.Buffer()) == 0){
            out_path = temp_path;
            out_path += (dir_info->d_name);
            out_path += (is_dir ? "/" : "");
            closedir(dir);
            return true;
        }else if(is_dir && dir_info->d_name[0] != '.'){
            String search_path(temp_path);
            search_path += (dir_info->d_name);
            search_path += (is_dir ? "/" : "");
            is_found = SearchRecursiveHelper(target, out_path, search_path);
        }

        dir_info = readdir(dir);
    }

    closedir(dir);
    return is_found;
}


#elif defined(OS_WINDOWS)


Directory::Directory(const String& path) : m_DirPath(path)
{
	m_Handler = FindFirstFileA(path.Buffer(), &m_Informations);
	ASSERTF(m_Handler == INVALID_HANDLE_VALUE, "Failed to load direcoty informations (Windows implementation)");
}

Directory::~Directory()
{
	FindClose(m_Handler);
}

bool Directory::IsRepository() const
{
	return m_Informations.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY;
}

bool Directory::IsFile() const
{
	return m_Informations.dwFileAttributes == FILE_ATTRIBUTE_NORMAL;
}

uint32 Directory::GetStatusChangeTime() const
{
	FILETIME lastWriteTime = m_Informations.ftLastWriteTime;
	SYSTEMTIME systemTime;
	FileTimeToSystemTime(&lastWriteTime, &systemTime);
	return 0;
}


uint32 Directory::GetLastFileAcessTime() const
{
	FILETIME lastAcessTime = m_Informations.ftLastAccessTime;
	SYSTEMTIME systemTime;
	FileTimeToSystemTime(&lastAcessTime, &systemTime);
	return 0;
}

uint32 Directory::GetFileModificationTime() const
{
	FILETIME lastModificationTime = m_Informations.ftLastWriteTime;
	SYSTEMTIME systemTime;
	FileTimeToSystemTime(&lastModificationTime, &systemTime);
	return 0;
}

uint32 Directory::GetOwnerUID() const { return 0; }

uint32 Directory::GetOwnerGID() const { return 0; }

int64 Directory::GetSize() const
{
	return (m_Informations.nFileSizeHigh * (MAXDWORD + 1))  + m_Informations.nFileSizeLow;
}

void Directory::GetContent(Vector<String>& sub_dirs) const
{
	if (m_Informations.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		return;

	WIN32_FIND_DATA info;
	String dir_path(m_DirPath);

	if (((m_DirPath.Back() == '\\' || m_DirPath.Back() == '/') && m_DirPath.Back() != '*')) {
		dir_path += "*";
	}else {
		dir_path += "\\*";
	}

	HANDLE handle = FindFirstFileA(dir_path.Buffer(), &info);
	
	while (FindNextFileA(handle, &info)) {
		if(info.cFileName[0] != '.') {
			String sub_path(dir_path);
			sub_path.PopBack();
			sub_path += info.cFileName;
			// sub_path += (info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY ? "\\" : "");
			sub_dirs.EmplaceBack(sub_path);
		}
	}

	FindClose(handle);
}

void Directory::GetContentEx(Vector<Directory>& sub_dirs) const
{
	if (m_Informations.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		return;

	WIN32_FIND_DATA info;
	String dir_path(m_DirPath);

	if (((m_DirPath.Back() == '\\' || m_DirPath.Back() == '/') && m_DirPath.Back() != '*')) {
		dir_path += "*";
	}else {
		dir_path += "\\*";
	}

	HANDLE handle = FindFirstFileA(dir_path.Buffer(), &info);

	while (FindNextFileA(handle, &info)) {
		if (info.cFileName[0] != '.') {
			String sub_path(dir_path);
			sub_path.PopBack();
			sub_path += info.cFileName;
			// sub_path += (info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY ? "\\" : "");
			sub_dirs.EmplaceBack(sub_path);
		}
	}

	FindClose(handle);
}

Directory Directory::GetParent() const
{
	// TODO : Needs more work.
	String parentDir(m_DirPath.Buffer());
	*(strrchr(parentDir.EditableBuffer(), '\\') + 1) = 0;
	return Directory(parentDir.Buffer());
}

const String& Directory::GetPath() const
{
	return m_DirPath;
}

bool Directory::Search(const String& target, String& out_path) const
{
	if (m_Informations.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		return false;

	out_path = m_DirPath;

	if (((m_DirPath.Back() == '\\' || m_DirPath.Back() == '/') && m_DirPath.Back() != '*')) {
		out_path += "*";
	}else {
		out_path += "\\*";
	}

	WIN32_FIND_DATA info;
	HANDLE handle = FindFirstFileA(out_path.Buffer(), &info);
	out_path.PopBack();

	while (FindNextFileA(handle, &info)) {
		if (strcmp(info.cFileName, target.Buffer()) == 0) {
			out_path += (info.cFileName);
			// out_path += (info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY ? "\\" : "");
			return true;
		}
	}

	FindClose(handle);
	out_path.Clear();
	return false;
}

bool Directory::Search(const String& target) const
{
	if (m_Informations.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		return false;

	String dir_path(m_DirPath);
	if (((m_DirPath.Back() == '\\' || m_DirPath.Back() == '/') && m_DirPath.Back() != '*')) {
		dir_path += "*";
	}else{
		dir_path += "\\*";
	}

	WIN32_FIND_DATA info;
	HANDLE handle = FindFirstFileA(dir_path.Buffer(), &info);

	while (FindNextFileA(handle, &info)) {
		if (strcmp(info.cFileName, target.Buffer()) == 0) {
			return true;
		}
	}

	FindClose(handle);
	return false;
}

bool Directory::SearchRecursive(const String& target, String& out_path) const
{
	if (m_Informations.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		return false;

	String dir_path(m_DirPath);
	if ((m_DirPath.Back() == '\\' || m_DirPath.Back() == '/') && m_DirPath.Back() != '*') {
		dir_path += "*";
	}else {
		dir_path += "\\*";
	}

	return SearchRecursiveHelper(target, out_path, dir_path);
}

bool Directory::SearchRecursiveHelper(const String& target, String& out_path, String temp_path) const
{
	WIN32_FIND_DATA info;
	HANDLE handle = FindFirstFileA(temp_path.Buffer(), &info);
	bool is_found = false;

	while (FindNextFileA(handle, &info) && !is_found) {
		bool is_dir = info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY;

		if (strcmp(info.cFileName, target.Buffer()) == 0) {
			temp_path.PopBack();
			out_path = temp_path;
			out_path += (info.cFileName);
			out_path += (is_dir ? "\\" : "");
			FindClose(handle);
			return true;
		}else if (is_dir && info.cFileName[0] != '.') {
			String search_path(temp_path);
			search_path.PopBack();
			search_path += (info.cFileName);
			search_path += (is_dir ? "\\*" : "");
			is_found = SearchRecursiveHelper(target, out_path, search_path);
		}
	}

	FindClose(handle);
	return is_found;
}

#endif

TRE_NS_END