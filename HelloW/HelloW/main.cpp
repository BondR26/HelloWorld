#include<iostream>
#include<windows.h>

int main()
{
	std::cout << "Hello git"<<std::endl;
	__asm
	{
		mov eax, ebx;
	}

	HANDLE handle = CreateFile(L"1.txt",
		GENERIC_ALL,
		0,
		0,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0);

	CloseHandle(handle);

	return EXIT_SUCCESS;
}