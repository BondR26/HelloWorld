#include<Windows.h>
#include<processthreadsapi.h>
#include<iostream>
#include<string>
#include<vector>
#include<set>
#include<algorithm>
/*з. Реализовать класс Gamble, который с помощью рандома загадывает простое число.
Класс реализует функцию MakePrediction(std::string userName, int prediction)
С консоли пользователи вводят свои предположения, для этого мы запрашиваем у пользователя его имя и число.
После ввода имени и числа создается поток, который вызывает MakePrediction.
MakePrediction добавляет предсказание в мембер класса Gamble (std::map).
Через 2 минуты после старта приложения,
Gamble определяет победителя для этого находит ставку наиболее близкую к загаданному числу и выводит на экран.

Доп задача: std::map нельзя использовать многопоточно, т.е. доступ к ней должен быть синхронизирован.
Попробуйте синхронизировать доступ используя CriticalSection из Winapi*/

CRITICAL_SECTION            g_CritSection = { NULL };

typedef std::pair<std::string, int> Attempt;

struct AttemptLess
{
    bool operator()(const Attempt& lhs, const Attempt& rhs) const
    {
        return lhs.second < rhs.second;
    }
};



inline int GenerateSecond(int seconds)
{
    return seconds * 1000;
}

class Gamble
{
public:
    Gamble()
    {
        srand(time(0));
    }
    ~Gamble() {}

    int GenerateNext(int limit = 0)
    {
        if (limit == 0)
            return m_RandNum = rand();
        else
            return m_RandNum = rand() % limit;
    }

    void MakePrediction(Attempt attempt)
    {
        m_guesses.insert(attempt);
    }

    std::multiset<Attempt>::iterator SelectTheWinner()
    {
        std::multiset<Attempt>::iterator lowerBoundIt = {};
        if ((lowerBoundIt = m_guesses.find(Attempt{ "Computer", m_RandNum })) != m_guesses.end())
        {
            return lowerBoundIt;
        }
        else
        {
            if ((lowerBoundIt = m_guesses.lower_bound(Attempt{ "Computer", m_RandNum })) != m_guesses.end())
            {
                if (lowerBoundIt == m_guesses.begin())
                    return lowerBoundIt;
                else
                {
                    //Here iterator we look for must be behind the searched answer;
                    //what we do we assing below the searched iterator value and then resotre its org value
                    std::multiset<Attempt>::iterator behindSearcheIt = --lowerBoundIt;
                    lowerBoundIt++;
                    if ((m_RandNum - behindSearcheIt->second) < (lowerBoundIt->second - m_RandNum))
                        return behindSearcheIt;
                    else
                        return lowerBoundIt;

                }
            }

        }
    }

    long GetRandNum()
    {
        return m_RandNum;
    }

private:

    long                                 m_RandNum = 0;
    std::multiset<Attempt, AttemptLess>  m_guesses = {};
};

struct PredictionAndGamble
{
    Gamble  gamble = {};
    Attempt usernameGamble = {};
};

DWORD WINAPI MyThread(void* context)
{
    Sleep(GenerateSecond(1));
    EnterCriticalSection(&g_CritSection);

    PredictionAndGamble* predAndGambl = static_cast<PredictionAndGamble*>(context);

    predAndGambl->gamble.MakePrediction(predAndGambl->usernameGamble);

    LeaveCriticalSection(&g_CritSection);

    return EXIT_SUCCESS;
}

int main()
{
    HANDLE                      threadHandle = NULL;
    Gamble                      gamble = {};
    std::string                 username = {};
    int                         guess = 0;
    Attempt                     usernameGuess = {};

    gamble.GenerateNext();
    std::cout << "THe number was generated, now you shoud try to guess it\n";
    Sleep(GenerateSecond(1));

    std::cout << "Please enter your username ";
    std::cin >> username;
    std::cout << "Please enter your guess ";
    std::cin >> guess;

    usernameGuess = Attempt{ username,guess };
    PredictionAndGamble predAndGamble = { gamble, usernameGuess };

    InitializeCriticalSection(&g_CritSection);

    threadHandle = CreateThread(NULL, NULL, MyThread, static_cast<void*>(&predAndGamble), NULL, NULL);
    if (threadHandle == INVALID_HANDLE_VALUE)
        return FALSE;
    WaitForSingleObject(threadHandle, INFINITE);
    if (threadHandle != INVALID_HANDLE_VALUE)
        CloseHandle(threadHandle);

    predAndGamble.gamble.MakePrediction(Attempt("Motherfucker", 100500));
    predAndGamble.gamble.MakePrediction(Attempt("John Wayne", 10000));
    std::multiset<Attempt>::iterator it = predAndGamble.gamble.SelectTheWinner();

    std::cout << "And the winner is " << it->first << " " << it->second << std::endl;
    std::cout << "The number was " << predAndGamble.gamble.GetRandNum() << std::endl;

    return EXIT_SUCCESS;
}