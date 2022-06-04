#define _CRT_SECURE_NO_WARNINGS

#include<windows.h>
#include<iostream>


int main(int argc, char* argv[])
{
	WCHAR srcFilename[CHAR_MAX];
	WCHAR destFilename[CHAR_MAX];

	std::cout << "Please eneter src filename\n";
	char src[CHAR_MAX * 2];
	std::cin >> src;
	
	char dest[CHAR_MAX * 2];
	std::cout << "Please eneter dest filename\n";
	std::cin >> dest;


	mbstowcs(srcFilename, src, sizeof(srcFilename));

	mbstowcs(destFilename, dest,sizeof(destFilename));

	//Open existing file
	HANDLE handle = CreateFile(srcFilename,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (!handle)
	{
		printf("File doesnt exist\n");
		return EXIT_FAILURE;
	}

	
	DWORD fileSize = GetFileSize(handle, NULL);
	if (fileSize == INVALID_FILE_SIZE)
	{
		printf("FIle size is not valid\n");
		return EXIT_FAILURE;
	}

	//Try to open dest file(the task is that this file doesnt exist but to create dest)
	HANDLE destHandle = CreateFile(destFilename,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (destHandle != INVALID_HANDLE_VALUE)
	{
		printf("File wor write already exist\n");
		return EXIT_FAILURE;
	}

	//now we create 
	destHandle = CreateFile(destFilename,
		GENERIC_WRITE,
		0,
		0,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (destHandle == INVALID_HANDLE_VALUE)
	{
		printf("FIle wasnt created\n");
	}

	DWORD totalBytes = 0;
	DWORD bytesRead = 0;
	DWORD bytesWritten = 0;
	PVOID buff = malloc(fileSize/10);

	while (totalBytes != fileSize)
	{
		BOOL flag = ReadFile(handle, buff, fileSize / 10, &bytesRead, NULL);
		if (!flag)
		{
			printf("File wasnt read properly\n");
			return EXIT_FAILURE;
		}

		std::cout << (wchar_t*)buff << std::endl;

		totalBytes += bytesRead;
		
		flag = WriteFile(destHandle,
			buff,
			fileSize / 10,
			&bytesWritten,
			NULL);
		if (!flag)
		{
			printf("File wasnt written to \n");

			return EXIT_FAILURE;
		}
	}

	
	if(handle)
	CloseHandle(handle);

	if(destHandle)
	CloseHandle(destHandle);
	
	free(buff);

	
	return EXIT_SUCCESS;
}