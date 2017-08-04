#include <basetsd.h>
#include <string>
#include "FileMapping.h"
#include <windows.h>

MappingHandle CreateTheFileMapping(MappingHandle hFile,uint64_t mappingSize,bool bTrueIsRead)
{
    DWORD dwCreateType = 0;
    if (bTrueIsRead)
    {
        dwCreateType = PAGE_READONLY;
    }
    else
    {
        dwCreateType = PAGE_READWRITE;
    }
    HANDLE hMapping = NULL;
    hMapping = CreateFileMapping(
        hFile, NULL, dwCreateType, 
        (DWORD)(mappingSize >> 32),
        (DWORD)(mappingSize & 0xFFFFFFFF),
        NULL
        );
    if (INVALID_HANDLE_VALUE == hMapping)
        return NULL;
    return hMapping;
}

char *MapViewOfTheFile(MappingHandle hMapping, uint64_t qwFileOffset,
    uint64_t qwBlockBytes, bool bTrueIsRead)
{   
    DWORD dwMapViewType = 0;
    if (bTrueIsRead)
    {
        dwMapViewType = FILE_MAP_READ;
    }
    else
    {
        dwMapViewType = FILE_MAP_WRITE;
    }
    if (qwFileOffset < 0 || qwBlockBytes < 0)
        return NULL;
    char* pszMapData = NULL;
    pszMapData = (char*)MapViewOfFile(hMapping, dwMapViewType,
        (DWORD)(qwFileOffset >> 32),
        (DWORD)(qwFileOffset & 0xFFFFFFFF), (size_t)qwBlockBytes);
    return pszMapData;
}

void CloseFileMappingHandle(MappingHandle *ppvMapping)
{
    if (NULL != *ppvMapping)
    {
        CloseHandle(*ppvMapping);
        *ppvMapping = NULL;
    }
}

bool UnmapViewOfTheFile(const char * c_pszData)
{
    return UnmapViewOfFile(c_pszData);
}

