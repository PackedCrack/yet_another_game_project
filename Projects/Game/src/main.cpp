//
// Created by qwerty on 22/07/2024.
//
#define LOG_DIRECTORY "Log/game"
#include "debug/debug_defines.h"

#include "asset_loader/Model.h"
//
//
int main()
{
    LOG_INFO("info");
    LOG_WARN("warn");
    LOG_ERR("error");
    LOG_FATAL("fatal");
    asl::Model mdata{ R"(C:\Users\qwerty\Documents\repos\game\resources\assets\meshes\Corset.glb)" };


    return 0;
}
