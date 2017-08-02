#include "CopyFileUI.h"
#include <stdio.h>

#include <ShlObj.h>



CopyFileUI::CopyFileUI()
{
    memset(_sourcePath, 0, MAX_PATH);
    memset(_desPath, 0, MAX_PATH);
}


CopyFileUI::~CopyFileUI()
{
   
}

void CopyFileUI::Start()
{
    printf("please input the source file path:\n");
    sprintf(_sourcePath, "D:\\TempFile\\IP.txt");
    //scanf("%s", &_sourcePath);
    printf("please input the destination director:\n");
    sprintf(_desPath, "E:\\");
    //scanf("%s", &_desPath);
}

bool CopyFileUI::Update()
{
    printf("%s====¡·%s", _sourcePath, _desPath);
    printf("Begin Copy(Y/N):\n");
    //getchar();
    char ch='Y';
    //scanf("%c", &ch);
    if (ch == 'Y')
    {
        printf("copy...\n");
        if (false == _copyF.CopyFileToFile(_sourcePath, _desPath, 
            OVERRIDE_WHEN_EXIST))
        {
            const char* err=_copyF.GetErrorInfo();
            printf("ERROR:%s\n", err);
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
    return false;  
}

bool CopyFileUI::Close()
{
    return true;
}
