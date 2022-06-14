#define _CRT_SECURE_NO_WARNINGS

#include<Windows.h>
#include<iostream>
#include<psapi.h>
#include<string>
/*Домашка, которую можно сделать до следующего занятия:
Напишите программу, которая через переменную окружения принимает время жизни потока в секундах. 
Запускает поток который выводит каждые 5 сек текущее время и завершается через заданное количество времени
*/

#define  TO_SECONDS(x)  (x*1000)
#define  FIVE_SEC       5000

class HandleWrapper
{
public:
	explicit HandleWrapper(HANDLE handle):m_Handle(handle){}
	~HandleWrapper()
	{
		CloseHandle(m_Handle);
	}

	HANDLE operator=(HANDLE handle)
	{
		if (m_Handle)
		{
			std::cout << "Beware prevoius handle was close\n";
			CloseHandle(m_Handle);
		}

		m_Handle = handle;
	}

	bool operator == (HANDLE handle)
	{
		return this->m_Handle == handle;
	}

	HANDLE GetHandle()
	{
		return m_Handle;
	}

private:

	HANDLE m_Handle = NULL;
};


BOOL KillProc(const std::string& filename)
{
	DWORD allProc[1024], bytesNeeded, cProcesses;

	LPSTR buffer = (LPSTR)malloc(1024);

	unsigned int i;

	bool flag = false;

	HANDLE handle = NULL;

	if (!EnumProcesses(allProc, sizeof(allProc), &bytesNeeded))
	{
		return 1;
	}

	for (int i = 0; i < bytesNeeded/sizeof(DWORD); i++)
	{
		handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, allProc[i]);

		GetProcessImageFileNameA(handle, buffer, 1024);

		if (strcmp(buffer, filename.c_str())==NULL)
		{
			printf("The process was found\n");
			flag = true;
			break;
		}

		CloseHandle(handle);
	}
	

	if (flag)
		TerminateProcess(handle, NULL);

	if(handle != INVALID_HANDLE_VALUE)
		WaitForSingleObject(handle, NULL);

	if (handle != INVALID_HANDLE_VALUE)
		CloseHandle(handle);
	
	free(buffer);
}



DWORD WINAPI MyThread(PVOID context)
{
	std::string timestr = static_cast<char*>(context);

	std::cout << "Time given to us " << timestr <<std::endl;

	int time = TO_SECONDS(std::stoi(timestr));

	while (true)
	{
		std::cout << "THREAD threat\n";\

		Sleep(FIVE_SEC);
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	std::string context;
	if (argv[1])
	{
		context = argv[1];

		//time given to us in seconds
		int time = std::stoi(context);

		HandleWrapper threadHandle(CreateThread(0, 0, MyThread, static_cast<PVOID>(const_cast<char*>(&context.c_str()[0])), 0, 0));

		WaitForSingleObject(threadHandle.GetHandle(), TO_SECONDS(time));
	}

	std::string filename;
	std::cout << "Please enter the name of the proces to kill\n";
	std::cin >> filename;
	KillProc(filename);

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