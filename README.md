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
| Esc | quit |

## Project layout

- `src/core`: window and input
- `src/graphics`: shaders, meshes, models, view modes
- `src/scene`: camera and scene layout
- `external`: stb_image, tinyobjloader
- `assets`: models and shaders loaded at runtime
- `assets_src`: original downloads (not needed at runtime)
- `tools`: converts `assets_src` into `assets/models` (requires Blender and ImageMagick): `bash tools/convert_all.sh`

Asset sources and licenses are listed in `CREDITS.md`.
