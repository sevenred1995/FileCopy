#pragma once
#include "CopyFileLogic.h"

#define MAX_PATH 260

class CopyFileUI
{
public:
    CopyFileUI();
    ~CopyFileUI();
public:
//    void InitializeUI();
    void StartUI();
    void UpdateUI();
    void CloseUI();
private:
    char m_InputSourcePath[MAX_PATH];
    char m_InputDestPath[MAX_PATH];
    CopyFileLogic m_logic;
};