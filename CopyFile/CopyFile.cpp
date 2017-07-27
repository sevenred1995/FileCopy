#include "CopyFile.h"
#include <iostream>  
#include <fstream>  
#include <cstring>  

#if defined(_WIN32)
#include <Windows.h>
#include <WinBase.h>
#include <direct.h>
#include <io.h> 
#include <shlobj.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include <iostream>
#include <time.h>
#include "FileMapping.h"
#include <conio.h>
#else
#include <dirent.h>  
#include <unistd.h>  
#include <sys/stat.h>  
#include <sys/types.h>  
#include <pwd.h>  
#endif


#define BLOCK_SIZE 1024*256

using namespace std;

unsigned int CusCopyFile::CopyAllFile(const std::string& sourceFolderURL,const std::string& desFolderURL)
{
	this->_sourceURL = sourceFolderURL;
	this->_desURL    = desFolderURL;
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	ReadAllFloders(files,dirs);
	if (!CopyDirecFromCache(dirs))//des
		return CREATE_FLODER_FAILED;
	return CopyFileFromCache(files); 
	//return true;
}
unsigned int CusCopyFile::CopyOneFile(
	const std::string& sourceFileURL, const std::string& desFolderURL
)
{
	this->_sourceURL = sourceFileURL;
	this->_desURL = desFolderURL;
	int n = 0;
	string fileName;
#if defined(_WIN32)
	while (sourceFileURL.find('\\', n) != std::string::npos)
		n = sourceFileURL.find('\\', n) + 1;
#else

#endif
	if (n == 0)
	{
		printf("src path error\n");
		return SRC_FILE_ERROR;
	}
	fileName = sourceFileURL.substr(n - 1, sourceFileURL.size());//get the file name
	MakeDir(desFolderURL);//create desURL--处理返回值
	std::string destionFileURL = desFolderURL + fileName;
	return ToolsCopy(sourceFileURL, destionFileURL);
}

bool CusCopyFile::CopyDirecFromCache(const std::vector<std::string>& dirs)
{
	if (dirs.empty())//no sub directory
		return MakeDir(this->_desURL);
	//copy dir
	for (int i = 0; i < dirs.size(); i++)
	{
		if (!MakeDir(this->_desURL + dirs.at(i)))//link desFolder and source folder
			continue;										 //return false;
	}
	return true;
}

//批量拷贝
unsigned CusCopyFile::CopyFileFromCache(const std::vector<std::string>& files)
{
	cout << "copying..." << endl;
	clock_t start = clock();
	for (int i=0; i < files.size(); i++)
	{
		std::string srcFileURL = this->_sourceURL + files.at(i);
	    std::string desFileURL = this->_desURL + files.at(i);
		unsigned int res = ToolsCopy(srcFileURL, desFileURL);
		if (COPY_FILE_EXISTED == res)
		{
			printf("%s has existed!\n",desFileURL.c_str());
			return COPY_FILE_EXISTED;
		}
		if (COPY_FILE_SUCCESS == res)
			continue;
		return COPY_FILE_FAILED;
	}
	clock_t end = clock();
	printf("copy all file need time:%ds\n", (end - start) / CLOCKS_PER_SEC);
	return COPY_FILE_SUCCESS;
}
unsigned int CusCopyFile::ToolsCopy(
    const std::string & srcFileURL,const std::string & desFileURL)
{
	if (!IsExit(srcFileURL))
		return SRC_FILE_ERROR;
	//file existed,don't handle this file
	if (IsExit(desFileURL)) {
		return COPY_FILE_EXISTED;//file exited,user maybe can selected to continue cover it;
		//continue
	}
	int size = FileMapping::GetSize(srcFileURL);
	if (!CopyByMmap(srcFileURL, desFileURL))
	{
		printf("%s copy failed\n",desFileURL.c_str());
		return COPY_FILE_FAILED;
		//continue;
	}
	//cout << srcFileURL << "copy success" << endl;
	printf("%s copy success\n", srcFileURL.c_str());
	return COPY_FILE_SUCCESS;
}
bool CusCopyFile::MakeDir(const std::string& url)
{
	std::string folderBuilder;
	std::string sub;
	sub.reserve(url.size());
#if defined(_WIN32)
 	for (auto it = url.begin(); it != url.end(); ++it)
	{
		const char c = *it;
		sub.push_back(c);
		if (c != '\\' && it != url.end() - 1)continue;
		folderBuilder.append(sub);
		if (0 == ::_access(folderBuilder.c_str(), 0))//file have existed
		{
			sub.clear();
			continue;
		}
		if (0 != ::_mkdir(folderBuilder.c_str()))//create failed
			return false;
	}
#else
	
#endif
	return true;
}

