#include "CopyFile.h"
#include <iostream>  
#include <fstream>  
#include <cstring>  

#if defined(_WIN32)
#include<Windows.h>
#include<WinBase.h>
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

using namespace std;
#define BLOCK_SIZE 1024*256
bool CusCopyFile::CopyAllFile(const std::string& sourceFolderURL,const std::string& destinationFolderURL)
{
	this->_sourceURL = sourceFolderURL;
	this->_desURL    = destinationFolderURL;
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	ReadAllFloders(files,dirs);
	CopyDirecFromCache(dirs);
	CopyFileFromCache(files); 
	return true;
}
bool CusCopyFile::CopyOneFile(const std::string& sourceFileURL, const std::string& destinationFolderURL)
{
	this->_sourceURL = sourceFileURL;
	this->_desURL = destinationFolderURL;
	std::string src = sourceFileURL;
	int n = 0;
	string fileName;
#if defined(_WIN32)
	while (src.find('\\', n) != std::string::npos)
		n = src.find('\\', n) + 1;
#else
	n = 0;
	while (src.find('/', n) != std::string::npos)
		n = src.find('/', n) + 1;

#endif
	if (n == 0)
	{
		std::cout << "src path error" << endl;
		return false;
	}
	fileName = src.substr(n - 1, src.size());//get the file name
	MakeDir(destinationFolderURL);//create destinationURL
	std::string destionFileURL = destinationFolderURL + fileName;
	if (IsExit(destionFileURL)) {
		cout << "file has exited.." << endl;
		return false;
	}
	cout << "copying..." << endl;
	clock_t start = clock();
	int size = FileMapping::GetSize(sourceFileURL);
	if (size < 1024 * 256)
	{
		if (CopyByStream(sourceFileURL, destionFileURL))
			cout << sourceFileURL << " copy success" << endl;
		else
		{
			cout << destionFileURL << "copy failed" << endl;
			return false;
		}
	}
	else
	{
		if (CopyByMmap(sourceFileURL, destionFileURL))
			cout << sourceFileURL << " copy success" << endl;
		else
		{
			cout << sourceFileURL << "copy failed" << endl;
			return false;
		}
	}
	clock_t end = clock();
	cout << "need time："<< (end - start)/ CLOCKS_PER_SEC <<"s"<< endl;
	return true;
}

bool CusCopyFile::CopyDirecFromCache(const std::vector<std::string>& dirs)
{
	if (dirs.empty())
		return MakeDir(this->_desURL);
	//copy dir
	for (int i = 0; i < dirs.size(); i++)
	{
		if (!MakeDir(this->_desURL + dirs.at(i)))//link desFolder and source folder
			continue;										 //return false;
	}
	return true;
}

void CusCopyFile::CopyFileFromCache(const std::vector<std::string>& files)
{
	cout << "copying..." << endl;
	clock_t start = clock();
	for (int i=0; i < files.size(); i++)
	{
		std::string srcFileURL = this->_sourceURL + files.at(i);
		std::string desFileURL = this->_desURL    + files.at(i);
		if (IsExit(desFileURL)) {
			cout << "file has exited.." << endl; continue;
		}
		int size = FileMapping::GetSize(srcFileURL);
		if (size < 1024 * 256)
		{
			if(CopyByStream(srcFileURL,desFileURL))
				cout << srcFileURL << " copy success" << endl;
			else
			{
				cout << srcFileURL << "copy failed" << endl;
				continue;
			}
		}
		else
		{
			if (CopyByMmap(srcFileURL, desFileURL))
				cout << srcFileURL << " copy success" << endl;
			else
			{
				cout << srcFileURL << "copy failed" << endl;
				continue;
			}
		}
	}
	clock_t end = clock();
	cout << "copy all file need time：" << (end - start) / CLOCKS_PER_SEC <<"s"<< endl;
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
		if (0 == ::_access(folderBuilder.c_str(), 0))
		{
			sub.clear();
			continue;
		}
		if (0 != ::_mkdir(folderBuilder.c_str()))
			return false;
	}
#else
	
#endif
	return true;
}

bool CusCopyFile::IsExit(const std::string & url)
{
	struct stat fileStat;
	if ((stat(url.c_str(), &fileStat) == 0))
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
	///TODO判断文件大小
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
		cout << "There is No This Directional,MayBe you will copy single file!" << endl;
	}
	_findclose(_hFile);
#endif
}

bool CusCopyFile::CopyByStream(const std::string& sourceFileURL,const std::string& destinationFileURL)
{
	std::ifstream _in;
	_in.open(sourceFileURL, ios::binary);
	if (!_in) {
		std::cout << "open src file : " << sourceFileURL << " failed" << std::endl;
		return false;
	}
	std::ofstream _out;
	_out.open(destinationFileURL, ios::binary);
	if (!_out)
	{
		std::cout << "create new file : " << destinationFileURL << " failed" << std::endl;
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
bool CusCopyFile::CopyByMmap(const std::string& sourceFileURL,const std::string& destinationFileURL)
{
	int iThreads=1;
	if (false == SetMap(sourceFileURL, destinationFileURL, iThreads))
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
	//handles[iThreads]=CreateThread(NULL,NULL,ListenThread,)
	WaitForMultipleObjects(iThreads, handles, true, INFINITE);
	cout << "=======all Thread " << "run end===============" << endl;
	for (int i = 0; i < iThreads+1; i++)
    	CloseHandle(handles[i]);
	delete[] handles;
	return true;
}

bool CusCopyFile::SetMap(const std::string & sourceFileURL, const std::string & destinationFileURL, int iThreads)
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
		info.pDesMapping = FileMapping::Open(destinationFileURL, size);
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
	clock_t start = clock();
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
	clock_t end = clock();
	cout <<endl<< "Thread " << iThread << " run end,times:" << (end - start)/ CLOCKS_PER_SEC << endl;
}

void CusCopyFile::ListenProc(int iThreads)
{
	while (true)
	{
		//listen every progress of the thread;
		for (int i = 0; i < iThreads; i++)
		{
			std::map<int, ThreadCopyInfo>::iterator it = mCopyMap.find(i);
			if (it == mCopyMap.end())
				return;
			float progress = (float)(it->second.offset - it->second.startPos) / (it->second.endPos - it->second.startPos);
			printf("\rThread%d:%0.2f%%", i,progress*100);
			fflush(stdout);
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