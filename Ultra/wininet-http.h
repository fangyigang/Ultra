#ifndef ULTRA_HTTP_H
#define ULTRA_HTTP_H

#include <Windows.h>
#include <string>
#include <WinInet.h>
#include "./number.h"
#include "./file-io.h"

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
	virtual void HttpHandle(int nStatus, int nProgress, const std::string& strContent) = 0;
};

class WininetHttp
{
public:

	WininetHttp(void) : m_bOpened(false), m_httpCallback(NULL)
	{

	}

	~WininetHttp(void)
	{
		CloseHandles();
	}

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

	bool DownloadString(const std::wstring& wstrUrl, IHttpCallback* httpCallback, unsigned nTimeout = 0)
	{
		m_httpCallback = httpCallback;
		CallHttpHandle(HttpStatus::kConnecting, -1, "");

		if (!OpenHandles(wstrUrl, nTimeout))
		{
			return false;
		}

		DWORD dwStatus;
		QueryInfoNumber(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus);
		if (dwStatus >= 400)
		{
			CallHttpHandle(HttpStatus::kConnectFailure, -1, "");
			return false;
		}

		DWORD dwContentLen;
		if (!QueryInfoNumber(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLen))
		{
			dwContentLen = 0;
		}

		DWORD dwSize = 0;
		DWORD dwRead = 0;
		char* pBuf = NULL;
		std::string strBuf;
		int nProgress;
		while (true)
		{
			if (0 == InternetQueryDataAvailable(m_hOpenUrl, &dwSize, 0, 0))
			{
				break;
			}
			if (dwSize <= 0)
			{
				break;
			}
			pBuf = new char[dwSize];
			if (InternetReadFile(m_hOpenUrl, pBuf, dwSize, &dwRead))
			{
				strBuf.append(pBuf, dwRead);
				DWORD dwDl = strBuf.length();
				if (dwContentLen > 0)
				{
					nProgress = ultra::GetProgress(strBuf.length(), dwContentLen);
				}
				else
				{
					nProgress = 0;
				}
				CallHttpHandle(HttpStatus::kDownloading, nProgress, "");
			}
			delete pBuf;
			pBuf = NULL;
		}

		if (dwContentLen > 0)
		{
			if (strBuf.length() == dwContentLen)
			{
				CallHttpHandle(HttpStatus::kSuccess, nProgress, strBuf);
			}
			else
			{
				CallHttpHandle(HttpStatus::kDownloadFailure, nProgress, strBuf);
				return false;
			}
		}
		else
		{
			CallHttpHandle(HttpStatus::kSuccess, 100, strBuf);
		}
		return true;
	}

	bool DownloadFile(const std::wstring& wstrUrl, const std::wstring& wstrFilePath, IHttpCallback* httpCallback, unsigned nTimeout = 0)
	{
		m_httpCallback = httpCallback;
		CallHttpHandle(HttpStatus::kConnecting, -1, "");
		if (!OpenHandles(wstrUrl, nTimeout))
		{
			return false;
		}

		DWORD dwStatus;
		QueryInfoNumber(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus);
		if (dwStatus >= 400)
		{
			CallHttpHandle(HttpStatus::kConnectFailure, -1, "");
			return false;
		}

		File file;
		if (!file.Create(wstrFilePath))
		{
			CallHttpHandle(HttpStatus::kCreateFileFailure, -1, "");
			return false;
		}

		DWORD dwContentLen;
		if (!QueryInfoNumber(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLen))
		{
			dwContentLen = 0;
		}

		DWORD dwSize = 0;
		DWORD dwRead = 0;
		DWORD dwReadTotal = 0;
		DWORD dwWrite = 0;
		char* pBuf = NULL;
		int nProgress = -1;
		bool bRet = true;
		while (true)
		{
			if (0 == InternetQueryDataAvailable(m_hOpenUrl, &dwSize, 0, 0))
			{
				break;
			}
			if (dwSize <= 0)
			{
				break;
			}
			pBuf = new char[dwSize];
			if (InternetReadFile(m_hOpenUrl, pBuf, dwSize, &dwRead))
			{
				if (!file.Write(pBuf, dwRead, &dwWrite))
				{
					CallHttpHandle(HttpStatus::kWriteFileFailure, nProgress, "");
					return false;
				}
				if (dwContentLen > 0)
				{
					nProgress = GetProgress(file.GetSize(), dwContentLen);
				}
				else
				{
					nProgress = 0;
				}
				CallHttpHandle(HttpStatus::kDownloading, nProgress, "");
			}
			delete[] pBuf;
		}
		if (dwContentLen > 0)
		{
			if (file.GetSize() == dwContentLen)
			{
				file.Close();
				CallHttpHandle(HttpStatus::kSuccess, nProgress, "");
			}
			else
			{
				CallHttpHandle(HttpStatus::kDownloadFailure, nProgress, "");
				return false;
			}
		}
		else
		{
			file.Close();
			CallHttpHandle(HttpStatus::kSuccess, 100, "");
		}
		return true;
	}


private:
	
	bool OpenHandles(const std::wstring& wstrUrl, unsigned nTimeout)
	{
		m_hOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (m_hOpen != NULL)
		{
			std::wstring wstrCUrl;
			CanonicalizeUrl(wstrUrl, &wstrCUrl);
			if (nTimeout > 0)
			{
				InternetSetOption(m_hOpen, INTERNET_OPTION_CONNECT_TIMEOUT, (LPVOID)&nTimeout, sizeof(nTimeout));
			}
			m_hOpenUrl = InternetOpenUrl(m_hOpen, wstrCUrl.c_str(), NULL, 0,
				INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);
		}
		if (m_hOpenUrl != NULL)
		{
			m_bOpened = true;
		}

		return m_bOpened;
	}

	void CloseHandles(void)
	{
		if (m_hOpen != NULL)
		{
			InternetCloseHandle(m_hOpen);
		}
		if (m_hOpenUrl != NULL)
		{
			InternetCloseHandle(m_hOpenUrl);
		}
	}

	bool QueryInfoNumber(DWORD dwFlags, DWORD* dwNum)
	{
		if (!(dwFlags & HTTP_QUERY_FLAG_NUMBER))
		{
			return false;
		}
		DWORD dwSize;
		return (0 != HttpQueryInfo(m_hOpenUrl, dwFlags, dwNum, &dwSize, NULL));
	}

	void CallHttpHandle(int nStatus, int nProgress, const std::string& strContent)
	{
		if (m_httpCallback != NULL)
		{
			m_httpCallback->HttpHandle(nStatus, nProgress, strContent);
		}
	}

private:

	HINTERNET m_hOpen;
	HINTERNET m_hOpenUrl;
	bool m_bOpened;
	IHttpCallback* m_httpCallback;
};


class WininetDlList
{
public:
	WininetDlList(void)
	{

	}
	~WininetDlList(void)
	{

	}

};


}

#endif