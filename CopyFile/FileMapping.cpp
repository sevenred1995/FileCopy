#include "FileMapping.h"
#include <windows.h>

void * FileMapping::Open(const std::string fileURL, UINT64 size)
{
	//if (fileURL.empty())
		//return NULL;
	unsigned long desiredAccess = -1;
	unsigned long shareMode = 1;
	unsigned long creation = -1;
	unsigned long protect = -1;
	if (size < 1) {//read
		desiredAccess  = GENERIC_WRITE|GENERIC_READ;
		shareMode      = FILE_SHARE_READ;
		creation       = OPEN_ALWAYS;
		protect        = PAGE_READONLY;
	}
	else//write
	{
		desiredAccess  = GENERIC_WRITE | GENERIC_READ;
		shareMode      = FILE_SHARE_WRITE;
		creation       = OPEN_ALWAYS;
		protect        = PAGE_READWRITE;
	}
	HANDLE hFile;
	try
	{
		WCHAR wszClassName[256];
		memset(wszClassName, 0, sizeof(wszClassName));
		MultiByteToWideChar(CP_ACP, 0, fileURL.c_str(), strlen(fileURL.c_str()) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));
		hFile = CreateFile(wszClassName, desiredAccess, shareMode, NULL, creation, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	}
	catch (...)
	{
		return NULL;
	}
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return NULL;
	}
	unsigned long low, high;
	if (size < 1)
	{
		try
		{
			low = ::GetFileSize(hFile,&high);
		}
		catch (...)
		{
			CloseHandle(hFile);
			return NULL;
		}
	}
	else
	{
		ULARGE_INTEGER _size;
		_size.QuadPart = size;
		low           = _size.LowPart;
		high          = _size.HighPart;
	}
	HANDLE hMapping = NULL;
	try
	{
		hMapping = CreateFileMapping(hFile, NULL, protect, high, low, NULL);
		//hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, __high, __low, NULL);
	}
	catch (...)
	{
		CloseHandle(hFile);
		return NULL;
	}
	if (NULL == hMapping)
	{
		CloseHandle(hFile);
		return NULL;
	}
	CloseHandle(hFile);
	return hMapping;
}

void * FileMapping::Open(void * pparam)
{
	if (NULL == pparam)
		return NULL;
	Param* pDetail = (Param*)pparam;
	if (NULL == pDetail)
		return NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	try
	{
		WCHAR wszClassName[256];
		memset(wszClassName, 0, sizeof(wszClassName));
		MultiByteToWideChar(CP_ACP, 0, pDetail->fileurl.c_str(), strlen(pDetail->fileurl.c_str()) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));
		hFile= CreateFile(wszClassName, pDetail->desiredAccess, pDetail->shareMode, NULL, pDetail->creation, pDetail->attribute, NULL);
	}
	catch (const std::exception&)
	{
		return NULL;
	}
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return NULL;
	}
	HANDLE hMapping = NULL;
	try
	{
		hMapping = CreateFileMapping(hFile, NULL, pDetail->protect, pDetail->low, pDetail->high, NULL);
	}
	catch (...)
	{
		CloseHandle(hFile);
		return NULL;
	}
	if (NULL == hMapping)
	{
		CloseHandle(hFile);
		return NULL;
	}
	CloseHandle(hFile);
	return hMapping;
}

UINT64 FileMapping::GetSize(std::string fileURL)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	try
	{
		WCHAR wszClassName[256];
		memset(wszClassName, 0, sizeof(wszClassName));
		MultiByteToWideChar(CP_ACP, 0, fileURL.c_str(), strlen(fileURL.c_str()) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));
		hFile = CreateFile(wszClassName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	catch (const std::exception&)
	{
		return INVALID_FILE_SIZE;
	}
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return INVALID_FILE_SIZE;
	}

	unsigned long low, high;
	try
	{
		low = ::GetFileSize(hFile, &high);
	}
	catch (...)
	{
		CloseHandle(hFile);
		return INVALID_FILE_SIZE;
	}

	ULARGE_INTEGER size;
	size.LowPart  = low;
	size.HighPart = high;
	CloseHandle(hFile);
	return size.QuadPart;
}

bool FileMapping::Read(void * pMmaping, UINT64 offset, unsigned long size, void * buf)
{
	if (NULL == pMmaping)
		return false;
	if (offset < 0 || size < 0)
		return false;
	ULARGE_INTEGER temp;
	temp.QuadPart = offset;
	void* pdata = NULL;
	try
	{
		pdata = MapViewOfFile(pMmaping, FILE_MAP_READ, temp.HighPart, temp.LowPart, size);
	}
	catch (...)
	{
		return false;
	}
	if (NULL == pdata)
		return false;
	try{memcpy(buf, pdata, size);}
	catch (...)
	{
		try	{UnmapViewOfFile(pdata);}
		catch (...){}
		return false;
	}
	bool ret = false;
	try
	{ret = UnmapViewOfFile(pdata);}
	catch (...)
	{return false;}
	if (false == ret)
		return false;
	return true;
}

bool FileMapping::Write(void * pMmaping, UINT64 offset, unsigned long size, const void * buf)
{

	if (NULL == pMmaping)
		return false;
	if (offset < 0 || size < 0)
		return false;
	ULARGE_INTEGER temp;
	temp.QuadPart = offset;
	void* pdata = NULL;
	try
	{
		pdata = MapViewOfFile(pMmaping, FILE_MAP_WRITE, temp.HighPart, temp.LowPart, size);
	}
	catch (...)
	{
		return false;
	}
	if (NULL == pdata)
		return false;
	try { 
		memcpy(pdata,buf, size);
	}
	catch (...)
	{
		try { UnmapViewOfFile(pdata); }
		catch (...) {}
		return false;
	}
	bool __ret = false;
	try
	{
		__ret = UnmapViewOfFile(pdata);
	}
	catch (const std::exception&)
	{
		return false;
	}
	if (false == __ret)
		return false;
	return true;
}

void FileMapping::Close(void ** ppMapping)
{
	try
	{
		if (NULL != *ppMapping)
		{
			CloseHandle(*ppMapping);
			*ppMapping = NULL;
		}
	}
	catch (...)
	{
	}
}
