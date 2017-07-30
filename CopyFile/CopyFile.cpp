#include "CopyFile.h"

#include <stdio.h>
#include <string.h>
#include <process.h>
#include "FileMapping.h"

bool CusCopyFile::CopyFileA(
    const char * sourceFile, const char * desDir)
{
    TYPECODE desCode = GetPathTypeCode(desDir);
    if (!(desCode&ISDIR))
        return false;//desDir pattern error
    TYPECODE srcCode = GetPathTypeCode(sourceFile);
    if (srcCode&NOEXIST)
        return false;
    if (srcCode&ISFILE)
    {
        const char* fileName = GetFileName(sourceFile);
        CopyInfo info;
        if (!MkCopyInfo(info, sourceFile, desDir, fileName))
            return false;
        ProcessFile(info,false);
    }
    if (srcCode&ISDIR)
    {
        //EnTer The DirProcess func
        return ProcessDir(sourceFile,desDir);
    }
    return true; 
}

bool CusCopyFile::ProcessDir(const char* sourceDir,const char* desDir)
{
    if (_inturrput)return false;
    long    hFile = 0;
    bool    ret   = true;
    struct  _finddata_t fileInfo;
    char    srcTemp[MAX_PATH];
    char    desTemp[MAX_PATH];
    memset(srcTemp, 0, MAX_PATH);
    memset(desTemp, 0, MAX_PATH);
    if (strlen(sourceDir) > MAX_PATH - 1)
        return false;//overflow
    strcpy(srcTemp, sourceDir);
    if (strlen(sourceDir) + strlen("\\*.*") > MAX_PATH - 1)
        return false;
    strcat(srcTemp, "\\*.*");
    hFile = _findfirst(srcTemp, &fileInfo);
    memset(srcTemp, 0, MAX_PATH);
    if (hFile==-1)//search failed 
        return false;
    do 
    {
        if(strcmp(fileInfo.name,".")==0
            ||strcmp(fileInfo.name,"..")==0)
            continue;
        if ((fileInfo.attrib&_A_SUBDIR)) {
            strcpy(srcTemp, sourceDir);
            strcat(srcTemp, "\\");
            if (strlen(srcTemp) + strlen(fileInfo.name) > MAX_PATH - 1)
                return false;//interrupt
            strcat(srcTemp, fileInfo.name);
            strcpy(desTemp, desDir);
            strcat(desTemp,"\\");
            strcat(desTemp,fileInfo.name);
            //need one desDir
            if (false==MkDir(desTemp))
            {
                _findclose(hFile);
                _inturrput = false; 
                return false;
            }
            ProcessDir(srcTemp,desTemp);
        }
        else
        {
            CopyInfo info = { 0 };
            info.srcDir = sourceDir;
            info.desDir = desDir;
            info.fileName = fileInfo.name;
            if (false == ProcessFile(info,true))
            {
                _findclose(hFile);
                _inturrput = true;
                return false;//interrupt all operator or Log the error
            }
        }
    } while (_findnext(hFile,&fileInfo)==0);
    _findclose(hFile);
    return ret;
}

bool CusCopyFile::ProcessFile(const CopyInfo& info,bool isDir)
{
    char srcTemp[MAX_PATH];
    char desTemp[MAX_PATH];
    memset(srcTemp, 0, MAX_PATH);
    memset(desTemp, 0, MAX_PATH);
    strcpy(desTemp, info.desDir);
    strcat(desTemp, "\\");
    strcat(desTemp, info.fileName);

    strcpy(srcTemp, info.srcDir);
    if (isDir)
    {
        strcat(srcTemp, "\\");
        strcat(srcTemp, info.fileName);
    }
    if (0 == _access(desTemp, 0))//if the file have existed,we will compare the 
    {                            //diff between of the Tow file
    }        
    if (false == InitBytesTransInfo(srcTemp, desTemp))
        return false;
    //create one thread to transfor the file info
    HANDLE hTrans = 
        (HANDLE)_beginthreadex(NULL, 0,CThreadFunc, this, 0, NULL);
    WaitForSingleObject(hTrans, INFINITE);
    return true;
}
//Get the filePath type
//Path_No_Exist  Path_Is_File   Path_Is_Dir
TYPECODE CusCopyFile::GetPathTypeCode(const char* path)
{
    long hFile = 0;
    struct _finddata_t fileInfo;
    hFile = _findfirst(path, &fileInfo);
    if (hFile == -1)
        return NOEXIST;
    _findclose(hFile);
    if (fileInfo.attrib&_A_SUBDIR)
        return ISDIR;
    else
        return ISFILE;
}

unsigned int CusCopyFile::CThreadFunc(void * param)
{
    CusCopyFile* pObj = (CusCopyFile*)param;
    if (pObj != NULL)
        pObj->CopyProc();
    return 0;
}

bool CusCopyFile::CopyProc()
{
    unsigned char buf[BLOCK_SZIE];
    while (true)
    {
        memset(buf, 0, BLOCK_SZIE);
        UINT64 block = BLOCK_SZIE;
        if (_transInfo.offset + BLOCK_SZIE > _transInfo.endPos)
            block = _transInfo.endPos - _transInfo.offset;
        if (false == FileMapping::Read(_transInfo.pSrcMapping,
            _transInfo.offset,block, buf))
        {
            CloseHandle(_transInfo.pSrcMapping);
            CloseHandle(_transInfo.pDesMapping);
            return false;
        }
        if (false == FileMapping::Write(_transInfo.pDesMapping, 
            _transInfo.offset,block, buf))
        {
            CloseHandle(_transInfo.pSrcMapping);
            CloseHandle(_transInfo.pDesMapping);
            return false;
        }
        _transInfo.offset = _transInfo.offset + block;
        if (_transInfo.offset >= _transInfo.endPos)
        {
            CloseHandle(_transInfo.pSrcMapping);
            CloseHandle(_transInfo.pDesMapping);
            break;
        }
    }
    return true;
}

bool CusCopyFile::InitBytesTransInfo(const char* sourceFile,
    const char* desFile)
{
    UINT64 size = FileMapping::GetSize(sourceFile);
    memset(&_transInfo, 0, sizeof(_transInfo));
    _transInfo.pSrcMapping = FileMapping::Open(sourceFile, 0);
    if (NULL == _transInfo.pSrcMapping)
    {
        return false;
    }
    _transInfo.pDesMapping = FileMapping::Open(desFile, size);
    if (NULL == _transInfo.pDesMapping)
    {
        return false;
    }
    _transInfo.startPos = 0;
    _transInfo.offset = 0;
    _transInfo.endPos = size;
    return true;
}

bool 
CusCopyFile::MkCopyInfo(CopyInfo & info, 
    const char * sourceDir, 
    const char * desDir, 
    const char * fileName)
{
    info.srcDir = sourceDir;
    info.desDir = desDir;
    info.fileName = fileName;
    return true;
}

bool CusCopyFile::MkDir(const char * dir)
{
    if (0 == ::_access(dir, 0))//file has existed
        return true;
    if (0 == ::_mkdir(dir))
        return true;
    return false;
}

const char * CusCopyFile::GetFileName(const char * path)
{
    int length = strlen(path); 
    int j = length;
    while (j > 0 && path[--j] != '\\')
        NULL;
    char fn[20];
    return path + j + 1;
}
