# The Rime of the Ancient Mariner

> *Oh! dream of joy! is this indeed*
> *The light-house top I see?*
>
> Samuel Taylor Coleridge, *The Rime of the Ancient Mariner* (1798)

A 3D game in OpenGL inspired by Coleridge's poem: a lighthouse island in the open sea, a small boat, cargo adrift on the water, and an albatross circling the light.

## Build (Linux)

```bash
sudo apt install build-essential cmake libglfw3-dev libglew-dev libglm-dev
cmake -S . -B build
cmake --build build -j
./build/app
```

Run the program from the project root, since the paths to `assets/` are relative.

## Controls

| Key | Action |
|---|---|
| W / S | raise / lower the sails |
| A / D | turn the helm left / right |
| Mouse | look around the ship |
| X | take up the cross-bow (when the story asks for it) |
| C | switch between the ship and the free camera |
| Enter | skip to the next verse |
| W A S D, Space / Left Ctrl | move the free camera |
| Left Shift (hold) | free camera 3× faster |
| Tab | release / capture the mouse |
| 1 / 2 / 3 | solid / wireframe / points |
| 4 | toggle flat / smooth shading |
| T | toggle weather (calm / storm) |
| F | toggle fog |
| L | lightning strike (during the storm) |
| Esc | quit |

## Project layout

- `src/core`: window and input
- `src/graphics`: shaders, meshes, models, skinned meshes and skeletal animation, lights, shadow maps, on-screen text, view modes
- `src/scene`: camera, skybox, water, fog, wind, rain, lightning, lighthouse beam and scene layout
- `src/game`: the story, the ship controls and the cameras
- `external`: stb_image, stb_easy_font, tinyobjloader
- `assets`: models, characters and their animations, skyboxes and shaders loaded at runtime
- `tools`: Blender and ImageMagick scripts that converted the original downloads into `assets/models` and `assets/skybox` (`tools/convert_all.sh`), and the scripts that build the animated characters (`tools/build_characters.sh`)

Asset sources and licenses are listed in `CREDITS.md`.
