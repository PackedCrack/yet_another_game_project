//
// Created by qwerty on 22/07/2024.
//
#define LOG_DIRECTORY "Log/game"
#include "debug/Logger.h"

#include "asset_loader/Model.h"
//
//
int main()
{
    LOG_DEBUG("debug {}", 1);
    LOG_INFO("info {}", 1);
    LOG_WARN("warn {}", 1);
    LOG_ERR("error {}", 1);
    asl::Model mdata{ R"(C:\Users\qwerty\Documents\repos\game\resources\assets\meshes\Corset.glb)" };


    return 0;
}
