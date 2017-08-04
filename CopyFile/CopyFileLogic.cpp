
#include "CopyFileLogic.h"
#include "CopyFile.h"

#include <string.h>
#include <io.h>
#include <direct.h>
#include <sys\types.h>
#include <sys\stat.h>


#include <windows.h>

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

bool CopyFileLogic::CopyFolderToFolder(
    const char *c_szSourceFolderPath,
    const char *c_szDestFolderPath,
    bool  bFailIfExists)
{
    E_PATH_TYPE sourcetype = _GetPathType(c_szSourceFolderPath);
    E_PATH_TYPE destType   = _GetPathType(c_szDestFolderPath);
    if (sourcetype == PATH_IS_DIR && destType == PATH_IS_DIR)
    {
        char szNewdestFolder[MAX_PATH];
        const char *szFolderName = _GetFileName(c_szSourceFolderPath);
        if (
            false == Safestrcpy(szNewdestFolder, c_szDestFolderPath) ||
            false == Safestrcat(szNewdestFolder, "\\") ||
            false == Safestrcat(szNewdestFolder, szFolderName)
           )
        {
            return false;
        }
        if (PATH_NO_EXIST == _GetPathType(szNewdestFolder))
            _MakeDir(szNewdestFolder);
        return _ProcessDir(c_szSourceFolderPath, szNewdestFolder, bFailIfExists);
    }
    //"path is not valid"
    return false;
}

bool CopyFileLogic::CopyFileToFolder(
    const char *c_szSourceFolderPath,
    const char *c_szDestFolderPath,
    bool  bFailIfExists)
{
    E_PATH_TYPE SourceType = _GetPathType(c_szSourceFolderPath);
    E_PATH_TYPE DestType   = _GetPathType(c_szDestFolderPath);
    if (PATH_IS_FILE == SourceType && PATH_IS_DIR == DestType)
    {
        const char* fileName = _GetFileName(c_szSourceFolderPath);
        char newdestFilePath[MAX_PATH];
        memset(newdestFilePath, 0, MAX_PATH);
        if (
            false == Safestrcpy(newdestFilePath, c_szDestFolderPath) ||
            false == Safestrcat(newdestFilePath, "\\") ||
            false == Safestrcat(newdestFilePath, fileName)
            )
        {
            return false;
        }
        return 
            CopyFileByMmap(c_szSourceFolderPath, newdestFilePath, bFailIfExists);
    }
    return false;
}

bool CopyFileLogic::CopyFilesToFolder(
    const char **c_pszSourceFolderPath,
    const char *c_szDestFolderPath,
    bool  bFailIfExists)
{
    if (c_pszSourceFolderPath == NULL)return false;
    const char** pszSourceTemp = c_pszSourceFolderPath;
    while (*pszSourceTemp !='\0')
    {
        if (false == CopyFileToFolder(*pszSourceTemp, c_szDestFolderPath, bFailIfExists))
            return false;
        pszSourceTemp++;
    }
    return true;
}

bool CopyFileLogic::CopyFileToFile(
    const char * c_szSourceFilePath,
    const char * c_szDestFilePath,
    bool bFailIfExists)
{
    E_PATH_TYPE SourceType  = _GetPathType(c_szSourceFilePath);
    if (PATH_IS_FILE == SourceType)
    {
        return
            CopyFileByMmap(c_szSourceFilePath, c_szDestFilePath, bFailIfExists);
    }
    return false;
}

bool CopyFileLogic::_ProcessDir(
    const char *c_szSourthPath, 
    const char *c_szDestPath, 
    bool bFailIfExists)
{
    long    hFile = 0;
    struct  _finddata_t fileInfo;
    char    szSrcFind[MAX_PATH];
    char    szDesFind[MAX_PATH];
    if (false == Safestrcpy(szSrcFind, c_szSourthPath) ||
        false == Safestrcat(szSrcFind, "\\*.*"))
    {
        return false;
    }
    hFile = _findfirst(szSrcFind, &fileInfo);
    memset(szSrcFind, 0, MAX_PATH);
    if (hFile == -1)//search failed 
        return false;
    do
    {
        if (strcmp(fileInfo.name, ".") == 0
            || strcmp(fileInfo.name, "..") == 0)
            continue;
        if ((fileInfo.attrib&_A_SUBDIR)) {
            if (false == Safestrcpy(szSrcFind, c_szSourthPath) ||
                false == Safestrcat(szSrcFind, "\\") ||
                false == Safestrcat(szSrcFind, fileInfo.name))
            {
                return false;
            }
            if (false == Safestrcpy(szDesFind, c_szDestPath) ||
                false == Safestrcat(szDesFind, "\\") ||
                false == Safestrcat(szDesFind, fileInfo.name))
            {
                return false;
            }
            //need one desDir
            if (false == _MakeDir(szDesFind))
            {
                _findclose(hFile);
                return false;
            }
            if (false == _ProcessDir(szSrcFind, c_szDestPath, bFailIfExists))
                return false;
        }
        else
        {
            char srcTemp[MAX_PATH];
            char desTemp[MAX_PATH];
            if (false == Safestrcpy(desTemp, c_szDestPath) ||
                false == Safestrcat(desTemp, "\\") ||
                false == Safestrcat(desTemp, fileInfo.name))
            {
                return false;
            }
            if (false == Safestrcpy(srcTemp, c_szSourthPath) ||
                false == Safestrcat(srcTemp, "\\") ||
                false == Safestrcat(srcTemp, fileInfo.name)
                )
            {
                return false;
            }
            if (false == CopyFileByMmap(srcTemp,desTemp,bFailIfExists))
            {
                _findclose(hFile);
                return false;
            }
        }
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return true;
}

bool CopyFileLogic::_MakeDir(const char *c_szPath)
{
    int nRet = ::_mkdir(c_szPath);
    if (EEXIST == nRet)
        return false;
    if (ENOENT == nRet)
        return false;
    if (0==nRet)
        return true;
}

E_PATH_TYPE CopyFileLogic::_GetPathType(const char *c_szPath)
{
    struct stat fileInfo;
    if (stat(c_szPath, &fileInfo) != 0)
        return PATH_NO_EXIST;
    if (fileInfo.st_mode & _S_IFDIR)
        return PATH_IS_DIR;
    else
        return PATH_IS_FILE;
}

const char *CopyFileLogic::_GetFileName(const char *c_szPath)
{
    int j = strlen(c_szPath);
    while (j > 0 && c_szPath[--j] != '\\')
        NULL;
    return c_szPath + j + 1;
}
