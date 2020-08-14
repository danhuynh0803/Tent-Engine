#ifdef _WIN32
#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#include <windows.h>
#include <string>

class WindowsFileSystem
{
public:
    static std::string OpenFile(const char* filter);
    static std::string SaveFile(const char* filter, const char* fileType);
    static HRESULT hr;
};

#endif // WINDOWS_FILE_SYSTEM_H
#endif