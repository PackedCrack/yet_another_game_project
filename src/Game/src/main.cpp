//
// Created by qwerty on 22/07/2024.
//
// AssetLoader
#include <assetloader/ModelHandle.hpp>
// Enginge
#include <engine/Odin.hpp>
#include <engine/components/Model.hpp>
#include <engine/components/Geometry.hpp>
#include <engine/components/Parent.hpp>
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


        odin::Entity e = odin::make_entity();
        std::filesystem::path filepath{ R"(C:\Users\qwerty\Documents\repos\game\resources\assets\meshes\Lantern.glb)" };
        odin::emplace_component<odin::component::Model>(e, filepath);

        auto move = [&e]()
        {
            const auto& model = odin::get_components<odin::component::Model>(e);
            static bool a = true;
            if (a)
            {
                odin::rotate(model.root, 90.0f, { 1.0f, 0.0f, 1.0f });
                a = false;
            }

            auto time = (float) std::chrono::steady_clock::now().time_since_epoch().count();
            //odin::rotate(model.root, 0.01f, { 0.0f, 1.0f, 0.0f });
            for (auto&& node : model.nodes)
            {
                glm::vec3 delta{};
                delta.y = std::sin(time) / 10;
                //delta.x = std::sin(time) / 10;

                const odin::Entity& n = node.second;
                odin::translate(n, delta);
            }
        };

        while (engine.running())
        {
            engine.begin_frame();

            move();

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
