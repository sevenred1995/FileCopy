#pragma once
#include <stdint.h>

typedef void* FileHandle;
//enum E_FILEOPEN_TYPE { OPENFILE_READ, OPENFILE_WRITE };

FileHandle OpenTheFile(const char *c_szfilePath, bool bTrueIsRead);
void       CloseTheFileHandle(FileHandle hFile);
uint64_t   GetTheFileSize(FileHandle hFile,const char* c_szFilePath);
bool       DeleteTheFile(const char* c_szFilePath);