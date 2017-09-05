#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "CopyFile.h"

#include <windows.h>
#include <time.h>

//int main(int argc, char** argv)
//{   
//   // LARGE_INTEGER m_liPerfFreq = { 0 };
//   // QueryPerformanceFrequency(&m_liPerfFreq);
//   // LARGE_INTEGER m_liPerfStart = { 0 };
//   // QueryPerformanceCounter(&m_liPerfStart);
//   ///* if (false == CopyTheFile("D:\\U3D神启源.zip", "E:\\File\\U3D神启源.zip", true))
//   // {
//   //     printf("copyfile failed!\n");
//   //     return false;
//   // }*/
//   // if (false == CopyTheFolder("D:\\TempFile\\JX","E:\\File\\Hello", true))
//   // {
//   //   printf("\ncopy file filed!\n");
//   //   return 0;
//   // }
//   // LARGE_INTEGER liPerfNow = { 0 };
//   // QueryPerformanceCounter(&liPerfNow);
//   // int time = (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
//   // char buffer[100];
//   // sprintf(buffer, "\n执行时间 %d millisecond\n", time);
//   // printf("%s", buffer);
//   // printf("SUCCESS!\n");
//   // return 0;
//}