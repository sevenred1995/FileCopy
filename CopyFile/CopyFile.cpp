#define _CRT_SECURE_NO_WARNINGS

#include "CopyFile.h"
#include <errno.h>
#include <assert.h>
#include <queue>

#ifdef _WIN32
#include "FileMapping.h"
#include <io.h>
#include <direct.h>
#include <windows.h>
#include <sys\types.h>
#include <sys\stat.h>


#define PATH_CONNECTOR_STR "\\"
#define PATH_CONNECTOR_CH  '\\'
#define PATH_FILTER      "*.*"

#define IFDIR  _S_IFDIR

#define ACCESS(path) _access((path),0)
#define MKDIR(path)  _mkdir((path))

#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <limits>
#include <unistd.h>   
#include <fcntl.h>   
#include <dirent.h>


#define PATH_CONNECTOR_STR "/"
#define PATH_CONNECTOR_CH  '/'
#define MAX_PATH  PATH_MAX

#define IFDIR  S_IFDIR

#define ACCESS(path) access((path),0)
#define MKDIR(path)       mkdir((path),S_IRWXU)


#endif 

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif


#include <stdio.h>
#include <stdlib.h>

enum PathType
{
    PATH_IS_DIR,
    PATH_IS_FILE,
    PATH_NO_EXIST
};
struct Node
{
    char path[MAX_PATH];
    struct Node *next;
};
class pathList 
{
public:
    ~pathList()
    {
        _clear();
    }
    pathList()
    {
        mhead = NULL;
        mtail = NULL;
    }
    bool  push(const char *_path)
    {
        assert(_path != NULL);
        Node *tempNode = (Node*)malloc(sizeof(Node));
        if (strlen(_path) >= MAX_PATH)
        {
            free(tempNode);
            tempNode = NULL;
            return false;
        }
        strcpy(tempNode->path, _path);
        if (mhead == NULL)
            tempNode->next = mtail;
        else
            tempNode->next = mhead;
        mhead = tempNode;
        return true;
    }
    bool  pop()
    {
        assert(mhead != NULL);
        Node *tempNode = mhead;
        mhead = mhead->next;
        tempNode->next = NULL;
        free(tempNode);
        tempNode = NULL;
        return true;
    }
    int   size()
    {
        int size = 0;
        Node *tempfirst = mhead;
        while (tempfirst != NULL)
        {
            size++;
            tempfirst = tempfirst->next;
        }
        return size;
    }
    bool empty()
    {
        return (mhead == mtail);
    }
    const char *top()
    {
        assert(mhead != NULL);
        return mhead->path;
    }
private:
    bool _clear()
    {
        while (mhead != mtail)
        {
            if (false == pop())
                return false;
        }
        return true;
    }
private:
   struct Node *mhead;
   struct Node *mtail;
};


#pragma region
//nRetstr=srcPath+"\\"+name;
bool  ContactfullPath(const char *sourcePath, const char *name,  char (*outPath)[MAX_PATH])
{
    assert(name != NULL);
    assert(sourcePath != NULL);
    int pathTempLen = strlen(sourcePath);
    if (pathTempLen > MAX_PATH)
        return false;
    bool endWithPathConnecter = false;
    if (sourcePath[pathTempLen - 1] == PATH_CONNECTOR_CH)
        endWithPathConnecter = true;
    int  nLen = 0;
    if (endWithPathConnecter)
        nLen = pathTempLen + strlen(name);
    else
        nLen = pathTempLen + strlen(name) + strlen(PATH_CONNECTOR_STR);
    if (nLen > MAX_PATH)
        return false;
    strcpy(*outPath, sourcePath);//(*outPath) = sourcePath
    if (false==endWithPathConnecter)
        strcat(*outPath, PATH_CONNECTOR_STR); //*outPath+='\\'
    strcat(*outPath, name);//*outPath+filename
    
    return true;
}

const char *ContactfullPathWithmalloc(const char *sourcePath, const char *name)
{
    assert(name != NULL);
    assert(sourcePath != NULL);
    int pathTempLen = strlen(sourcePath);
    if (pathTempLen > MAX_PATH)
        return NULL;
    bool endWithPathConnecter = false;
    if (sourcePath[pathTempLen - 1] == PATH_CONNECTOR_CH)
        endWithPathConnecter = true;
    int  nLen = 0;
    if (endWithPathConnecter)
        nLen = pathTempLen + strlen(name);
    else
        nLen = pathTempLen + strlen(name) + strlen(PATH_CONNECTOR_STR);
    if (nLen > MAX_PATH)
        return NULL;

    char *newPath = (char*)malloc(nLen+1);
    strcpy(newPath, sourcePath);//(*outPath) = sourcePath
    if (false == endWithPathConnecter)
        strcat(newPath, PATH_CONNECTOR_STR); //*outPath+='\\'
    strcat(newPath, name);//*outPath+filename
    return newPath;
}
//if the path not exists,
bool  TryMakeDir(const char *path)
{
    assert(path != NULL);
    if (0 ==   ACCESS(path))
        return true; 
    int nRet = MKDIR(path);
    if (0 == nRet)
        return true;
    return false;
}

