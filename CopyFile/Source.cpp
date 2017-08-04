#include <stdio.h>
#include "CopyFile.h"
int main(int argc, char** argv)
{   
    CopyFileByMmap("D:\\TempFile\\TX.txt","E:\\IP.txt",true);
    return 0;
}