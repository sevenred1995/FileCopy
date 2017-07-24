#pragma once
#include "CopyFile.h"

#define ALL 1001
#define ONE 1002
inline 
void CopyAllFolderAndFile(const std::string& sourceURL, const std::string& desURL, std::string suffix)
{
	CusCopyFile copy;
	copy.SetSuffix(suffix);
	copy.CopyAllFile(sourceURL, desURL);
	copy.SetSuffix("");
}
void CopyOneFile(const std::string& sourceURL, const std::string& desURL)
{
	CusCopyFile copy;
	copy.CopyOneFile(sourceURL, desURL);
}