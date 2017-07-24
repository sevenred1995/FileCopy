#pragma once
#include <string>
#include <vector>
#include <map>
#include <windows.h>


#if defined  _WIN32                                                         //Windows
#include <Windows.h>
#define CLock_Mutex_t                 HANDLE
#define CLock_Mutex_Init(_mutex)      (_mutex = CreateSemaphore(NULL,1,1,NULL))
#define CLock_Mutex_Lock(_mutex)      (WaitForSingleObject(_mutex, INFINITE))
#define CLock_Mutex_UnLock(_mutex)    (ReleaseSemaphore(_mutex,1,NULL))
#define CLock_Mutex_Destroy(_mutex)   (CloseHandle(_mutex))
#endif

//source
//Destination Folder /
class CusCopyFile {
public:
	CusCopyFile() :_fileSuffix("") { CLock_Mutex_Init(mutex); }
	void SetSuffix(std::string suffix) { _fileSuffix = suffix; }
public:
	struct ThreadCopyInfo  
	{
		int   threadId;
		void* pSrcMapping;  //mapping object of the source file
		void* pDesMapping;  // mapping object of the destination file
		UINT64 startPos;
		UINT64 endPos;
		UINT64 offset;     //currnt thread's offset
	};
	struct ThreadParam
	{
		int threadId;
		CusCopyFile* pObj;
	};

public:
	bool CopyAllFile(const std::string& sourceFolderURL,const std::string& destinationFolderURL);
	bool CopyOneFile(const std::string& sourceFileURL, const std::string& destinationFolderURL);
private:
	void  CopyFileFromCache(const std::vector<std::string>& files);
	bool  CopyDirecFromCache(const std::vector<std::string>& dirs);
	bool  MakeDir(const std::string& dirURL);
	bool  IsExit (const std::string& fileURL);
	void  ReadAllFloders(std::vector<std::string>& files,std::vector<std::string>& dirs, std::string fileNameList ="");

private:
	bool  CopyByStream(const std::string& sourceFileURL,const std::string& destinationFileURL);

private:
	bool  CopyByMmap  (const std::string& sourceFileURL,const std::string& destinationFileURL);
	bool  SetMap(const std::string& sourceFileURL, const std::string& destinationFileURL, int iThreads);
	static unsigned long __stdcall  CopyThread(LPVOID  param);
	void  CopyProc(int iThread);
	UINT64 GetAllocSize(UINT64  totalSize,int totalThreads);
private:
	std::map<int, ThreadCopyInfo> mCopyMap;
private:
	std::string  _sourceURL;
	std::string  _desURL;
	std::string  _fileSuffix; 

	CLock_Mutex_t mutex;

	//std::string  _excep_file;
};