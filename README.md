# The Rime of the Ancient Mariner

> *Oh! dream of joy! is this indeed*
> *The light-house top I see?*
>
> Samuel Taylor Coleridge, *The Rime of the Ancient Mariner* (1798)

A 3D game in OpenGL inspired by Coleridge's poem: a lighthouse island in the open sea, a small boat, and cargo adrift on the water.

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
| W A S D | move the camera |
| Space / Left Ctrl | move up / down |
| Left Shift (hold) | move 3× faster |
| Mouse | look around |
| Tab | release / capture the mouse |
| 1 / 2 / 3 | solid / wireframe / points |
| 4 | toggle flat / smooth shading |
| T | toggle weather (calm / storm) |
| F | toggle fog |
| L | lightning strike (during the storm) |
| Esc | quit |

## Project layout

- `src/core`: window and input
- `src/graphics`: shaders, meshes, models, lights, shadow maps, view modes
- `src/scene`: camera, skybox, water, fog, wind, rain, lightning, lighthouse beam and scene layout
- `external`: stb_image, tinyobjloader
- `assets`: models, skyboxes and shaders loaded at runtime
- `tools`: Blender and ImageMagick scripts that converted the original downloads into `assets/models` and `assets/skybox` (`tools/convert_all.sh`)

Asset sources and licenses are listed in `CREDITS.md`.
