//
// Created by qwerty on 22/07/2024.
//
#include <cstdio>
#include <signal.h>
int main()
{
    std::printf("Hello");
    // cppcheck-suppress unreachableCode
    raise(SIGABRT);
    // cppcheck-suppress unreachableCode
    __debugbreak();
    // cppcheck-suppress unreachableCode
    return 1;
}
