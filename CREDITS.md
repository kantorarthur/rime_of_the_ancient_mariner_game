# Credits and licenses

## CC-BY 4.0 models (attribution required)

- **Lighthouse**: "FREE Old Style Lighthouse Assets (fixed)" by aresdavide94
  https://sketchfab.com/3d-models/free-old-style-lighthouse-assets-fixed-ad450ca49c0c4ef3831059039191509a
  License: CC-BY 4.0 (http://creativecommons.org/licenses/by/4.0/)

- **Keeper's house**: "Wooden House Building" by Rescue3D Assets (@rescue3d)
  https://sketchfab.com/3d-models/wooden-house-building-6e46d5e6787243518eb14d5c9db4fa82
  License: CC-BY 4.0 (http://creativecommons.org/licenses/by/4.0/)

- **Boat**: "Fishing Boat" by Rafael Rodrigues (@Rafael_BR873D)
  https://sketchfab.com/3d-models/fishing-boat-6e33005fba3542a2aa61e187321d465e
  License: CC-BY 4.0 (http://creativecommons.org/licenses/by/4.0/)
  (used without the original model's decorative sand, water and rock base)

## Poly Haven, CC0 (public domain)

- wooden_crate_02: https://polyhaven.com/a/wooden_crate_02
- wooden_lantern_01: https://polyhaven.com/a/wooden_lantern_01
- modular_wooden_pier: https://polyhaven.com/a/modular_wooden_pier
- coastal_cliff_02: https://polyhaven.com/a/coastal_cliff_02
- rock_boulder_dry (texture): https://polyhaven.com/a/rock_boulder_dry
- rock_boulder_cracked (texture): https://polyhaven.com/a/rock_boulder_cracked
- coast_sand_rocks_02 (texture): https://polyhaven.com/a/coast_sand_rocks_02
- dutch_ship_large_01: https://polyhaven.com/a/dutch_ship_large_01
- clarens_night_01 (HDRI, storm skybox): https://polyhaven.com/a/clarens_night_01
- clarens_night_02 (HDRI, calm skybox): https://polyhaven.com/a/clarens_night_02

## BlendSwap, CC0 (public domain)

- **Albatross**: based on "Low Poly Seagull" by Nodelete, https://blendswap.com/blend/7419
  (wings posed in flight, wing tips darkened and vertex colors baked into a texture by `tools/make_albatross.py`)

## Generated in this project

- The rocks (`rocks/rock_1..3`), the island (`island/`) and the curved cliff ring (`cliff/cliff_ring`)
  are generated procedurally by `tools/convert.py` from the Poly Haven assets above.
- The skyboxes (`assets/skybox/calm`, `assets/skybox/storm`) are cube maps made from the Poly Haven HDRIs above
  by `tools/hdri2cubemap.py`; the land below the horizon is replaced with a sea haze.

## Text

- Epigraph from *The Rime of the Ancient Mariner* by Samuel Taylor Coleridge (1798), public domain.

## Libraries

- stb_image (Sean Barrett, public domain / MIT)
- tinyobjloader (Syoyo Fujita, MIT)
