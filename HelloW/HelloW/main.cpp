#define _CRT_SECURE_NO_WARNINGS

#include<Windows.h>
#include<iostream>
#include<psapi.h>
#include<string>
/*Домашка, которую можно сделать до следующего занятия:
Напишите программу, которая через переменную окружения принимает время жизни потока в секундах. 
Запускает поток который выводит каждые 5 сек текущее время и завершается через заданное количество времени
*/

inline  int ToSeconds(int x)
{
	return x * 1000;
}
#define  FIVE_SEC       5000
#define  MAX_SIZE       1024

class HandleWrapper
{
public:
	explicit HandleWrapper(HANDLE handle)
	{
		if (handle)
			m_Handle = handle;
		else
			throw(1);
	}

	~HandleWrapper()
	{
		if(m_Handle)
			CloseHandle(m_Handle);
	}

	HANDLE operator=(HANDLE handle)
	{
		if (m_Handle)
			CloseHandle(m_Handle);

		m_Handle = handle;
	}

	bool operator == (HANDLE handle)
	{
		return m_Handle == handle;
	}

	bool operator != (HANDLE handle)
	{
		return !(m_Handle == handle);
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
	DWORD allProc[MAX_SIZE], bytesNeeded, cProcesses;

	LPSTR buffer[MAX_SIZE];

	unsigned int i;

	bool flag = false;

	HANDLE handle = NULL;

	if (!EnumProcesses(allProc, sizeof(allProc), &bytesNeeded))
	{
		return 1;
	}

	for (int i = 0; i < bytesNeeded / sizeof(DWORD); i++)
	{
		handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, allProc[i]);
		if (handle != INVALID_HANDLE_VALUE)
		{
			GetProcessImageFileNameA(handle, buffer[0], MAX_SIZE);

			if (strcmp(buffer[0], filename.c_str()) == NULL)
			{
				std::cout << ("The process was found\n");
				flag = true;
				break;
			}
		}
		CloseHandle(handle);
	}

	if (flag)
	{
		TerminateProcess(handle, NULL);
	}
	if (handle != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(handle, NULL);
	}
	if (handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
	}
}



DWORD WINAPI MyThread(PVOID context)
{
	std::string timestr = static_cast<char*>(context);

	std::cout << "Time given to us " << timestr <<std::endl;

	int time = ToSeconds(std::stoi(timestr));

	while (true)
	{
		std::cout << "THREAD threat\n";

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
		long long time = 0;
		//time given to us in seconds
		try
		{
			 time = std::stoi(context);
		}
		catch (std::invalid_argument const& ex)
		{
			std::cout << "std::invalid_argument::what(): " << ex.what() << '\n';
		}
	
		HandleWrapper threadHandle(CreateThread(0, 0, MyThread, static_cast<PVOID>(const_cast<char*>(&context.c_str()[0])), 0, 0));

		WaitForSingleObject(threadHandle.GetHandle(), ToSeconds(time));
	}

	std::string filename;
	std::cout << "Please enter the name of the proces to kill\n";
	std::cin >> filename;
	KillProc(filename);

	std::cout << "Please enetergit src filename\n";
	char src[MAX_SIZE];
	std::cin >> src;
	
	char dest[MAX_SIZE];
	std::cout << "Please eneter dest filename\n";
	std::cin >> dest;
	//Open existing file
	HANDLE handle = CreateFileA(src,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (!handle)
	{
		std::cout <<("File doesnt exist\n");
		return EXIT_FAILURE;
	}

	
	DWORD fileSize = GetFileSize(handle, NULL);
	if (fileSize == INVALID_FILE_SIZE)
	{
		std::cout <<("FIle size is not valid\n");
		return EXIT_FAILURE;
	}

	//Try to open dest file(the task is that this file doesnt exist but to create dest)
	HANDLE destHandle = CreateFileA(dest,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (destHandle != INVALID_HANDLE_VALUE)
	{
		std::cout <<("File wor write already exist\n");
		return EXIT_FAILURE;
	}

	//now we create 
	destHandle = CreateFileA(dest,
		GENERIC_WRITE,
		0,
		0,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (destHandle == INVALID_HANDLE_VALUE)
	{
		std::cout <<("FIle wasnt created\n");
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
			std::cout <<("File wasnt read properly\n");
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
			std::cout <<("File wasnt written to \n");

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