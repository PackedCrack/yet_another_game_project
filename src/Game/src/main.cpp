//
// Created by qwerty on 22/07/2024.
//
#include "debug/Logger.hpp"
#include "assetloader/Model.hpp"
#include "engine/Odin.hpp"
// Win32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
//
//
int main()
{
    try
    {
        odin::WindowInfo wndInfo{ .resolution = odin::graphics::window::HDPlus{},
                                  .borderless = false,
                                  .fullscreen = false,
                                  .mouseGrab = false };
        odin::OdinInfo info{ .applicationName = "Odin Application", .windowInfo = std::move(wndInfo) };
        odin::Odin engine{ info };

        odin::graphics::Graphics& gfx = engine.graphics();
        gfx.draw();


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
