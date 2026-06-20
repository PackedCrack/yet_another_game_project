# Odin Game Engine

A WIP game engine for educational purposes. The renderer is GPU driven and uses the Vulkan API and compute shaders to leverage GPGPU to build draw commands on the GPU itself. It relies on ECS to drive things such as game logic and input handling.


```text
Odin-Game-Engine/
├── cmake/
├── common/                 # Shared headers/types used across modules
├── external/               # Third-party dependencies
├── resources/              # Runtime assets/resources
├── src/
│   ├── Debug/              # Logging/debug support
│   │   └── src/
│   │       ├── Logger.cpp
│   │       ├── Logger.hpp
│   │       └── debug_defines.hpp
│   │
│   ├── AssetLoader/        # Asset registry, scene graph, meshes, materials, glTF
│   │   └── src/
│   │       ├── AssetRegistry.cpp
│   │       ├── Material.cpp
│   │       ├── Mesh.cpp
│   │       ├── ModelHandle.cpp
│   │       ├── SceneGraph.cpp
│   │       ├── SceneGraphNode.cpp
│   │       ├── UUID.cpp
│   │       └── gltf_loader.cpp
│   │
│   ├── Engine/             # Core engine runtime
│   │   └── src/
│   │       ├── Core.cpp
│   │       ├── Odin.cpp
│   │       ├── ECS.cpp
│   │       ├── Entity.cpp
│   │       ├── Input.cppp
│   │       ├── component/  # ECS component data
│   │       ├── graphics/   # Rendering, Vulkan, frame handling, attachments
│   │       ├── state/      # Runtime/input state
│   │       ├── system/     # Engine systems, e.g. transform
│   │       └── window/     # SDL/window handling
│   │
│   └── Game/               # Application/game executable
│       └── src/
│           └── main.cpp
│
├── CMakeLists.txt
├── asset-check.py
└── cppcheck.py
````


## Build Instructions

`The main branch is likely not the latest!` 

There are several external dependencies that must be manually installed to build the project:

* CMake
* Vulkan SDK
* cppcheck
* clang-format
* clang-tidy


### Windows/Visual Studio

````PowerShell
git clone --branch <branch> --single-branch --recurse-sumbodules git@github.com:PackedCrack/Odin-Game-Engine.git

cd Odin-Game-Engine

mkdir vsbuild; cd vsbuild

cmake ..

.\Odin.slnx
````
