#pragma once
#define  _CRT_SECURE_NO_WARNINGS
#include <d3d11.h>



enum E_PATH_TYPE { PATH_NO_EXIST = 0, PATH_IS_DIR, PATH_IS_FILE };
class CopyFileLogic
{
public:
    //D:\\Temp->E:\\Temp
    bool CopyFolderToFolder(
        const char *c_szSourceFolderPath,
        const char *c_szDestFolderPath,
        bool  bFailIfExists
    );
    bool CopyFileToFolder(
        const char *c_szSourceFilePath,
        const char *c_szDestFolderPath,
        bool  bFailIfExists
    );

    bool CopyFilesToFolder(
        const char **c_pszSourceFilesPath,
        const char *c_szDestFolderPath,
        bool  bFailIfExists
    );

    bool CopyFileToFile(
        const char *c_szSourceFilePath,
        const char *c_szDestFilePath,
        bool  bFailIfExists
    );

private:
    bool _ProcessDir(
        const char *c_szSourthPath,
        const char *c_szDestPath,
        bool bFailIfExists
    );
    bool           _MakeDir(const char *c_szPath);
    const char    *_GetFileName(const char *c_szPath);
    E_PATH_TYPE    _GetPathType(const char *c_szPath);
};

