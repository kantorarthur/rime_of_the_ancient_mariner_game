#!/usr/bin/env bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TOOLS="$ROOT/tools"
SRC="$ROOT/assets_src"
OUT="$ROOT/assets/characters"
OUTFITS="$SRC/Modular Character Outfits - Fantasy[Standard]"
BASE="$SRC/Universal Base Characters[Standard]"
HAIR="$BASE/Hairstyles/Rigged to Head Bone/glTF (Godot -Unreal)/T_Hair_1_BaseColor.png"
TEXTURES="$OUT/textures"

mkdir -p "$TEXTURES" "$OUT/animations"

texture()
{
    convert "$1" -resize 1024x1024 -quality 90 "$TEXTURES/$2.jpg"
}

hair()
{
    convert "$HAIR" -resize 512x512 \( +clone -fill "$1" -colorize 100 \) -compose multiply -composite -quality 90 "$TEXTURES/$2.jpg"
}

texture "$OUTFITS/Exports/glTF (Godot-Unreal)/Outfits/T_Peasant_BaseColor.png" peasant_1
texture "$OUTFITS/Textures/Peasant/T_Peasant_2_BaseColor.png" peasant_2
texture "$OUTFITS/Exports/glTF (Godot-Unreal)/Outfits/T_Ranger_BaseColor.png" ranger_1
texture "$OUTFITS/Textures/Ranger/T_Ranger_3_BaseColor.png" ranger_3
texture "$BASE/Base Characters/Textures/T_Superhero_Male_Ligh.png" skin_head_male
texture "$BASE/Base Characters/Textures/T_Superhero_Female_Light_BaseColor.png" skin_head_female
texture "$OUTFITS/Exports/glTF (Godot-Unreal)/Outfits/T_Regular_Male_Dark_BaseColor.png" skin_hands_male
convert "$BASE/Base Characters/Godot - UE/T_Eye_Brown.png" -resize 256x256 -quality 90 "$TEXTURES/eyes.jpg"
convert "$TEXTURES/ranger_1.jpg" -modulate 35,10 -quality 90 "$TEXTURES/ranger_dark.jpg"
convert "$TEXTURES/skin_head_male.jpg" -modulate 80,5 -quality 90 "$TEXTURES/skin_grey_male.jpg"
convert "$TEXTURES/skin_hands_male.jpg" -modulate 80,5 -quality 90 "$TEXTURES/skin_grey_hands.jpg"
convert "$TEXTURES/skin_head_female.jpg" -modulate 135,12 -quality 90 "$TEXTURES/skin_pale_female.jpg"
hair '#3a2616' hair_brown
hair '#1c1714' hair_black
hair '#d8d4cc' hair_white
hair '#c49a45' hair_gold

blender -b --factory-startup -P "$TOOLS/build_characters.py" -- "$SRC" "$TOOLS/characters.json" "$OUT" 2>&1 | grep -E "^built|Traceback" || true

blender -b --factory-startup -P "$TOOLS/export_animations.py" -- "$SRC" "$OUT/animations" \
    Idle_Loop=idle Idle_Talking_Loop=talk Driving_Loop=helm Death01=death Sitting_Idle_Loop=sit Sitting_Talking_Loop=sit_talk \
    Pistol_Aim_Neutral=aim Pistol_Shoot=shoot Spell_Simple_Shoot=bless Spell_Simple_Idle_Loop=bless_hold Crouch_Idle_Loop=grieve \
    Fixing_Kneeling=kneel Dance_Loop=dance Walk_Loop=walk Walk_Formal_Loop=walk_formal Swim_Idle_Loop=swim Hit_Chest=hit \
    Interact=interact Push_Loop=haul Idle_Rail_Loop=rail Idle_Rail_Call=rail_call Idle_No_Loop=shake_head Idle_FoldArms_Loop=fold_arms \
    Idle_Lantern_Loop=lantern LayToIdle=rise Zombie_Idle_Loop=zombie Zombie_Walk_Fwd_Loop=zombie_walk Yes=yes OverhandThrow=throw Consume=consume \
    2>&1 | grep -E "^exported|Traceback" || true
