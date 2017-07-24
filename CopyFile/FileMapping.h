#pragma once
#include <basetsd.h>
#include <string>
class FileMapping
{
public:
	struct Param {
		std::string fileurl;
		unsigned long desiredAccess;
		unsigned long shareMode;
		unsigned long creation;
		unsigned long attribute;
		unsigned long protect;
		unsigned long low;
		unsigned long high;
	};
	static void *Open(const std::string fileURL, UINT64 size);
	static void *Open(void* pparam);
	static UINT64 GetSize(std::string fileURL);
	static bool Read(void * pMmaping, UINT64 offset, unsigned long size, void* buf);
	static bool Write(void * pMmaping, UINT64 offset, unsigned long size, const void* buf);
	static void Close(void ** ppMapping);
};