//if the full file path end with '\\' or '/',the stat can not process it,
//we can judge it by access function.
PathType GetPathType(const char *path)
{
    assert(path != NULL);

    struct stat fileInfo;

    if (0 == stat(path, &fileInfo))//if path is invalid or path str end with '\\' 
    {
        if (fileInfo.st_mode & IFDIR)
            return PATH_IS_DIR;
        else
            return PATH_IS_FILE;
    }
    bool PATH_ENDWITH_CONNECTOR_OR_PATH_IS_INVALID = true;
    if (PATH_ENDWITH_CONNECTOR_OR_PATH_IS_INVALID)
    {
        if (0 == ACCESS(path))
            return PATH_IS_DIR;
        return PATH_NO_EXIST;
    }
  
}
#pragma endregion

bool _CopyFileByMmap(const char *sourceFile, const char *destFile)
{
    assert(sourceFile != NULL);
    assert(destFile   != NULL);
#ifdef _WIN32
    //read
    uint64_t qwFileSize = 0;
    Handle readmapping = CreateFileMappingWithReadMode(sourceFile,&qwFileSize);
    if (INVALID_MAPPING_HANDLE_VALUE == readmapping)
    {
        return false;
    }
    Handle writemapping = CreateFileMappingWithWriteMode(destFile, qwFileSize);
    if (INVALID_MAPPING_HANDLE_VALUE == writemapping)
    {
        return false;
    }
    if (NULL == writemapping && 0 == qwFileSize)
    {
        return true;
    }

    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);
    DWORD dwGran = sysInfo.dwAllocationGranularity;
    uint64_t qwFileOffset = 0;
    DWORD dwBlockBytes = 1023 * dwGran;

    while (true)
    {
        if (qwFileSize - qwFileOffset < dwBlockBytes)
            dwBlockBytes = (DWORD)(qwFileSize - qwFileOffset);
        char *srcMapAddress =
            (char*)MapViewOfFileingWithReadMode(readmapping, qwFileOffset, dwBlockBytes);
       
        if (NULL == srcMapAddress)
        {
            bool closeRet = CloseMappingHandle(readmapping);
            assert(closeRet);
            closeRet = CloseMappingHandle(writemapping);
            assert(closeRet);
            return false;
        }
        char *desMapAddress =
            (char*)MapViewOfFileingWithWriteMode(writemapping, qwFileOffset, dwBlockBytes);
       
        if (desMapAddress == NULL)
        {
            bool unmapRet = UnmapViewOfTheFile(srcMapAddress);
            assert(unmapRet);
            bool closeRet = CloseMappingHandle(readmapping);
            assert(closeRet);
            closeRet = CloseMappingHandle(writemapping);
            assert(closeRet);
            return false;
        }
        try
        {
            memcpy(desMapAddress, srcMapAddress, dwBlockBytes);
            float progress = (float)(qwFileOffset) / qwFileSize;
            printf("\r:%0.2f%%", progress * 100);
        }
        catch (...)
        {
            bool unmapRet = UnmapViewOfTheFile(srcMapAddress);
            assert(unmapRet);
            unmapRet = UnmapViewOfTheFile(desMapAddress);
            assert(unmapRet);
            bool closeRet = CloseMappingHandle(readmapping);
            assert(closeRet);
            closeRet = CloseMappingHandle(writemapping);
            assert(closeRet);
            return false;
        }
        
        qwFileOffset = qwFileOffset + dwBlockBytes;

        bool unmapRet = UnmapViewOfTheFile(srcMapAddress);
        assert(unmapRet);
        unmapRet = UnmapViewOfTheFile(desMapAddress);
        assert(unmapRet);

        if (qwFileOffset >= qwFileSize)
        {
            bool closeRet = CloseMappingHandle(readmapping);
            assert(closeRet);
            closeRet = CloseMappingHandle(writemapping);
            assert(closeRet);
            return true;
        }
    }
