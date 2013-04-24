#include <iostream>
#include "string-op.h"
#include "file-op.h"
int main()
{
	std::wcout<<ultra::ConvertToWString(1239999.99);
	getchar();
	return 0;
}