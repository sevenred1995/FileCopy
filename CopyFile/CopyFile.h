#pragma once
#define  _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#endif

#define BLOCK_SZIE 1024*256
#define MAX_PATH 100
enum TYPECODE { NOEXIST = 0, ISDIR, ISFILE };
class CusCopyFile
{
private:
    struct CopyInfo {
        const char* srcDir;
        const char* desDir;
        const char* fileName;
    };
    struct BytesTransInfo
    {
        //int threadId;
        void*  pSrcMapping;
        void*  pDesMapping;
        UINT64 startPos;
        UINT64 endPos;
        UINT64 offset;
    };

public:
    bool CopyFileA(const char* sourceFile, const char* desDir);
private:
    bool ProcessDir(const char* sourceDir,const char* desDir);
    bool ProcessFile(const CopyInfo& info,bool isDir);
private:
    TYPECODE GetPathTypeCode(const char* path);

    static unsigned int __stdcall CThreadFunc(void* param);
    bool  CopyProc();
    bool  InitBytesTransInfo(const char* sourceFile,
        const char* desFile);

private:
    bool        MkCopyInfo(CopyInfo& info,
                            const char* sourceDir, 
                            const char*desDir,
                            const char* fileName);
    bool        MkDir(const char* dir);
    const char* GetFileName(const char* path);
private:
    const char* _desDir;
    const char* _srcFile;
private:
    BytesTransInfo  _transInfo;
    bool _inturrput = false;
};

