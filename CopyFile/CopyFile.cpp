#include "CopyFile.h"
#include "FileMapping.h"


#include <stdio.h>
#include <string.h>
#include <process.h>
#include <sys\types.h>
#include <sys\stat.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#endif
#define BLOCK_SZIE 1024*256

bool Safestrcpy(char * des, const char * src)
{
    if (strlen(src) > MAX_PATH - 1)
        return false;
    strcpy(des, src);
    return true;
}

bool Safestrcat(char * des, const char * src)
{
    if (strlen(src) + strlen(des) > MAX_PATH - 1)
        return false;
    strcat(des, src);
    return true;
}

CusCopyFile::CusCopyFile():m_inturrput(false)
{
    memset(m_errorInfo, 0, 1024);
}

CusCopyFile::~CusCopyFile()
{
    _ClearErrInfo();
}

//"D:\\Temp"->"E:\\FI"-->true-->destFloder= E:\\FI\\Temp
//false-->destFloder=E:\\FI
//"D:\\" D: D ->>D:\\ true-> D:\\Temp false->D:
bool CusCopyFile::CopyFolderToFolder(
    const char *sourceFolderPath,
    const char *destFolderPath,
    OPERATE_WHEN_EXIST typeIfExists)
{
    //处理盘符的情况
    if (strlen(sourceFolderPath) == 1)
    {
        char srcTemp[4];
        memset(srcTemp, 0, 4);
        Safestrcpy(srcTemp,sourceFolderPath);
        Safestrcat(srcTemp,":");
    }
    if (strlen(sourceFolderPath) == 2)
    {

    }
    PATH_TYPE sourcetype = _GetPathType(sourceFolderPath);
    PATH_TYPE destType   = _GetPathType(destFolderPath);
    if (sourcetype == PATH_IS_DIR&&destType==PATH_IS_DIR)
    {
        char newdestFolder[MAX_PATH];
        memset(newdestFolder, 0, MAX_PATH);
        const char* folderName = _GetFileName(sourceFolderPath);
        if (
            false == Safestrcpy(newdestFolder, destFolderPath) ||
            false == Safestrcat(newdestFolder, "\\") ||
            false == Safestrcat(newdestFolder, folderName)
            )
        {
            sprintf(m_errorInfo, "The file name: %s will out of bounds!", 
                newdestFolder);
            return false;
        }
        if (PATH_NO_EXIST == _GetPathType(newdestFolder))
            _MakeDir(newdestFolder);
        if (_ProcessDir(sourceFolderPath, newdestFolder))
            return false;//中断返回
        return true;
    }
    //"path is not valid"
    return false;
}

bool CusCopyFile::CopyFilesToFolder(
    const char ** sourceFilePath, 
    const char * destFolderPath, 
    OPERATE_WHEN_EXIST typeIfExisted)
{
    const char **temp = sourceFilePath;
    
    return false;
}

