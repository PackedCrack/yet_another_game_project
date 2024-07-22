//
// Created by qwerty on 22/07/2024.
//
#define LOG_DIRECTORY "Log/game"
#include "debug/debug_defines.h"
int main()
{
    LOG_INFO("info");
    LOG_WARN("warn");
    LOG_ERR("error");
    LOG_FATAL("fatal");
    return 0;
}