bool CusCopyFile::IsExit(const std::string & url)
{
	struct stat fileStat;
	if ((stat(url.c_str(), &fileStat) == 0)&&!(fileStat.st_mode&_S_IFDIR))
	{
		return true;
	}
	return false;
}
void
CusCopyFile::ReadAllFloders(
	std::vector<std::string>& files, 
	std::vector<std::string>& dirs, 
	std::string fileNameList)
{
#if defined(_WIN32)
	long _hFile = 0;
	struct _finddata_t fileInfo;
	string p, temp;
	string src = this->_sourceURL;
	if (fileNameList.empty())
		p = src.append("\\*");// .append(this->_fileSuffix);
	else
		p = src.append(fileNameList).append("\\*");// .append(this->_fileSuffix);

	if ((_hFile = _findfirst(p.c_str(), &fileInfo)) != -1)
	{
		do
		{
			if ((fileInfo.attrib &  _A_SUBDIR)) { 
				if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
				{
					dirs.push_back(temp.assign(fileNameList).append("\\").append(fileInfo.name));//process all file
					ReadAllFloders(files, dirs, temp.assign(fileNameList).append("\\").append(fileInfo.name));
				}
			}
			else
				files.push_back(temp.assign(fileNameList).append("\\").append(fileInfo.name));//process all dir
				
		} while (_findnext(_hFile, &fileInfo) == 0);
	}
	else
	{
		printf("There is no this directory,MayBe you will copy single file!\n");
		CopyOneFile(this->_sourceURL,this->_desURL);
	}
	_findclose(_hFile);
#endif
}

bool CusCopyFile::CopyByStream(const std::string& sourceFileURL,const std::string& desFileURL)
{
	std::ifstream _in;
	_in.open(sourceFileURL, ios::binary);
	if (!_in) {
		printf("open src file : %s failed\n", sourceFileURL.c_str());
		return false;
	}
	std::ofstream _out;
	_out.open(desFileURL, ios::binary);
	if (!_out)
	{
		printf("create new file : %s failed\n", desFileURL.c_str());
		_in.close();
		return false;
	}
	//_out << _in.rdbuf();
	_in.seekg(0, ios::end);
	long length = _in.tellg();
	_in.seekg(0, ios::beg);
	const int _buffer_size = 1024*256;
	char buffer[_buffer_size];
	while (1) {
		memset(buffer, 0, sizeof(buffer));
		_in.read(buffer, _buffer_size);
		_out.write(buffer, sizeof(buffer));
		int curPos = _in.tellg();
		if(-1==curPos)
			break;
	}
	_out.close();
	_in.close();
	return true;
}
bool CusCopyFile::CopyByMmap(const std::string& sourceFileURL,const std::string& desFileURL)
{
	
	int iThreads=1;
	if (false == SetMap(sourceFileURL, desFileURL, iThreads))
		return false;
	HANDLE* handles=new HANDLE[iThreads+1];
	for (int i = 0; i < iThreads+1; i++)
	{
		ThreadParam *pThreadParam = new ThreadParam();
		memset(pThreadParam, 0, sizeof(ThreadParam));
		pThreadParam->pObj = this;
		pThreadParam->threadId = i;
		pThreadParam->iThreads = iThreads;
		HANDLE hThread = CreateThread(NULL,NULL, CopyThread, pThreadParam, NULL, NULL);
		handles[i] = hThread;
	}
	WaitForMultipleObjects(iThreads, handles, true, INFINITE);
	printf("\n=======all Thread run end===============\n");
	for (int i = 0; i < iThreads+1; i++)
    	CloseHandle(handles[i]);
	delete[] handles;
	return true;
}

