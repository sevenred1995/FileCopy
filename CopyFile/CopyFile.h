#pragma once
#define  _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <string.h>
#include <process.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#endif

typedef unsigned long long u_llong;
typedef unsigned long      u_long;
#define BLOCK_SZIE 1024*256
#define MAX_PATH 1024

enum typeCode { NOEXIST = 0, ISDIR, ISFILE };
enum copyType { OVERRIDE=0,NOOPERATION};
class CusCopyFile
{
public:
    CusCopyFile();
    ~CusCopyFile();
private:
    struct CopyInfo {
        const char* srcDir;
        const char* desDir;
        const char* fileName;
    };
    struct BytesTransInfo
    {
        const char* sourceFile;
        const char* desFile;
        void*  pSrcMapping;
        void*  pDesMapping;
        u_llong startPos;
        u_llong endPos;
        u_llong offset;
    };

public:
    bool CopyFileA(const char* sourceFile, const char* desDir, copyType  type);
    bool GetErrInfo(char* &err);
private:
    bool ProcessDir(const char* sourceDir,const char* desDir);
    bool ProcessFile(const CopyInfo& info,bool isDir);
    typeCode GetPathTypeCode(const char* path);
    bool CopyProc();
    bool InitBytesTransInfo(const char* sourceFile,
        const char* desFile);
    
    void ClearErrInfo();
private:
    bool MkCopyInfo(CopyInfo& info,
                            const char* sourceDir, 
                            const char*desDir,
                            const char* fileName);
    bool MkDir(const char* dir);
    const char* GetFileName(const char* path);
    bool Safestrcpy(char* des, const char* src);
    bool Safestrcat(char* des, const char* src);
private:
    char _err_info[1024];
private:
    copyType _type;
    BytesTransInfo  _transInfo;
    bool _inturrput;
};

