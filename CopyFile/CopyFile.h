#pragma once
#define  _CRT_SECURE_NO_WARNINGS

enum PATH_TYPE { PATH_NO_EXIST = 0, PATH_IS_DIR, PATH_IS_FILE };
enum OPERATE_WHEN_EXIST { OVERRIDE_WHEN_EXIST,NOOPERATION_WHEN_EXIST};
class CusCopyFile
{
public:
    CusCopyFile();
    ~CusCopyFile();
public:
    /*bool CopyFileOrDirectory(
        const char *sourcePath,
        const char *destPath,
        OPERATE_WHEN_EXIST  type
    );*/
    bool CopyFolderToFolder(
        const char *sourceFolderPath,
        const char *destFolderPath,
        OPERATE_WHEN_EXIST typeIfExists
    );
    bool CopyFilesToFolder(
        const char **sourceFilePath,
        const char *destFolderPath,
        OPERATE_WHEN_EXIST typeIfExisted
    );
    bool CopyFileToFolder(
        const char *sourceFilePath,
        const char *destFolderPath,
        OPERATE_WHEN_EXIST typeIfExisted
    );
    bool CopyFileToFile(
        const char *sourceFilePath,
        const char *destFilePath,
        OPERATE_WHEN_EXIST typeIfExists
    );

    const char *GetErrorInfo();
private:
    struct  CopyInfo;
    bool _ProcessDir(const char  *sourceDir, const char *destDir);
    bool _ProcessFile(const char *sourceFilePath,const char *destFilePath);
    bool _CopyProc();
    bool _InitBytesTransInfo(
        const char* sourceFile, const char* desFile
   );
    void _ClearErrInfo();
    bool _MakeCopyInfo(
        CopyInfo   *retInfo,
        const char *sourceDir, 
        const char *desDir,
        const char *fileName
    );
    bool _MakeDir(const char* dir);
    const char *_GetFileName(const char *path);
    PATH_TYPE   _GetPathType(const char* path);
private:
    struct CopyInfo {
        const char* sourcePath;
        const char* destPath;
        const char* fileName;
    };
    struct BytesTransInfo
    {
        const char* sourcePath;
        const char* destPath;
        void*       pSrcMapping;
        void*       pDesMapping;
        unsigned long        startPos;
        unsigned long        endPos;
        unsigned long        offset;
    };

private:
    char m_errorInfo[1024];

    OPERATE_WHEN_EXIST  m_type;
    BytesTransInfo      m_transInfo;
    bool m_inturrput;
};

