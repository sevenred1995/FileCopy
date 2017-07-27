#pragma once
#include "CopyFile.h" 
void CopyAllFolderAndFile(const std::string& sourceURL, const std::string& desURL, std::string suffix)
{
	CusCopyFile copy;
	unsigned int res= copy.CopyAllFile(sourceURL, desURL);
	switch (res)
	{
	case COPY_FILE_EXISTED:
     	break;
	case COPY_FILE_FAILED:
		break;
	case COPY_FILE_SUCCESS:
		break;
	case CREATE_FLODER_FAILED:
		break;
	default:
		break;
	}
}

//改：结构统一  UI与底层分离
void CopyOneFile(const std::string& sourceURL, const std::string& desURL)
{
	CusCopyFile copy;
	unsigned int res = copy.CopyOneFile(sourceURL, desURL);
	switch (res)
	{
	case COPY_FILE_EXISTED:
		printf("%s file has existed\n",desURL.c_str());
		break;
	case COPY_FILE_FAILED:
		//TodO
		break;
	case COPY_FILE_SUCCESS:
		break;
	case SRC_FILE_ERROR:
		printf("src file error\n");
		break;
	default:
	//	PRINTF "UNKNONW ERROR"
		break;
	}
}