bool CusCopyFile::SetMap(const std::string & sourceFileURL, const std::string & desFileURL, int iThreads)
{
	UINT64 size = FileMapping::GetSize(sourceFileURL);
	UINT64 threadCopySize = GetAllocSize(size,iThreads);
	mCopyMap.clear();
	for (int i=0;i<iThreads;i++)
	{
		ThreadCopyInfo info;
		memset(&info, 0, sizeof(ThreadCopyInfo));
		info.threadId = i;
		info.pSrcMapping = FileMapping::Open(sourceFileURL, 0);
		if (NULL == info.pSrcMapping)
			return false;
		info.pDesMapping = FileMapping::Open(desFileURL, size);
		if (NULL == info.pDesMapping)
			return false;
		info.startPos = i*threadCopySize;
		info.offset = info.startPos;
		if (i==iThreads-1)
			info.endPos = size;
		else
			info.endPos = (i + 1)*threadCopySize;
		mCopyMap[i] = info;
	}
	return true;
}

void CusCopyFile::CopyProc(int iThread)
{
	std::map<int, ThreadCopyInfo>::iterator it = mCopyMap.find(iThread);
	if (it == mCopyMap.end())
		return;
	ThreadCopyInfo info = it->second;
	BYTE buf[BLOCK_SIZE];
	while (true)
	{
		memset(buf, 0, BLOCK_SIZE);
		UINT64 block = BLOCK_SIZE;
		if (info.offset + BLOCK_SIZE > info.endPos)
			block = info.endPos - info.offset;
		if(false==FileMapping::Read(info.pSrcMapping,info.offset,block,buf))
			break;//interrupt
		if(false==FileMapping::Write(info.pDesMapping,info.offset,block,buf))
			break;//interrupt 
		info.offset = info.offset + block;
		it->second.offset = info.offset;
		if (info.offset >= info.endPos)
		{
			CloseHandle(info.pSrcMapping);
			CloseHandle(info.pDesMapping);
			break;
		}
	}
}

void CusCopyFile::ListenProc(int iThreads)
{
    std::map<int, ThreadCopyInfo>::iterator it;
	while (true)
	{
		//listen every progress of the thread;
		for (int i = 0; i < iThreads; i++)
		{
			it=mCopyMap.find(i);
			if (it == mCopyMap.end())
				return;
			Sleep(1);
			if (it->second.offset != it->second.endPos)
			{
				float progress = (float)(it->second.offset - it->second.startPos) / (it->second.endPos - it->second.startPos);
				printf("\rThread%d:%0.2f%%", i, progress * 100);
				fflush(stdout);
			}
		}
	}
}

unsigned long CusCopyFile::CopyThread(LPVOID param)
{
	ThreadParam* pThreadParam = (ThreadParam*)param;

	if (pThreadParam->threadId != pThreadParam->iThreads)
	{
		pThreadParam->pObj->CopyProc(pThreadParam->threadId);
	}
	else
	{
		pThreadParam->pObj->ListenProc(pThreadParam->iThreads);
	}
	if (NULL != pThreadParam)
	{
		delete pThreadParam;
	}
	return 0;
}

UINT64  CusCopyFile::GetAllocSize(UINT64  totalSize, int totalThreads)
{
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);
	UINT64 allocSize = sysInfo.dwAllocationGranularity;
	UINT64 allocSizes;
	if (0 == totalSize % allocSize)
	{
		allocSizes = totalSize / allocSize;
	}
	else 
	{
		allocSizes = totalSize / allocSize + 1;
	} 
	UINT64 threadAllocSizes = allocSizes / totalThreads;
	UINT64 threadAllocSize = threadAllocSizes * allocSize;
	return threadAllocSize;
}