//"D:\\Temp\\a.txt"->"E:\\Temp"
//"D:\\Temp\\b.txt"->"E:\\" or "E:" or "E"
bool CusCopyFile::CopyFileToFolder(
    const char *sourceFilePath,
    const char *destFolderPath, 
    OPERATE_WHEN_EXIST typeIfExisted)
{
    PATH_TYPE sourceType = _GetPathType(sourceFilePath);
    PATH_TYPE destType   = _GetPathType(destFolderPath);
    if (PATH_IS_FILE == sourceType&&PATH_IS_DIR == destType)
    {
        const char* fileName = _GetFileName(sourceFilePath);
        char newdestFilePath[MAX_PATH];
        memset(newdestFilePath, 0, MAX_PATH);
        if (
            false == Safestrcpy(newdestFilePath, destFolderPath) ||
            false == Safestrcat(newdestFilePath, "\\") ||
            false == Safestrcat(newdestFilePath, fileName)
           )
        {
            sprintf(m_errorInfo, "The file name: %s will out of bounds!",
                newdestFilePath);
            return false;
        }
        CopyFileToFile(sourceFilePath, newdestFilePath, typeIfExisted);
    }
    return false;
}
//"D:\\temp\\a.txt"->"E:\\b.txt" file->file
bool CusCopyFile::CopyFileToFile(
    const char *sourceFilePath,
    const char *destFilePath,
    OPERATE_WHEN_EXIST typeIfExists)
{
    PATH_TYPE type   = _GetPathType(destFilePath);
    if (PATH_IS_FILE == _GetPathType(sourceFilePath))
    {
        if (false == _ProcessFile(sourceFilePath, destFilePath))
            return false;
        return true;
    }
    sprintf(m_errorInfo, "path is not valid");
    return false;
}
const char* CusCopyFile::GetErrorInfo()
{
    const char* str = m_errorInfo;
    return str;
}
bool CusCopyFile::_ProcessDir(const char* sourceDir,const char* desDir)
{
    if (m_inturrput) return false;
    long    hFile = 0;
    bool    ret   = true;
    struct  _finddata_t fileInfo;
    char    srcTemp[MAX_PATH];
    char    desTemp[MAX_PATH];
    memset(srcTemp, 0, MAX_PATH);
    memset(desTemp, 0, MAX_PATH);
    if (false == Safestrcpy(srcTemp, sourceDir)||
        false == Safestrcat(srcTemp, "\\*.*"))
    {
        sprintf(m_errorInfo, "The file name: %s will out of bounds!",
            srcTemp);
        m_inturrput = true;
        return false;
    }
    hFile = _findfirst(srcTemp, &fileInfo);
    memset(srcTemp, 0, MAX_PATH);
    if (hFile==-1)//search failed 
        return false;
    do 
    {
        if (m_inturrput)break;
        if(strcmp(fileInfo.name,".")==0
            ||strcmp(fileInfo.name,"..")==0)
            continue;
        if ((fileInfo.attrib&_A_SUBDIR)) {
            if (false == Safestrcpy(srcTemp, sourceDir)||
                false == Safestrcat(srcTemp, "\\")||
                false == Safestrcat(srcTemp, fileInfo.name))
            {
                sprintf(m_errorInfo, "The file name: %s will out of bounds!",
                    srcTemp);
                m_inturrput = true;
                return false;
            }
            if (false == Safestrcpy(desTemp, desDir)||
                false == Safestrcat(desTemp, "\\")||
                false == Safestrcat(desTemp, fileInfo.name))
            {
                sprintf(m_errorInfo, "The file name: %s will out of bounds!",
                    desTemp);
                m_inturrput = true;
                return false;
            }
            //need one desDir
            if (false== _MakeDir(desTemp))
            {
                sprintf(m_errorInfo, "create a directory: %s failed!", desTemp);
                _findclose(hFile);
                m_inturrput = false; 
                return false;
            }
            _ProcessDir(srcTemp,desTemp);
        }
        else
        {
            char srcTemp[MAX_PATH];
            char desTemp[MAX_PATH];
            memset(srcTemp, 0, MAX_PATH);
            memset(desTemp, 0, MAX_PATH);
            if (false == Safestrcpy(desTemp, desDir) ||
                false == Safestrcat(desTemp, "\\") ||
                false == Safestrcat(desTemp, fileInfo.name))
            {
                sprintf(m_errorInfo, "The file name: %s will out of bounds!",
                    desTemp);
                return false;
            }
            if (false == Safestrcpy(srcTemp, sourceDir) ||
                false == Safestrcat(srcTemp, "\\") ||
                false == Safestrcat(srcTemp, fileInfo.name)
                )
            {
                sprintf(m_errorInfo, "The file name: %s will out of bounds!",
                    srcTemp);
                return false;
            }
            if (false == _ProcessFile(srcTemp,desTemp))
            {
                _findclose(hFile);
                m_inturrput = true;
                return false;//interrupt all operator or Log the error
            }
        }
    } while (_findnext(hFile,&fileInfo)==0);
    _findclose(hFile);
    return m_inturrput;
}

