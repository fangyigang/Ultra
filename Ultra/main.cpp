#include <Windows.h>
#include <iostream>
#include "string-op.h"
#include "file-op.h"
#include "md5.h"
#include "number.h"
#include "wininet-http.h"
#include "Firewall.h"

#pragma comment(lib, "shlwapi.lib")

enum Status
{
	YES = 1,
	NO = 2
};

struct HttpCallback : public ultra::IHttpCallback
{
	void HttpHandle(int nStatus, DWORD dwDlSize, DWORD dwFileSize, const std::string& strContent)
	{
		if (nStatus != ultra::HttpStatus::kConnectFailure)
		{
			std::cout<<nStatus<<" "<<dwDlSize<<" "<<dwFileSize<<std::endl;
		}
	}
}httpCallback;


typedef std::vector< std::wstring > FileInfo;
typedef std::vector< FileInfo > FileList;

int main()
{
// 
//   	ultra::WininetHttp http;
//   	http.DownloadFile(L"http://game2.down.sandai.net/xiumo/loginupdater/mini_update/0102020042/dl_peer_id.dll", L"D:\\dl_peer_id.dll", &httpCallback, 1000);
// 	ultra::AddAppToFirewall(L"D:\\updater.exe", L"Updater.exe");
//	ultra::ReplaceFiles(L"D:\\Demo\\", L"D:\\Demo1\\");



#define WM_SETPROGRESS		WM_USER + 100
#define WM_UPDATE			WM_USER + 101
#define UPDATER_CLASS_NAME			L"Updater"
#define UPDATE_EVENT_NAME	L"XG-ANTI-UPDATE-EVENT-{4977CF33-F562-4700-A74F-6717CCB3FF0C}"
#define DLABC_EVENT_NAME	L"XG_ANTI_DLABC_EVENT-{4977CF33-F562-4700-A74F-6717CCB3FF0C}"
							


	HANDLE hDlAbcEvent =  CreateEvent(NULL, false, false, DLABC_EVENT_NAME);
 	HWND hWin = FindWindow(UPDATER_CLASS_NAME, NULL);
 	PostMessage(hWin, WM_SETPROGRESS, 0, 20);

 	HANDLE hUpdateEvent =  CreateEvent(NULL, false, false, UPDATE_EVENT_NAME);
 	PostMessage(hWin, WM_UPDATE, 0, 0);
	WaitForSingleObject(hUpdateEvent, INFINITE);
	PostMessage(hWin, WM_SETPROGRESS, 0, 50);
	WaitForSingleObject(hDlAbcEvent, INFINITE);


	std::cout<<"yes";
// 	PostMessage(hWin, WM_SETPROGRESS, 0, 20);
// 	PostMessage(hWin, WM_SETPROGRESS, 0, 100);
// 	PostMessage(hWin, WM_DESTROY, 0, 0);
	//getchar();
	return 0;
}