#else
    int src_fd;
    int dst_fd;
    if ((src_fd = open(sourceFile, O_RDONLY)) < 0){
        //printf("file1 open failed！\n");
        return false;
    }
    if ((dst_fd = open(destFile, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {
        //printf("file2 open failed! \n");
        return false;
    }
    struct stat stat;
    fstat(src_fd, &stat);               // 获取文件信息  
    truncate(destFile, stat.st_size);    // 设置文件大小 

    unsigned long size=1024*1024*4;//1024*getpagesize();
    unsigned long offset=0;
    unsigned long totalSize=stat.st_size;
    while (true)
    {    
        if(offset+size>totalSize)
            size=totalSize-offset;


        void *src_ptr=mmap(NULL, size, PROT_READ, MAP_PRIVATE, src_fd, offset);
        void *dst_ptr=mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, dst_fd, offset);

        if (dst_ptr == MAP_FAILED) {
           // printf("mmap error: %s\n", strerror(errno));
            return false;
        }

        if(NULL==memcpy(dst_ptr,src_ptr,size))
        {
            munmap(src_ptr,size);
            munmap(dst_ptr,size);
            //printf("copy failed!\n");
            return false;
        }
        munmap(src_ptr,size);
        munmap(dst_ptr,size);
        offset = offset + size;
        if (offset >= totalSize)
        {
           // printf("copy finished!\n");
            close(src_fd);
            close(dst_fd);
            return true;
        }
      }
#endif
}
bool _ProcessFolder (const char *sourcePath, const char *destPath, bool failIfExists)
{
    assert(sourcePath != NULL && destPath != NULL);
    if (false == TryMakeDir(destPath))
    {
        return false;//create error;
    }
    char srcPathTemp[MAX_PATH];
    char desPathTemp[MAX_PATH];
    
    //char *psrcPathTemp = srcPathTemp;
    //char *pdesPathTemp = desPathTemp;

#ifdef _WIN32
    long     hFile = 0;
    struct   _finddata_t fileInfo;
    if (false == ContactfullPath(sourcePath, PATH_FILTER , &srcPathTemp))
    {
        return false;
    }
    hFile = _findfirst(sourcePath, &fileInfo);

    if (hFile == -1)//search failed 
    {
        return false;
    }
    do
    {
        memset(srcPathTemp, 0, MAX_PATH);
        memset(desPathTemp, 0, MAX_PATH);
        ContactfullPath(sourcePath, fileInfo.name, &srcPathTemp);
        ContactfullPath(destPath,   fileInfo.name, &desPathTemp);
        if ((fileInfo.attrib & _A_SUBDIR)) {
            if (strcmp(fileInfo.name, ".") == 0
                || strcmp(fileInfo.name, "..") == 0)
                continue;
            if (false == _ProcessFolder(srcPathTemp, desPathTemp, failIfExists))
            {
                if (hFile != NULL)
                    _findclose(hFile);
                return false;
            }
        }
        else
        {
            if (PATH_IS_FILE == GetPathType(desPathTemp) && !failIfExists)
                return false; //return file has exist
            if (false == _CopyFileByMmap(srcPathTemp, desPathTemp))
            {
                if (hFile != NULL)
                    _findclose(hFile);
                return false;
            }
        }
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);

#else
    if (strlen(sourcePath) > MAX_PATH)
    {
        return false;
    }
    strcat(srcPathTemp, sourcePath);
    struct dirent *fileInfo;
    DIR *dp  = opendir(srcPathTemp);
    fileInfo = readdir(dp);
    if (NULL == fileInfo)
    {
        return false;
    }
    do 
    {
        memset(srcPathTemp, 0, MAX_PATH + 1);
        memset(desPathTemp, 0, MAX_PATH + 1);
        ContactfullPath(sourcePath, fileInfo.d_name, &srcPathTemp);
        ContactfullPath(destPath,   fileInfo.d_name, &desPathTemp);
        if (PATH_IS_DIR==GetPathType(psrcPathTemp))
        {
            if (strcmp(fileInfo->d_name, ".") == 0
                || strcmp(fileInfo->d_name, "..") == 0)
                continue ;
            if (false == CusCopyFolder(srcPathTemp, desPathTemp, failIfExists))
            {
                if (!dp)
                    closedir(dp);
                return false;
            }
        }
        else if (PATH_IS_FILE == GetPathType(srcPathTemp))
        {
            if(!failIfExists)
                return false;
            if (false == _CopyFileByMmap(srcPathTemp, desPathTemp))
            {
                if (!dp)
                    closedir(dp);
                return false;
            }
        }
    } while (fileInfo = readdir(dp));
    closedir(dp);
#endif
    return true;
}


//BFS
bool _ProcessFolderWithNonrecursive(
    const char *sourcePath, const char *destPath, bool failIfExists
)
{
    assert(sourcePath != NULL && destPath != NULL);
    if (false == TryMakeDir(destPath))
        return false;
    pathList srcfolders;
    pathList desfolders;
    if (!desfolders.push(destPath))
        return false;
    if (!srcfolders.push(sourcePath))
        return false;
    long hFile;
    struct _finddata_t fileInfo;

    
    while (!srcfolders.empty())
    {
        char tempsrcFolder[MAX_PATH];
        char tempdesFolder[MAX_PATH];
        strcpy(tempsrcFolder, srcfolders.top());
        strcpy(tempdesFolder, desfolders.top());
       
        desfolders.pop();
        srcfolders.pop();

        char tempdescopyPath[MAX_PATH];
        char tempsrcfindPath[MAX_PATH];
        if (false == ContactfullPath(tempsrcFolder, PATH_FILTER, &tempsrcfindPath))
        {
            return false;
        }
        hFile = _findfirst(tempsrcfindPath, &fileInfo);
        if (hFile == -1)
        {
            return false;
        }
        do 
        {
            //if is folder
            if (fileInfo.attrib & _A_SUBDIR)
            {
                //skip the . and ..
                if (strcmp(fileInfo.name, ".") == 0
                    || strcmp(fileInfo.name, "..") == 0)
                    continue;
                if (false == ContactfullPath(tempsrcFolder, fileInfo.name, &tempsrcfindPath) ||
                    false == ContactfullPath(tempdesFolder, fileInfo.name, &tempdescopyPath) )
                {
                    if (hFile)
                        _findclose(hFile);
                    return false;
                }
                //create dest dir.
                if (false == TryMakeDir(tempdescopyPath))
                {
                    if (hFile)
                        _findclose(hFile);
                    return false;
                }

                desfolders.push(tempdescopyPath);
                srcfolders.push(tempsrcfindPath); 
            }
            else
            {
                if (false == ContactfullPath(tempsrcFolder, fileInfo.name, &tempsrcfindPath) ||
                    false == ContactfullPath(tempdesFolder, fileInfo.name, &tempdescopyPath))
                {
                    if (hFile)
                        _findclose(hFile);
                    return false;
                }
                if (PATH_IS_FILE == GetPathType(tempdescopyPath) && !failIfExists)
                {
                    if (hFile)
                        _findclose(hFile);
                    return false;
                }
                if (false == _CopyFileByMmap(tempsrcfindPath, tempdescopyPath))
                {
                   if (hFile)
                   _findclose(hFile);
                   return false;
                }
            }
        } while (_findnext(hFile,&fileInfo) == 0);

    }
    if (hFile)
        _findclose(hFile);
    return true;
}


bool CopyTheFolder(const char *sourcePath, const char *destPath, bool failIfExists)
{
    if (sourcePath == NULL || destPath == NULL)
        return false;
    PathType srctypeRet = GetPathType(sourcePath);
    PathType destypeRet = GetPathType(destPath);
    if (PATH_IS_DIR != srctypeRet || PATH_IS_DIR != destypeRet)
        return false;
    return _ProcessFolder(sourcePath, destPath, failIfExists);
        
}
bool CopyTheFile(const char *sourcePath, const char *destPath, bool failIfExists)
{
    if (sourcePath == NULL || destPath == NULL)
        return false;
    PathType srctypeRet = GetPathType(sourcePath);
    //source file is invalid
    if (PATH_IS_FILE != srctypeRet)
        return false;
    PathType destypeRet = GetPathType(destPath);
    //dest file has existed but failIfExists is false;
    if (PATH_IS_FILE == destypeRet && !failIfExists)
        return false;//return error info with file has exist
   return _CopyFileByMmap(sourcePath, destPath);
}

int main()
{
   // _ProcessFolder("D:\\TempFile", "E:\\TempFile", true);
   
    uint64_t fileSize = 0;
    //读方式打开文件
    Handle readHandle   = CreateFileMappingWithReadMode("D:\\TempFile\\TS.txt", &fileSize);
 
    char *rfile = (char *)MapViewOfFileingWithReadMode(readHandle, 0, fileSize);
    
    UnmapViewOfTheFile(rfile);


    CloseMappingHandle(readHandle);


    _CrtDumpMemoryLeaks();
    return 0;
}