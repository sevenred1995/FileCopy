#pragma once
#include "CopyFile.h"

class CopyFileUI
{
public:
    CopyFileUI();
    ~CopyFileUI();
public:
    bool Start();
    bool Update();
private:
    char _sourcePath[MAX_PATH];
    char _desPath[MAX_PATH];
    CusCopyFile _copyF;
};

