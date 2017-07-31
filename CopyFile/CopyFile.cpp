#include "CopyFile.h"
#include "FileMapping.h"





CusCopyFile::CusCopyFile():_inturrput(false)
{
    memset(_err_info, 0, 1024);
}

CusCopyFile::~CusCopyFile()
{
   
}
//sourceFile example:E:\\xx...\\xx or E:\\...\\xx.txt
//deDir      example:E:\\xx...\\xx
bool CusCopyFile::CopyFileA(
    const char * sourceFile, 
    const char * desDir,
    copyType  type)
{
    typeCode desCode = GetPathTypeCode(desDir);
    if (!(desCode&ISDIR))
    {
        sprintf(_err_info, "%s", "destination director failed!");
        return false;//desDir pattern error
    }
    _type = type;
    typeCode srcCode = GetPathTypeCode(sourceFile);
    if (srcCode&NOEXIST)
    {
        sprintf(_err_info, "%s", "source file not exist!");
        return false;
    }
        
    if (srcCode&ISFILE)
    {
        const char* fileName = GetFileName(sourceFile);
        CopyInfo info;
        if (!MkCopyInfo(info, sourceFile, desDir, fileName))
            return false;
        if (!ProcessFile(info, false))
            return false;
    }
    if (srcCode&ISDIR)
    {
        bool ret=ProcessDir(sourceFile,desDir);
        if (ret)
           return false;
    }
    return true; 
}
bool CusCopyFile::GetErrInfo(char * &err)
{
    strcpy(err, _err_info);
    ClearErrInfo();  
    return true;
}
bool CusCopyFile::ProcessDir(const char* sourceDir,const char* desDir)
{
    if (_inturrput) return false;
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
        sprintf(_err_info, "%s", "the length of the src path too long!");
        _inturrput = true;
        return false;
    }
    hFile = _findfirst(srcTemp, &fileInfo);
    memset(srcTemp, 0, MAX_PATH);
    if (hFile==-1)//search failed 
        return false;
    do 
    {
        if (_inturrput)break;
        if(strcmp(fileInfo.name,".")==0
            ||strcmp(fileInfo.name,"..")==0)
            continue;
        if ((fileInfo.attrib&_A_SUBDIR)) {
            if (false == Safestrcpy(srcTemp, sourceDir)||
                false == Safestrcat(srcTemp, "\\")||
                false == Safestrcat(srcTemp, fileInfo.name))
            {
                sprintf(_err_info, "%s", "the length of the src path too long!");
                _inturrput = true;
                return false;
            }
            if (false == Safestrcpy(desTemp, desDir)||
                false == Safestrcat(desTemp, "\\")||
                false == Safestrcat(desTemp, fileInfo.name))
            {
                sprintf(_err_info, "%s", "the length of the des path too long!");
                _inturrput = true;
                return false;
            }
            //need one desDir
            if (false==MkDir(desTemp))
            {
                sprintf(_err_info, "%s", "create director failed!");
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
    return _inturrput;
}

bool CusCopyFile::ProcessFile(const CopyInfo& info,bool isDir)
{
    char srcTemp[MAX_PATH];
    char desTemp[MAX_PATH];
    memset(srcTemp, 0, MAX_PATH);
    memset(desTemp, 0, MAX_PATH);
    if (false == Safestrcpy(desTemp, info.desDir) ||
        false == Safestrcat(desTemp, "\\") ||
        false == Safestrcat(desTemp, info.fileName))
    {
        sprintf(_err_info, "%s", "the length of the des path too long!");
        return false;
    }
        
    if (false == Safestrcpy(srcTemp, info.srcDir))
    {
        sprintf(_err_info, "%s", "the length of the src path too long!");
        return false;
    }
    if (isDir)
    {
        if (false == Safestrcat(srcTemp, "\\") ||
            false == Safestrcat(srcTemp, info.fileName))
        {
            sprintf(_err_info, "%s", "the length of the src path too long!");
            return false;
        }     
    }
    if (0 == ::_access(desTemp, 0))
    {
        if (_type == NOOPERATION)
            return true;
        if (_type == OVERRIDE)
            NULL;
    }
    if (false == InitBytesTransInfo(srcTemp, desTemp))
        return false;
    return CopyProc();
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
            sprintf(_err_info, "%s", "FileMapping::Read Failed!");
            remove(_transInfo.desFile);
            FileMapping::Close(&_transInfo.pSrcMapping);
            FileMapping::Close(&_transInfo.pDesMapping);
            return false;
        }
        if (false == FileMapping::Write(_transInfo.pDesMapping, 
            _transInfo.offset,block, buf))
        {
            sprintf(_err_info, "%s", "FileMapping::Write Failed!");
            remove(_transInfo.desFile);
            FileMapping::Close(&_transInfo.pSrcMapping);
            FileMapping::Close(&_transInfo.pDesMapping);
            return false;
        }
        _transInfo.offset = _transInfo.offset + block;
        if (_transInfo.offset >= _transInfo.endPos)
        {
            FileMapping::Close(&_transInfo.pSrcMapping);
            FileMapping::Close(&_transInfo.pDesMapping);
            break;
        }
    }
    return true;
}

bool CusCopyFile::InitBytesTransInfo(const char* sourceFile,
    const char* desFile)
{
    u_llong size = FileMapping::GetSize(sourceFile);
    memset(&_transInfo, 0, sizeof(_transInfo));
    _transInfo.sourceFile = sourceFile;
    _transInfo.desFile = desFile;
    _transInfo.pSrcMapping = FileMapping::Open(sourceFile, 0);
    if (NULL == _transInfo.pSrcMapping)
    {
        sprintf(_err_info, "%s", "FileMapping::Open sourceFile Failed!");
        return false;
    }
    _transInfo.pDesMapping = FileMapping::Open(desFile, size);
    if (NULL == _transInfo.pDesMapping)
    {
        sprintf(_err_info, "%s", "FileMapping::Open desFile Failed!");
        return false;
    }
    _transInfo.startPos = 0;
    _transInfo.offset = 0;
    _transInfo.endPos = size;
    return true;
}
void CusCopyFile::ClearErrInfo()
{
    memset(_err_info, 0, 1024);
}
//Get the filePath type
//Path_No_Exist  Path_Is_File   Path_Is_Dir
typeCode CusCopyFile::GetPathTypeCode(const char* path)
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

bool CusCopyFile::Safestrcpy(char * des, const char * src)
{
    if (strlen(src) > MAX_PATH - 1)
        return false;
    strcpy(des, src);
    return true;
}

bool CusCopyFile::Safestrcat(char * des, const char * src)
{
    if (strlen(src) + strlen(des) > MAX_PATH - 1)
        return false;
    strcat(des, src);
    return true;
}


