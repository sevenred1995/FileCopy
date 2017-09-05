#include "FileMapping.h"

#include <string>
#include <assert.h>
#include <windows.h>
typedef struct{} *H;
/* if the file size is zero,the function will success,but the return value is NULL 
 * if the function fails,return value is INVALID_HANDLE_VALUE
*/
Handle CreateFileMappingWithReadMode(const char *filePath, uint64_t  *outFilesize)
{
    assert(filePath != NULL);
    *outFilesize = -1;

    int len = strlen(filePath);
    assert(len < MAX_PATH);

    WCHAR *filePathW = (WCHAR*)malloc(len * sizeof(WCHAR) + 4);
    if (false == ::MultiByteToWideChar(CP_ACP, 0, filePath, -1, filePathW, len + 2)){
        free(filePathW);
        return INVALID_MAPPING_HANDLE_VALUE;
    }
    
    Handle filehandle = ::CreateFile(
        filePathW, GENERIC_READ, FILE_SHARE_READ , NULL, 
        OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN, NULL);
   
    free(filePathW);
    filePathW = NULL;

    if (INVALID_HANDLE_VALUE == filehandle)
    {
        return INVALID_MAPPING_HANDLE_VALUE;
    }

    LARGE_INTEGER  fileSize;
    if (false == ::GetFileSizeEx(filehandle, &fileSize))
    {
        ::CloseHandle(filehandle);
        return INVALID_MAPPING_HANDLE_VALUE;
    }
    //if the file size is zero
    if (0 == (*outFilesize = (uint64_t)fileSize.QuadPart))
    {
        ::CloseHandle(filehandle);
        return NULL;
    }

    Handle mapfhandle = CreateFileMapping(
        filehandle,NULL,PAGE_READONLY,fileSize.HighPart,fileSize.LowPart,NULL
    );
    if (NULL == mapfhandle)
    {
        *outFilesize = -1;
        ::CloseHandle(filehandle);
        return INVALID_MAPPING_HANDLE_VALUE;
    }
    ::CloseHandle(filehandle);
    return mapfhandle;
}

/*if input file size is zero,we only create a newfile but don't createmapping,
 *so return NULL when size is zero,but if fails ,return INVALID_MAPPING_HANDLE_VALUE
*/
Handle CreateFileMappingWithWriteMode(const char *filePath, uint64_t  inFilemappsize)
{
    assert(filePath != NULL);
    int len = strlen(filePath);
    assert(len < MAX_PATH);

    WCHAR *filePathW = (WCHAR*)malloc(len*sizeof(WCHAR) + 4);
    if (NULL == filePathW)
        return INVALID_MAPPING_HANDLE_VALUE;

    if (false == ::MultiByteToWideChar(CP_ACP, 0, filePath, -1, filePathW, len + 2)){
        free(filePathW);
        filePathW = NULL;
        return INVALID_MAPPING_HANDLE_VALUE;
    }
    Handle filehandle = ::CreateFile(
        filePathW, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
        CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    free(filePathW);
    filePathW = NULL;
    if (INVALID_MAPPING_HANDLE_VALUE == filehandle)
    {
        return INVALID_MAPPING_HANDLE_VALUE;
    }
    if (0 == inFilemappsize)
    {
        ::CloseHandle(filehandle);
        return NULL;
    }
    Handle maphandle = CreateFileMapping(
        filehandle, NULL, PAGE_READWRITE, 
        (DWORD)(inFilemappsize >> 32),
        (DWORD)(inFilemappsize & 0xFFFFFFFF), 
        NULL
    );
    if (NULL == maphandle)
    {
        ::CloseHandle(maphandle);
        return INVALID_MAPPING_HANDLE_VALUE;
    }
    ::CloseHandle(filehandle);
    return maphandle;
}

bool   CloseMappingHandle(Handle handle)
{
    return ::CloseHandle(handle);
}

void *MapViewOfFileingWithReadMode(HANDLE mappHandle, uint64_t offset, uint64_t size)
{
    char *mapAdress = (char*)::MapViewOfFile(
        mappHandle, FILE_MAP_READ,
        (DWORD)(offset >> 32),
        (DWORD)(offset & 0xFFFFFFFF),
        (size_t)size
    );
    return  mapAdress;
}
void *MapViewOfFileingWithWriteMode(HANDLE mappHandle, uint64_t offset, uint64_t size)
{
    char *mapAdress = (char*)::MapViewOfFile(
        mappHandle, FILE_MAP_WRITE,
        (DWORD)(offset >> 32),
        (DWORD)(offset & 0xFFFFFFFF),
        (size_t)size
    );
    return mapAdress;
}

bool UnmapViewOfTheFile(void *dataBuffer)
{
    return UnmapViewOfFile(dataBuffer);
}


