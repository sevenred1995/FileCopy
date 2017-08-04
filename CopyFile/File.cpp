#include "File.h"
#include <windows.h>
FileHandle OpenTheFile(const char * c_szfilePath, bool bTrueIsRead)
{
    unsigned long desiredAccess = -1;
    unsigned long shareMode = 1;
    unsigned long creation = -1;
    HANDLE hFile = NULL;
    if (bTrueIsRead)
    {
        desiredAccess = GENERIC_WRITE | GENERIC_READ;
        shareMode = FILE_SHARE_READ;
        creation = OPEN_ALWAYS;
    }
    else
    {
        desiredAccess = GENERIC_WRITE | GENERIC_READ;
        shareMode     = FILE_SHARE_WRITE;
        creation      = OPEN_ALWAYS;
    }
    WCHAR wszFilePath[256];
    memset(wszFilePath, 0, sizeof(wszFilePath));
    MultiByteToWideChar(CP_ACP, 0,
        c_szfilePath, strlen(c_szfilePath) + 1,
        wszFilePath, sizeof(wszFilePath) / sizeof(wszFilePath[0])
    );
    hFile = CreateFile(
        wszFilePath, desiredAccess, shareMode,
        NULL, creation, FILE_FLAG_SEQUENTIAL_SCAN, NULL
    );
    if (INVALID_HANDLE_VALUE == hFile)
        return NULL;
    return hFile;
}
void   CloseTheFileHandle(FileHandle hFile)
{
    if (INVALID_HANDLE_VALUE != hFile)
        CloseHandle(hFile);
}


uint64_t GetTheFileSize(FileHandle hFile, const char *c_szFilePath)
{
    DWORD dwFileSizeHigh;
    uint64_t qwFileSize = ::GetFileSize(hFile, &dwFileSizeHigh);
    qwFileSize |= (((uint64_t)dwFileSizeHigh) << 32);
    return qwFileSize;
}

bool   DeleteTheFile(const char * c_szFilePath)
{
    WCHAR wszFilePath[256];
    memset(wszFilePath, 0, sizeof(wszFilePath));
    MultiByteToWideChar(CP_ACP, 0, c_szFilePath,
        strlen(c_szFilePath) + 1, wszFilePath,
        sizeof(wszFilePath) / sizeof(wszFilePath[0]));
    if (DeleteFile(wszFilePath))
        return true;
    return false;
}
