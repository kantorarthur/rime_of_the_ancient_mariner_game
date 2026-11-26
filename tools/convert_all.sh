#!/usr/bin/env bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TOOLS="$ROOT/tools"
SRC="$ROOT/assets_src"
OUT="$ROOT/assets/models"

conv()
{
    blender -b --disable-autoexec --factory-startup -P "$TOOLS/convert.py" -- "$1" 2>&1 | grep -E "^RESULT|Traceback|Error: Python" || true
}

spec()
{
    blender -b --disable-autoexec --factory-startup -P "$TOOLS/rough2spec.py" -- "$1" "$2" "$3" "$4" 2>&1 | grep -E "^SPEC|Traceback" || true
}

diffuse()
{
    convert "$1" -resize "${3}x${3}" -quality 90 "$2"
    echo "DIFF $2"
}

conv "{\"name\":\"lighthouse\",\"src\":\"$SRC/free_old_style_lighthouse_assets_fixed/scene.gltf\",\"outdir\":\"$OUT/lighthouse\",\"scale\":3.0,\"max_tris\":25000}"
conv "{\"name\":\"house\",\"src\":\"$SRC/wooden_house_building.glb\",\"outdir\":\"$OUT/house\",\"scale\":0.6,\"unpack_images\":true}"
conv "{\"name\":\"crate\",\"src\":\"$SRC/wooden_crate_02_4k.blend/wooden_crate_02_4k.blend\",\"outdir\":\"$OUT/crate\",\"scale\":1.0}"
conv "{\"name\":\"lantern\",\"src\":\"$SRC/wooden_lantern_01_4k.blend/wooden_lantern_01_4k.blend\",\"outdir\":\"$OUT/lantern\",\"scale\":1.0}"
conv "{\"name\":\"pier_a\",\"src\":\"$SRC/modular_wooden_pier_4k.blend/modular_wooden_pier_4k.blend\",\"keep\":[\"modular_wooden_pier_section_02\"],\"outdir\":\"$OUT/pier\",\"max_tris\":9000}"
conv "{\"name\":\"pier_b\",\"src\":\"$SRC/modular_wooden_pier_4k.blend/modular_wooden_pier_4k.blend\",\"keep\":[\"modular_wooden_pier_section_03\"],\"outdir\":\"$OUT/pier\",\"max_tris\":9000}"
conv "{\"name\":\"cliff_ring\",\"kind\":\"cliff_ring\",\"src\":\"$SRC/coastal_cliff_02_4k.blend/coastal_cliff_02_4k.blend\",\"keep\":[\"coastal_cliff_02_LOD3\"],\"outdir\":\"$OUT/cliff\",\"segment_tris\":9000,\"radius\":20.5,\"length_scale\":0.6,\"height_scale\":0.7,\"depth_scale\":0.6,\"base_y\":-2.6,\"start_angle\":25,\"end_angle\":300,\"segments\":5,\"taper\":0.2,\"clip_height\":3.85}"
for seed in 1 2 3
do
    conv "{\"name\":\"rock_$seed\",\"kind\":\"rock\",\"seed\":$((seed * 17)),\"outdir\":\"$OUT/rocks\",\"max_tris\":3000,\"smooth\":true}"
done
conv "{\"name\":\"boat\",\"src\":\"$SRC/fishing_boat/scene.gltf\",\"outdir\":\"$OUT/boat\",\"scale\":1.0,\"exclude_prefix\":[\"Cenario\",\"Rock\"]}"
conv "{\"name\":\"island\",\"kind\":\"island\",\"outdir\":\"$OUT/island\",\"size\":64,\"res\":128,\"radius\":22,\"cliff_radius\":20.5,\"wall_start_offset\":-1.0,\"wall_end_offset\":0.0,\"rim_width\":0.0,\"rim_drop\":0.0,\"calm_start\":16.5,\"top\":3.5,\"uv_tile\":6,\"beach_start\":290,\"beach_end\":30,\"beach_fade\":10}"

