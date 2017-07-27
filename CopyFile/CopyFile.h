#pragma once
#include <string>
#include <vector>
#include <map>

#ifdef  _WIN32                                                         //Windows
#include <Windows.h>
#define CLock_Mutex_t                 HANDLE
#define CLock_Mutex_Init(_mutex)      (_mutex = CreateSemaphore(NULL,1,1,NULL))
#define CLock_Mutex_Lock(_mutex)      (WaitForSingleObject(_mutex, INFINITE))
#define CLock_Mutex_UnLock(_mutex)    (ReleaseSemaphore(_mutex,1,NULL))
#define CLock_Mutex_Destroy(_mutex)   (CloseHandle(_mutex))
#endif

#define CREATE_FLODER_FAILED  1000
#define CREATE_FLODER_SUCCESS 1001
#define COPY_FILE_FAILED      1002
#define COPY_FILE_SUCCESS     1003
#define COPY_FILE_EXISTED     1004
#define SRC_FILE_ERROR        1005
#define COPY_FUNC_ERROR       1006


//source
//des Folder /
class CusCopyFile {
public:
	CusCopyFile() :_fileSuffix("") { CLock_Mutex_Init(mutex); }
	void SetSuffix(std::string suffix) { _fileSuffix = suffix; }
public:

public:
	unsigned int CopyAllFile(const std::string& sourceFolderURL,const std::string& desFolderURL);
  //  unsigned int CopyAllFileWithFilter(const std::string& sourceFolderURL, const std::string& desFolderURL, "");

	unsigned int CopyOneFile(const std::string& sourceFileURL, const std::string& desFolderURL);
private:
	unsigned int CopyFileFromCache(const std::vector<std::string>& files);
	unsigned int ToolsCopy(const std::string& srcFileURL,const std::string& desFileURL);
	bool  CopyDirecFromCache(const std::vector<std::string>& dirs);
	bool  MakeDir(const std::string& dirURL);
	bool  IsExit (const std::string& fileURL);
	void  ReadAllFloders(std::vector<std::string>& files,std::vector<std::string>& dirs, std::string fileNameList ="");

private:
	bool  CopyByStream(const std::string& sourceFileURL,const std::string& desFileURL);
private:
	bool  CopyByMmap  (const std::string& sourceFileURL,const std::string& desFileURL);
	bool  SetMap(const std::string& sourceFileURL, const std::string& desFileURL, int iThreads);
	static unsigned long __stdcall  CopyThread(LPVOID  param);
	void   CopyProc(int iThread);
	void   ListenProc(int iThreads);
	UINT64 GetAllocSize(UINT64  totalSize,int totalThreads);
private:
	struct ThreadCopyInfo  
	{
		int   threadId;
		void* pSrcMapping;  //mapping object of the source file
		void* pDesMapping;  // mapping object of the des file
		UINT64 startPos;
		UINT64 endPos;
		UINT64 offset;     //currnt thread's offset
	};
	struct ThreadParam
	{
		int threadId;
		int iThreads;
		CusCopyFile* pObj;
	};	
    std::map<int, ThreadCopyInfo> mCopyMap;
private:
	std::string  _sourceURL;
	std::string  _desURL;
	std::string  _fileSuffix; 

	CLock_Mutex_t mutex;

	//std::string  _excep_file;
};