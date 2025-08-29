//
// Created by qwerty on 22/07/2024.
//
// AssetLoader
#include <assetloader/ModelHandle.hpp>
// Enginge
#include <engine/Odin.hpp>
#include <engine/components/Model.hpp>
#include <engine/state/Input.hpp>
#include <engine/window/DisplayResolution.hpp>
// Debug
#include <debug/Logger.hpp>
#include <debug/debug_defines.hpp>
// Win32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
//
//
int main(int argc, char** argv)
{
    try
    {
        odin::WindowInfo wndInfo{ .resolution = odin::window::HDPlus{}, .borderless = false, .fullscreen = false, .mouseGrab = false };
        odin::OdinInfo info{ .argc = argc, .argv = argv, .applicationName = "Odin Application", .windowInfo = std::move(wndInfo) };
        odin::Odin engine{ info };

        std::unique_ptr<odin::ECS> ecs = engine.make_ecs();
        odin::Entity e = ecs->make_entity();
        std::filesystem::path filepath{ R"(C:\Users\qwerty\Documents\repos\game\resources\assets\meshes\Lantern.glb)" };
        e.emplace<odin::component::Model>(filepath);


        while (engine.running())
        {
            engine.begin_frame();
            engine.render();
            engine.end_frame();
        }


        return EXIT_SUCCESS;
    }
    catch (const debug::fatal& err)
    {
        MessageBoxA(nullptr, err.what(), "Fatal Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
        return EXIT_FAILURE;
    }
}
