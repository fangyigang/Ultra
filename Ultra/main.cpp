#include <iostream>
#include "string-op.h"
#include "file-op.h"
#include "md5.h"

enum Status
{
	YES = 1,
	NO = 2
};
int main()
{
	std::wstring wstrDrive;
	unsigned __int64 nFreeSize;
	ultra::GetMaxFreeSpaceDrive(&wstrDrive, &nFreeSize);
// 	std::wcout<<wstrDrive;
// 	std::wcout<<nFreeSize;
// 	std::cout<<ultra::MD5File(L"d:\\a.ini");
	std::cout<<Status::YES;
	getchar();
	return 0;
}