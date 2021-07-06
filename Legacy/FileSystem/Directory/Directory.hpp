#pragma once

#include "Legacy/Misc/Defines/Common.hpp"
#include "Legacy/DataStructure/String.hpp"
#include "Legacy/DataStructure/Vector.hpp"
#include "Legacy/DataStructure/Pair.hpp"

#if defined(OS_LINUX) || defined(OS_UNIX)
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
#elif defined(OS_WINDOWS)
	#include <windows.h>
	#include <pathcch.h>
	#include <fileapi.h>
	#include <handleapi.h>
#endif

TRE_NS_START

class Directory
{
public:
    Directory(const String& path);
	
	~Directory();

    bool IsRepository() const;

    bool IsFile() const;

    uint32 GetStatusChangeTime() const;

    uint32 GetLastFileAcessTime() const;

    uint32 GetFileModificationTime() const;

    uint32 GetOwnerUID() const;

    uint32 GetOwnerGID() const;

    int64 GetSize() const;

    void GetContent(Vector<String>& sub_dirs) const;

    void GetContentEx(Vector<Directory>& sub_dirs) const;

    Directory GetParent() const;

    const String& GetPath() const;

    bool Search(const String& target, String& out_path) const;

    bool Search(const String& target) const;

    bool SearchRecursive(const String& target, String& out_path) const;

private:
    bool SearchRecursiveHelper(const String& target, String& out_path, String temp_path) const;

private:
    String m_DirPath;

#if defined(OS_LINUX) || defined(OS_UNIX)

    struct stat m_Informations;

#elif defined(OS_WINDOWS)

	HANDLE m_Handler;
	WIN32_FIND_DATA m_Informations;
#endif
};

TRE_NS_END