bool CusCopyFile::_ProcessFile(
    const char *sourceFilePath, const char *destFilePath
)
{
    if (0 == ::_access(destFilePath, 0))
    {
        if (m_type == NOOPERATION_WHEN_EXIST)
        {
            sprintf(m_errorInfo, "%s has existed!",destFilePath);
            return false;
        }
        if (m_type == OVERRIDE_WHEN_EXIST)
            NULL;
    }
    if (false == _InitBytesTransInfo(sourceFilePath, destFilePath))
        return false;
    return _CopyProc();
}
bool CusCopyFile::_CopyProc()
{
    unsigned char buf[BLOCK_SZIE];
    while (true)
    {
        memset(buf, 0, BLOCK_SZIE);
        UINT64 block = BLOCK_SZIE;
        if (m_transInfo.offset + BLOCK_SZIE > m_transInfo.endPos)
            block = m_transInfo.endPos - m_transInfo.offset;
        if (false == FileMapping::Read(m_transInfo.pSrcMapping,
            m_transInfo.offset,block, buf))
        {
            sprintf(m_errorInfo, "FileMapping::Read from %s Failed!",
                m_transInfo.sourcePath);
            remove(m_transInfo.destPath);
            FileMapping::Close(&m_transInfo.pSrcMapping);
            FileMapping::Close(&m_transInfo.pDesMapping);
            return false;
        }
        if (false == FileMapping::Write(m_transInfo.pDesMapping, 
            m_transInfo.offset,block, buf))
        {
            sprintf(m_errorInfo, "FileMapping::Write to %s Failed!",
                m_transInfo.destPath );
            remove(m_transInfo.destPath);
            FileMapping::Close(&m_transInfo.pSrcMapping);
            FileMapping::Close(&m_transInfo.pDesMapping);
            return false;
        }
        m_transInfo.offset = m_transInfo.offset + block;
        if (m_transInfo.offset >= m_transInfo.endPos)
        {
            FileMapping::Close(&m_transInfo.pSrcMapping);
            FileMapping::Close(&m_transInfo.pDesMapping);
            break;
        }
    }
    return true;
}
bool CusCopyFile::_InitBytesTransInfo(const char* sourceFile,
    const char* desFile)
{
    u_long size = FileMapping::GetSize(sourceFile);
    memset(&m_transInfo, 0, sizeof(m_transInfo));
    m_transInfo.sourcePath = sourceFile;
    m_transInfo.destPath = desFile;
    m_transInfo.pSrcMapping = FileMapping::Open(sourceFile, 0);
    if (NULL == m_transInfo.pSrcMapping)
    {
        sprintf(m_errorInfo, "FileMapping::Open source file: %s failed!",
            sourceFile);
        return false;
    }
    m_transInfo.pDesMapping = FileMapping::Open(desFile, size);
    if (NULL == m_transInfo.pDesMapping)
    {
        sprintf(m_errorInfo, "FileMapping::Open destional file: %s failed!",
            desFile);
        return false;
    }
    m_transInfo.startPos = 0;
    m_transInfo.offset = 0;
    m_transInfo.endPos = size;
    return true;
}
void CusCopyFile::_ClearErrInfo()
{
    memset(m_errorInfo, 0, 1024);
}
//Get the filePath type
//Path_No_Exist  Path_Is_File   Path_Is_Dir
PATH_TYPE CusCopyFile::_GetPathType(const char* path)
{
    struct stat fileInfo;
    if (stat(path, &fileInfo) != 0)
        return PATH_NO_EXIST;
    if (fileInfo.st_mode & _S_IFDIR)
        return PATH_IS_DIR;
    else
        return PATH_IS_FILE;
}

bool
CusCopyFile::_MakeCopyInfo(
    CopyInfo *info,
    const char * sourceDir, 
    const char * desDir, 
    const char * fileName)
{
    info->sourcePath = sourceDir;
    info->destPath = desDir;
    info->fileName = fileName;
    return true;
}

bool CusCopyFile::_MakeDir(const char * dir)
{
    if (0 == ::_access(dir, 0))//file has existed
        return true;
    if (0 == ::_mkdir(dir))
        return true;
    return false;
}

const char * CusCopyFile::_GetFileName(const char * path)
{
    int length = strlen(path); 
    int j = length;
    while (j > 0 && path[--j] != '\\')
        NULL;
    char fn[20];
    return path + j + 1;
}




