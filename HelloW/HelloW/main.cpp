#define _CRT_SECURE_NO_WARNINGS

#include<Windows.h>
#include<iostream>
#include<psapi.h>
#include<string>


typedef int (*ArithmeticOp)(int, int);

int main()
{
    HMODULE hmod =  LoadLibraryA("CalcDll.dll");
    if (hmod)
    {
        int ordinal = 1;

        ArithmeticOp sum = static_cast<ArithmeticOp>(static_cast<void*>(GetProcAddress(hmod, (LPCSTR)ordinal)));

        std::cout << "x+x = " << sum(10, 10) << std::endl;

        ordinal = 2;
        ArithmeticOp subtract = static_cast<ArithmeticOp>(static_cast<void*>(GetProcAddress(hmod, (LPCSTR)ordinal)));

        std::cout << "x-x=" << subtract(10, 10) << std::endl;

        ordinal = 3;
        ArithmeticOp multiply = static_cast<ArithmeticOp>(static_cast<void*>(GetProcAddress(hmod, (LPCSTR)ordinal)));

        std::cout << "x*x=" << multiply(10, 10) << std::endl;
    }
    else
        std::cout << "Lib wasnt opened\n";

    return EXIT_SUCCESS;
}