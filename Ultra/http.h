#ifndef ULTRA_HTTP_H
#define ULTRA_HTTP_H

#include <Windows.h>
#include <string>
#include <WinInet.h>

#pragma comment(lib, "WinInet.lib")

namespace ultra{

enum HttpStatus
{
	kSuccess           = 0,
	kConnecting        = 1,
	kDownloading       = 2,
	kConnectFailure    = 3,
	kDownloadFailure   = 4,
	kCreateFileFailure = 5,
	kWriteFileFailure  = 6,
};

struct IHttpCallback
{
	virtual void HttpHandle(int nStatus, int nProgress, const std::string& content) = 0;
};

class Http
{
public:

	Http(void);
	~Http(void);

	static bool CanonicalizeUrl(const std::wstring& strSrc, std::wstring* strDest)
	{
		wchar_t ch;
		DWORD dwLen = 1;
		bool bRet = (0 != InternetCanonicalizeUrl(strSrc.c_str(), &ch, &dwLen, ICU_BROWSER_MODE));
		if (!bRet)
		{
			wchar_t* pBuf = new wchar_t[dwLen];
			bRet = (0 != InternetCanonicalizeUrl(strSrc.c_str(), pBuf, &dwLen, ICU_BROWSER_MODE));
			strDest->assign(pBuf, dwLen);
			delete[] pBuf;
			pBuf = NULL;
		}
		return bRet;
	}

	bool DownloadString(const std::wstring& wstrUrl, IHttpCallback* httpCallback, unsigned nTimeout = 0);
	bool DownloadFile(const std::wstring& wstrUrl, const std::wstring& wstrFilePath, IHttpCallback* httpCallback, unsigned nTimeout = 0);


private:
	bool OpenHandles(const std::wstring& wstrUrl, unsigned nTimeout);
	bool CloseHandles(void);


};

}

#endif