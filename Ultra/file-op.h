#ifndef ULTRA_FILE_OP_H
#define ULTRA_FILE_OP_H

#include <Windows.h>
#include <string>
#include <string.h>
#include <wchar.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <ShellAPI.h>
#include <io.h>

namespace ultra {

inline bool IsFileExist(const std::string& strFile)
{
	return _access(strFile.c_str(), 0) == -1 ? false : true;
}

inline bool IsFileExist(const std::wstring& wstrFile)
{
	return _waccess(wstrFile.c_str(), 0) == -1 ? false : true;
}

inline bool PathAppend(std::string& strPath, std::string& strMore)
{
	char szPath[MAX_PATH] = {0};
	strcpy(szPath, strPath.c_str());
	bool bRet = ::PathAppendA(szPath, strMore.c_str());
	strPath = szPath;
	return bRet;
}

inline bool PathAppend(std::wstring& wstrPath, std::wstring& wstrMore)
{
	wchar_t szPath[MAX_PATH] = {0};
	wcscpy(szPath, wstrPath.c_str());
	bool bRet = ::PathAppendW(szPath, wstrMore.c_str());
	wstrPath = szPath;
	return bRet;
}

inline unsigned __int64 GetDriveFreeSpace(const std::wstring& wstrDrive)
{

}

}
#endif