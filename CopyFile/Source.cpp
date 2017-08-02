#include <stdio.h>
#include <iostream>
#include <time.h>
#include "CopyFileUI.h"
#include <signal.h>
#include <windows.h>
int main(int argc, char** argv)
{
   // Copy("", "", false);
    CopyFileUI ui;
    ui.Start();
    ui.Update();
    
    return 0;
}