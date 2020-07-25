#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>

class FileSystem
{
public:
    static std::string OpenFile(const char* filter);
    static std::string SaveFile();
};

#endif // FILE_SYSTEM_H

