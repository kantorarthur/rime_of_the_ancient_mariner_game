# The Rime of the Ancient Mariner

> *Oh! dream of joy! is this indeed*
> *The light-house top I see?*
>
> Samuel Taylor Coleridge, *The Rime of the Ancient Mariner* (1798)

A story-driven 3D game written in C++ with OpenGL, based on Coleridge's poem. You sail the mariner's ship around a lighthouse island at night, shoot the albatross, live through the curse, the ghost ship and the dying crew, bless the water-snakes, and come home to the lighthouse. The game ends with the old mariner telling his tale outside a wedding. The verses of the poem appear on screen as the story unfolds.

## Motivation

I really loved Iron Maiden's song "Rime of the Ancient Mariner" (*Powerslave*, 1984). After listening to it, I read Samuel Taylor Coleridge's poem that the song is based on, and then I decided to turn the story into a game for my university graphics project.

## Built with

- **C++17** and **OpenGL 4.1 core**, with shaders written in **GLSL**
- **GLFW** for the window and input, **GLEW** to load OpenGL, **GLM** for the mathematics
- **CMake** for the build
- **tinyobjloader** for OBJ models, **stb_image** for textures, **stb_easy_font** for on-screen text
- **Blender** (Python scripts) to build the skinned characters and export their animations
- **ImageMagick** and **Python** to convert textures and skyboxes and to generate some models

Rendering techniques: Blinn-Phong lighting with directional, point and spot lights, shadow mapping with PCF, Gerstner waves with Fresnel reflections, analytic height fog, a volumetric lighthouse beam, instanced rain, procedural lightning, skeletal animation and cinematic cameras.

## Download and play

Download the archive for your system from the [Releases](https://github.com/kantorarthur/rime_of_the_ancient_mariner_game/releases) page and unzip it.

- **Linux:** `rime_of_the_ancient_mariner-linux.zip`, then run `./rime_of_the_ancient_mariner.sh` in the unzipped folder.
- **Windows:** `rime_of_the_ancient_mariner-windows.zip`, then double-click `rime_of_the_ancient_mariner.exe` in the unzipped folder.

It needs a graphics card with OpenGL 4.1. Press Esc in the game for the menu and the list of controls.

## Build from source (Linux)

```bash
sudo apt install build-essential cmake libglfw3-dev libglew-dev libglm-dev
cmake -S . -B build
cmake --build build -j
./build/rime_of_the_ancient_mariner
```

`tools/make_release.sh` builds the game and packs it, with its assets and libraries, into `release/rime_of_the_ancient_mariner-linux.zip`. `tools/make_release_windows.sh` cross-compiles the Windows version with MinGW-w64 (`sudo apt install mingw-w64`) into `release/rime_of_the_ancient_mariner-windows.zip`.

## Project layout

- `src/core`: window and input
- `src/graphics`: shaders, meshes, models, skinned meshes and skeletal animation, lights, shadow maps, on-screen text, view modes
- `src/scene`: camera, skybox, water, fog, wind, rain, lightning, lighthouse beam, water-snakes and scene layout
- `src/game`: the story, the cinematic cameras, the ship controls and the pause menu
- `external`: stb_image, stb_easy_font, tinyobjloader
- `assets`: models, characters and their animations, skyboxes and shaders loaded at runtime
- `tools`: Blender, ImageMagick and Python scripts that build the assets, and the release scripts

Asset sources and licenses are listed in `CREDITS.md`.