diffuse "$SRC/free_old_style_lighthouse_assets_fixed/textures/Material_baseColor.png" "$OUT/lighthouse/lighthouse_diff.jpg" 2048
spec "$SRC/free_old_style_lighthouse_assets_fixed/textures/Material_metallicRoughness.png" "$OUT/lighthouse/lighthouse_spec.png" 1024 1
convert "$SRC/free_old_style_lighthouse_assets_fixed/textures/Material_emissive.png" -resize 1024x1024 "$OUT/lighthouse/lighthouse_emissive.png"
diffuse "$OUT/house/_packed_Image_0.png" "$OUT/house/house_diff.jpg" 2048
rm -f "$OUT"/house/_packed_*.png
diffuse "$SRC/wooden_crate_02_4k.blend/textures/wooden_crate_02_diff_4k.jpg" "$OUT/crate/crate_diff.jpg" 1024
spec "$SRC/wooden_crate_02_4k.blend/textures/wooden_crate_02_rough_4k.exr" "$OUT/crate/crate_spec.png" 1024 0
diffuse "$SRC/wooden_lantern_01_4k.blend/textures/wooden_lantern_01_diff_4k.jpg" "$OUT/lantern/lantern_diff.jpg" 1024
spec "$SRC/wooden_lantern_01_4k.blend/textures/wooden_lantern_01_rough_4k.exr" "$OUT/lantern/lantern_spec.png" 1024 0
diffuse "$SRC/modular_wooden_pier_4k.blend/textures/modular_wooden_pier_planks_diff_4k.png" "$OUT/pier/planks_diff.jpg" 1024
spec "$SRC/modular_wooden_pier_4k.blend/textures/modular_wooden_pier_planks_rough_4k.png" "$OUT/pier/planks_spec.png" 1024 0
diffuse "$SRC/modular_wooden_pier_4k.blend/textures/modular_wooden_pier_poles_diff_4k.png" "$OUT/pier/poles_diff.jpg" 1024
spec "$SRC/modular_wooden_pier_4k.blend/textures/modular_wooden_pier_poles_rough_4k.png" "$OUT/pier/poles_spec.png" 1024 0
diffuse "$SRC/coastal_cliff_02_4k.blend/textures/coastal_cliff_02_diff_4k.jpg" "$OUT/cliff/cliff_diff.jpg" 2048
spec "$SRC/coastal_cliff_02_4k.blend/textures/coastal_cliff_02_rough_4k.exr" "$OUT/cliff/cliff_spec.png" 1024 0
diffuse "$SRC/rock_boulder_dry_4k.blend/textures/rock_boulder_dry_diff_4k.jpg" "$OUT/rocks/rock_dry_diff.jpg" 1024
spec "$SRC/rock_boulder_dry_4k.blend/textures/rock_boulder_dry_rough_4k.exr" "$OUT/rocks/rock_dry_spec.png" 512 0
diffuse "$SRC/coast_sand_rocks_02_4k.blend/textures/coast_sand_rocks_02_diff_4k.jpg" "$OUT/island/ground_diff.jpg" 2048

conv "{\"name\":\"ship\",\"src\":\"$SRC/dutch_ship_large_01_4k.blend/dutch_ship_large_01_4k.blend\",\"outdir\":\"$OUT/ship\",\"scale\":1.0,\"max_tris\":36000}"
SHIP="$SRC/dutch_ship_large_01_4k.blend/textures"
diffuse "$SHIP/dutch_ship_large_01_hull_diff_4k.png" "$OUT/ship/hull_diff.jpg" 2048
diffuse "$SHIP/dutch_ship_large_01_rigging_diff_4k.png" "$OUT/ship/rigging_diff.jpg" 2048
convert "$SHIP/dutch_ship_large_01_sails_diff_4k.png" "$SHIP/dutch_ship_large_01_sails_alpha_4k.png" -alpha off -compose CopyOpacity -composite -resize 1024x1024 -define png:compression-level=9 "$OUT/ship/sails_diff.png"
spec "$SHIP/dutch_ship_large_01_hull_rough_4k.png" "$OUT/ship/hull_spec.png" 1024 0

blender -b --disable-autoexec --factory-startup -P "$TOOLS/make_albatross.py" -- "{\"src\":\"$SRC/Low Poly Seagull/Seagull - rev001.blend\",\"outdir\":\"$OUT/albatross\",\"wingspan\":3.0,\"subdivisions\":2,\"feather_strength\":0.25,\"dark_start\":0.4}" 2>&1 | grep -E "^RESULT|Traceback" || true

python3 "$TOOLS/write_materials.py" "$OUT"
python3 "$TOOLS/make_crossbow.py" "$OUT/crossbow"

SKY="$ROOT/assets/skybox"
sky()
{
    blender -b --disable-autoexec --factory-startup -P "$TOOLS/hdri2cubemap.py" -- "$1" 2>&1 | grep -E "^MOON|Traceback" || true
    for face in "$2"/*.png
    do
        convert "$face" -quality 92 "${face%.png}.jpg"
        rm "$face"
    done
}
sky "{\"src\":\"$SRC/clarens_night_02_4k.exr\",\"outdir\":\"$SKY/calm\",\"size\":1024,\"exposure\":1.0,\"rotate\":12,\"horizon_color_srgb\":[0.07,0.09,0.13],\"horizon_low\":12.0,\"horizon_high\":20.0,\"horizon_low_near_moon\":1.0,\"horizon_high_near_moon\":10.0,\"moon_width\":30}" "$SKY/calm"
sky "{\"src\":\"$SRC/clarens_night_01_4k.exr\",\"outdir\":\"$SKY/storm\",\"size\":1024,\"exposure\":0.45,\"saturation\":0.35,\"tint\":[0.85,0.92,1.05],\"rotate\":12,\"horizon_color_srgb\":[0.05,0.06,0.07],\"horizon_low\":12.0,\"horizon_high\":20.0,\"horizon_low_near_moon\":1.0,\"horizon_high_near_moon\":10.0,\"moon_width\":30}" "$SKY/storm"

bash "$TOOLS/build_characters.sh"
