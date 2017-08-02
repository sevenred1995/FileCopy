#pragma once
#include "CopyFile.h"
#include <windows.h>
class CopyFileUI
{
public:
    CopyFileUI();
    ~CopyFileUI();
public:
    void Start();
    bool Update();
    bool Close();
private:
    char _sourcePath[MAX_PATH];
    char _desPath[MAX_PATH];
    CusCopyFile _copyF;
};

