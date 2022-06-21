#define _CRT_SECURE_NO_WARNINGS

#include<Windows.h>
#include<iostream>
#include<psapi.h>
#include<string>
/*Домашка, которую можно сделать до следующего занятия:
Напишите программу, которая через переменную окружения принимает время жизни потока в секундах. 
Запускает поток который выводит каждые 5 сек текущее время и завершается через заданное количество времени
*/

/*HW
Есть функция UnloadTheShip которая эмитирует разгрузку корабля и выводит информацию о колличестве разгруженного груза (корабль вмещает 1000 ед), например:
Unloaded goods amount: 2
Unloaded goods amount: 3
...
У нас есть причал, который может принять на разгрузку не более 3х кораблей. Вам необходимо реализовать логику разгрузки 50 кораблей (1 поток обрабатывает 1 корабль)
*/

#define             RELEASE_SHIP         1
#define             POSSIBLE_TO_EXCEPT   3
#define             HARBOUR_CAPACITY     60
HANDLE              g_Semaphore          = NULL;
CRITICAL_SECTION    g_CritSection        = {};

DWORD WINAPI UnloadShip(void* context)
{
    EnterCriticalSection(&g_CritSection);

    WaitForSingleObject(g_Semaphore, INFINITE);
    
    int* cargoCount = static_cast<int*>(context);

    std::cout << "Unloaded cargo = " << *cargoCount <<std::endl;

    ReleaseSemaphore(g_Semaphore,RELEASE_SHIP, NULL);

    LeaveCriticalSection(&g_CritSection);

    return *cargoCount;
}

int main()
{
    int    context            = 0;
    HANDLE threads[HARBOUR_CAPACITY] = {};

    srand(time(0));
    g_Semaphore = CreateSemaphore(NULL, POSSIBLE_TO_EXCEPT, POSSIBLE_TO_EXCEPT, NULL);
    InitializeCriticalSection(&g_CritSection);
    
    for (int i = 0; i < HARBOUR_CAPACITY/3; i++)
    {                   
        context         = rand();
        threads[i]      = CreateThread(0, 0, UnloadShip, static_cast<void*>(&context), CREATE_SUSPENDED, 0);
        context         = rand();
        threads[i+20]   = CreateThread(0, 0, UnloadShip, static_cast<void*>(&context), CREATE_SUSPENDED, 0);
        context         = rand();
        threads[i + 40] = CreateThread(0, 0, UnloadShip, static_cast<void*>(&context), CREATE_SUSPENDED, 0);

    }


    for (int i = 0; i < HARBOUR_CAPACITY; i++)
    {
        ResumeThread(threads[i]);
    }

    WaitForMultipleObjects(POSSIBLE_TO_EXCEPT, threads, TRUE, INFINITE);

    CloseHandle(g_Semaphore);
    for (int i = 0; i < HARBOUR_CAPACITY; i++)
    {
        CloseHandle(threads[i]);
    }
    DeleteCriticalSection(&g_CritSection);

	return EXIT_SUCCESS;
}