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
	ULARGE_INTEGER freespace;
	::GetDiskFreeSpaceEx(wstrDrive.c_str(), &freespace, NULL, NULL);
	return freespace.QuadPart;
}

inline void GetMaxFreeSpaceDrive(std::wstring* wstrDrive, unsigned __int64* nFreeSize) 
{
		DWORD dwBufLen = ::GetLogicalDriveStrings(0, NULL);
		wchar_t* pBuf = new wchar_t [dwBufLen];

		ULONGLONG nMaxFree = 0;

		if (0 != ::GetLogicalDriveStrings(dwBufLen, pBuf)) 
		{
			wchar_t* pDriveTmp = pBuf;
			DWORD i = 0;
			while (i <= dwBufLen) 
			{
				if (DRIVE_FIXED == ::GetDriveType(pDriveTmp)) 
				{
					ULONGLONG t = GetDriveFreeSpace(pDriveTmp);
					if (t > nMaxFree) 
					{
						nMaxFree = t;
						wstrDrive->assign(pDriveTmp);
					}
				}
				i = static_cast<DWORD>(wcslen(pDriveTmp)) + 1;
				pDriveTmp += i;
			}
		}
		delete[] pBuf;
		*nFreeSize = nMaxFree;
}

inline std::string GetUpperPath(const std::string strPath)
{
	int nPos = strPath.rfind("\\");
	if (nPos == std::string::npos)
	{
		return strPath;
	}
	if (nPos == (strPath.length()-1))
	{
		return GetUpperPath(strPath);
	}
	return strPath.substr(0, nPos+1);
}

inline std::wstring GetUpperPath(const std::wstring wstrPath)
{
	int nPos = wstrPath.rfind(L"\\");
	if (nPos == std::wstring::npos)
	{
		return wstrPath;
	}
	if (nPos == (wstrPath.length()-1))
	{
		return GetUpperPath(wstrPath);
	}
	return wstrPath.substr(0, nPos+1);
}

inline std::string GetModuleFileNameA()
{
	char szBuffer[MAX_PATH] = {0};
	::GetModuleFileNameA(NULL, szBuffer, MAX_PATH);
	return szBuffer;
}

inline std::wstring GetModuleFileName()
{
	wchar_t szBuffer[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szBuffer, MAX_PATH);
	return szBuffer;
}

inline std::string GetModuleFilePathA()
{
	return GetUpperPath(GetModuleFileNameA());
}

inline std::wstring GetModuleFilePath()
{
	return GetUpperPath(GetModuleFileName());
}

inline bool DeleteFileAlways(const std::wstring& wstrFileName)
{
	::SetFileAttributes(wstrFileName.c_str(), 0);
	if (::DeleteFile(wstrFileName.c_str()) == 0)
	{
		return false;
	}
	return true;
}

inline bool RecursiveRemoveDirectory(const std::wstring& wstrDirectory) {
	//pFrom and pTo need double-null terminate
	wchar_t szTmp[MAX_PATH+1];
	wcscpy(szTmp, wstrDirectory.c_str());
	szTmp[wstrDirectory.length()] = L'\0';
	szTmp[wstrDirectory.length()+1] = L'\0';
	SHFILEOPSTRUCT fileop;
	fileop.hwnd = NULL;
	fileop.wFunc = FO_DELETE;
	fileop.pFrom = szTmp;
	fileop.pTo = NULL;
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	fileop.lpszProgressTitle = NULL;

	return (::SHFileOperation(&fileop) == 0);
}

}
#endif