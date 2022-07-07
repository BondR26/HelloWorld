#include "pch.h"
#include "calc.h"

extern "C" int Add(int first, int second)
{
    return first + second;
}

extern "C" int Sum(int a, int b) {
    return a + b;
}

extern "C" int Subtract(int first, int second)
{
    return first - second;
}

extern "C" int Multiply(int first, int second)
{
    return first * second;
}