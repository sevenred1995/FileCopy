#include "CopyFile.h"
#include "FileMapping.h"
#include "File.h"
#include "Log.h"

#include <io.h>
#include <windows.h>

#define READ_FILE  true
#define WRITE_FILE false

bool CopyFileByMmap(
    const char *c_szSourceFile,
    const char *c_szDestFile,
    bool bFailIfExists)
{
    if (0 == ::_access(c_szDestFile, 0))
    {
        if (bFailIfExists)
            NULL;
        else
        {
            char error[MAX_PATH];
            Log::getInstance()->SetLastErrorInfo("");
            return false;
        }
    }
    //read
    FileHandle hSrcFile = OpenTheFile(c_szSourceFile, READ_FILE);
    if (NULL == hSrcFile)
        return false;
    uint64_t qwFileSize = GetTheFileSize(hSrcFile, c_szSourceFile);

    MappingHandle pSrcMmaping = CreateTheFileMapping(hSrcFile,qwFileSize,READ_FILE);

    CloseTheFileHandle(hSrcFile);

    if (NULL==pSrcMmaping)
    {
        return false;
    }
    //write
    FileHandle hDesFile = OpenTheFile(c_szDestFile, WRITE_FILE);
 
    if (NULL == hDesFile)
        return false;
    //create the handle of mapping for write file
    MappingHandle pDesMmaping = CreateTheFileMapping(hDesFile,qwFileSize,WRITE_FILE);
    CloseTheFileHandle(hDesFile);
    if (NULL == pDesMmaping)
    {
        return false;
    }
    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);
    DWORD dwGran = sysInfo.dwAllocationGranularity;

    uint64_t qwFileOffset = 0;

    DWORD dwBlockBytes = 1000 * dwGran;//(DWORD->uint64)

    while (true)
    {
        if (qwFileSize - qwFileOffset < dwBlockBytes)
            dwBlockBytes = (DWORD)(qwFileSize-qwFileOffset);
        const char *lpSrcMapAddress =
            MapViewOfTheFile(pSrcMmaping, qwFileOffset, dwBlockBytes, READ_FILE);
        if (NULL == lpSrcMapAddress)
        {
            //mmap failed
            return false;
        }
        char *lpDesMapAddress =
            MapViewOfTheFile(pDesMmaping, qwFileOffset, dwBlockBytes, WRITE_FILE);
        if (lpDesMapAddress == NULL)
        {
            UnmapViewOfTheFile(lpSrcMapAddress);
            return false;
        }
        try
        {
            memcpy(lpDesMapAddress, lpSrcMapAddress, dwBlockBytes);
        }
        catch (...)
        {
            UnmapViewOfTheFile(lpSrcMapAddress);
            UnmapViewOfTheFile(lpDesMapAddress);
            CloseFileMappingHandle(&pSrcMmaping);
            CloseFileMappingHandle(&pDesMmaping);
            return false;
        }
        qwFileOffset = qwFileOffset + dwBlockBytes;
        UnmapViewOfTheFile(lpSrcMapAddress);
        UnmapViewOfTheFile(lpDesMapAddress);
        if (qwFileOffset >=qwFileSize)
        {
            CloseFileMappingHandle(&pSrcMmaping);
            CloseFileMappingHandle(&pDesMmaping);
            return true;
        }
    }  
}
