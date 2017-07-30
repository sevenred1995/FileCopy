
#include<stdio.h>
#include<iostream>
#include "CopyFile.h"
int main(int argc,char** argv)
{
    CusCopyFile co;
    co.CopyFileA("D:\\TempFile\\IP.txt", "E:\\CentOs");
    return 0;
}