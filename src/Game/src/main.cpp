//
// Created by qwerty on 22/07/2024.
//
#define LOG_DIRECTORY "Log/game"
#include "debug/Logger.h"
#include "asset_loader/Model.h"
// Win32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
//
//
int main()
{
    try
    {
        LOG_DEBUG("debug");
        LOG_DEBUG("debug {} {}", 1, "asdf");
        LOG_INFO("info {}", 1);
        LOG_WARN("warn {}", 1);
        LOG_ERR("error {}", 1);
        asl::Model uvTestManyScenes{ R"(C:\Users\qwerty\Documents\repos\game\resources\assets\meshes\tests\TextureCoordinateTest.glb)" };
        asl::Model lanternManyGroups{ R"(C:\Users\qwerty\Documents\repos\game\resources\assets\meshes\Lantern.glb)" };


        return EXIT_SUCCESS;
    }
    catch (const debug::fatal& err)
    {
        std::int32_t result = MessageBoxA(nullptr, err.what(), "Fatal Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
        ODIN_ASSERT(result != 0);    // ironic
        return EXIT_FAILURE;
    }
}
