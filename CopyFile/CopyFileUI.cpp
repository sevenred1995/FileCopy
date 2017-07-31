#include "CopyFileUI.h"


CopyFileUI::CopyFileUI()
{
    memset(_sourcePath, 0, MAX_PATH);
    memset(_desPath, 0, MAX_PATH);
}


CopyFileUI::~CopyFileUI()
{
}

bool CopyFileUI::Start()
{
    printf("please input the source file path:\n");
    //scanf("%s", &_sourcePath);
    char src[] = "D:\\TempFile";
    if(strlen(src)<MAX_PATH)
      strcpy(_sourcePath,src);
    printf("please input the destination director:\n");
    //scanf("%s", &_desPath);
    char des[] = "E:\\CentOS";
    if (strlen(des)<MAX_PATH)
      strcpy(_desPath, des);
    return true;
}

bool CopyFileUI::Update()
{
    printf("%s====¡·%s", _sourcePath, _desPath);
    printf("Begin Copy(Y/N):\n");
    char ch;
    scanf("%c", &ch); 
    if (ch == 'Y')
    {
        printf("copy...\n");
        if (false == _copyF.CopyFileA(_sourcePath, _desPath, OVERRIDE))
        {
            char* err = (char*)malloc(1024);
            _copyF.GetErrInfo(err);
            printf("ERROR:%s\n", err);
            free(err);
            return false;
        }
        else
        {
            printf("Copy Successed!\n");
            return true;
        }
    }
    else if (ch == 'N')
    {
        return false;
    }
    else
    {
        return false;
    }
      
}
