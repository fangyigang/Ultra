#include <iostream>
#include "string-op.h"
#include "file-op.h"
#include "md5.h"
#include "number.h"
#include "wininet-http.h"

enum Status
{
	YES = 1,
	NO = 2
};

struct HttpCallback : public ultra::IHttpCallback
{
	void HttpHandle(int nStatus, int nProgress, const std::string& strContent)
	{
		std::cout<<nStatus<<" "<<nProgress<<" "<<strContent<<std::endl;
	}
}httpCallback;

struct A
{
	std::string a;
	std::string b;
	std::string c;
};

struct B
{
	std::string a[3];
};

int main()
{

// 	ultra::WininetHttp http;
// 	http.DownloadFile(L"http://game2.down.sandai.net/xiumo/loginupdater/mini_update/0102020042/dl_peer_id.dll", L"D:\\dl_peer_id.dll", &httpCallback, 1000);
	A a;
	B* b;
	a.a = "aaa";
	a.b = "bbb";
	a.c = "ccc";
	b = (B*)a;
	for (int i = 0; i < 3; i++)
	{
		std::cout<<b->a[i]<<" ";
	}
	getchar();
	return 0;
}