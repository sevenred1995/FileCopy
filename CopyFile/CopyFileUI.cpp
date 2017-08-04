#include "CopyFileUI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
CopyFileUI::CopyFileUI()
{

}
CopyFileUI::~CopyFileUI()
{
}

void CopyFileUI::StartUI()
{
    memset(m_InputSourcePath, 0, MAX_PATH);
    memset(m_InputDestPath, 0, MAX_PATH);
}

void CopyFileUI::UpdateUI()
{
    //ShowUI
    
}

void CopyFileUI::CloseUI()
{

}
