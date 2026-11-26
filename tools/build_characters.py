import bpy
import bmesh
import mathutils
import struct
import sys
import os
import json

args = sys.argv[sys.argv.index("--") + 1:]
SRC = args[0].rstrip("/") + "/"
recipe_path = args[1]
out_dir = args[2]

OUTFITS = SRC + "Modular Character Outfits - Fantasy[Standard]/Exports/glTF (Godot-Unreal)/Outfits/"
BASE = SRC + "Universal Base Characters[Standard]/Base Characters/Godot - UE/"
HAIR = SRC + "Universal Base Characters[Standard]/Hairstyles/Rigged to Head Bone/glTF (Godot -Unreal)/"

FINGER_WORDS = ["index_", "middle_", "pinky_", "ring_", "thumb_", "_leaf"]
GL = mathutils.Matrix(((1, 0, 0, 0), (0, 0, 1, 0), (0, -1, 0, 0), (0, 0, 0, 1)))
GL_INVERSE = GL.inverted()


def kept(name):
    return not any(word in name for word in FINGER_WORDS)


def import_gltf(path):
    before = set(bpy.data.objects)
    bpy.ops.import_scene.gltf(filepath=path)
    return [o for o in bpy.data.objects if o not in before]


def dominant_group(obj, vertex):
    best = None
    best_weight = 0.0
    for g in vertex.groups:
        if g.weight > best_weight:
            best_weight = g.weight
            best = obj.vertex_groups[g.group].name
    return best


def keep_head_only(obj):
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    deform = bm.verts.layers.deform.active
    names = {g.index: g.name for g in obj.vertex_groups}
    remove = []
    for face in bm.faces:
        for v in face.verts:
            weights = v[deform]
            head = sum(w for i, w in weights.items() if names[i] in ("Head", "neck_01"))
            if head < 0.5:
                remove.append(face)
                break
    bmesh.ops.delete(bm, geom=remove, context='FACES')
    loose = [v for v in bm.verts if not v.link_faces]
    bmesh.ops.delete(bm, geom=loose, context='VERTS')
    bm.to_mesh(obj.data)
    bm.free()


def bone_head(armature, name):
    return armature.matrix_world @ armature.data.bones[name].head_local


def triangulate(obj):
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(obj.data)
    bm.free()


def lengthen_beard(obj, factor):
    chin = min(v.co.z for v in obj.data.vertices)
    top = max(v.co.z for v in obj.data.vertices)
    jaw = chin + (top - chin) * 0.55
    for v in obj.data.vertices:
        if v.co.z < jaw:
            v.co.z = jaw - (jaw - v.co.z) * factor
            v.co.y -= (jaw - v.co.z) * 0.12


def build(recipe):
    bpy.ops.wm.read_factory_settings(use_empty=True)

    outfit_objects = import_gltf(OUTFITS + recipe["outfit"] + ".gltf")
    armature = [o for o in outfit_objects if o.type == 'ARMATURE'][0]
    parts = []
    for o in outfit_objects:
        if o.type != 'MESH' or o.parent != armature:
            continue
        if any(word in o.name for word in recipe.get("drop", [])):
            continue
        parts.append((o, recipe["outfit_texture"], recipe["hands_texture"]))

    body_name = "Superhero_Female_FullBody" if recipe["female"] else "Superhero_Male_FullBody"
    body_objects = import_gltf(BASE + body_name + ".gltf")
    body_armature = [o for o in body_objects if o.type == 'ARMATURE'][0]
    offset = bone_head(armature, "Head") - bone_head(body_armature, "Head")
    for o in body_objects:
        if o.type != 'MESH' or o.parent != body_armature:
            continue
        if o.name.startswith("Eyebrows") and recipe.get("hide_eyebrows"):
            continue
        if o.name.startswith("SuperHero") or o.name.startswith("Superhero"):
            keep_head_only(o)
            texture = recipe["skin_texture"]
        elif o.name.startswith("Eyes"):
            texture = "eyes"
        else:
            texture = recipe["hair_texture"]
        for v in o.data.vertices:
            v.co += offset
        parts.append((o, texture, None))

    for hair in recipe.get("hair", []):
        hair_objects = import_gltf(HAIR + hair + ".gltf")
        hair_armature = [o for o in hair_objects if o.type == 'ARMATURE'][0]
        hair_offset = bone_head(armature, "Head") - bone_head(hair_armature, "Head")
        for o in hair_objects:
            if o.type != 'MESH' or o.parent != hair_armature:
                continue
            for v in o.data.vertices:
                v.co += hair_offset
            if hair == "Hair_Beard" and recipe.get("beard_length", 1.0) != 1.0:
                lengthen_beard(o, recipe["beard_length"])
            texture = recipe["beard_texture"] if hair == "Hair_Beard" and "beard_texture" in recipe else recipe["hair_texture"]
            parts.append((o, texture, None))

    bones = [b for b in armature.data.bones if kept(b.name)]
    index_of = {b.name: i for i, b in enumerate(bones)}

    def kept_ancestor(bone):
        while not kept(bone.name):
            bone = bone.parent
        return bone.name

    vertices = []
    indices = []
    submeshes = {}
    lookup = {}

    for obj, outfit_texture, skin_texture in parts:
        triangulate(obj)
        mesh = obj.data
        mesh.calc_normals_split()
        uv = mesh.uv_layers.active.data if mesh.uv_layers.active else None
        to_armature = armature.matrix_world.inverted() @ obj.matrix_world
        normal_matrix = to_armature.to_3x3().inverted().transposed()
        group_names = {g.index: g.name for g in obj.vertex_groups}
        source_armature = obj.parent

        for poly in mesh.polygons:
            material = mesh.materials[poly.material_index].name if mesh.materials else ""
            texture = outfit_texture
            if skin_texture and "Regular" in material:
                texture = skin_texture
            if texture not in submeshes:
                submeshes[texture] = []
            for loop_index in poly.loop_indices:
                loop = mesh.loops[loop_index]
                vertex = mesh.vertices[loop.vertex_index]
                position = GL @ (to_armature @ vertex.co)
                normal = (GL.to_3x3() @ (normal_matrix @ loop.normal)).normalized()
                texcoord = (uv[loop_index].uv.x, uv[loop_index].uv.y) if uv else (0.0, 0.0)

                weights = {}
                for g in vertex.groups:
                    name = group_names[g.group]
                    if name not in source_armature.data.bones:
                        continue
                    target = kept_ancestor(source_armature.data.bones[name])
                    weights[index_of[target]] = weights.get(index_of[target], 0.0) + g.weight
                best = sorted(weights.items(), key=lambda item: -item[1])[:4]
                total = sum(w for _, w in best)
                if total <= 0.0:
                    best = [(index_of["Head"], 1.0)]
                    total = 1.0
                best = [(b, w / total) for b, w in best]
                while len(best) < 4:
                    best.append((0, 0.0))

                key = (obj.name, loop.vertex_index, round(texcoord[0], 5), round(texcoord[1], 5), round(normal.x, 3), round(normal.y, 3), round(normal.z, 3))
                if key not in lookup:
                    lookup[key] = len(vertices)
                    vertices.append((position, normal, texcoord, best))
                submeshes[texture].append(lookup[key])

    ordered = []
    for texture, list_of_indices in submeshes.items():
        ordered.append((texture, len(indices), len(list_of_indices)))
        indices.extend(list_of_indices)

    path = os.path.join(out_dir, recipe["name"] + ".skin")
    with open(path, "wb") as f:
        f.write(b"SKIN")
        f.write(struct.pack("<I", 1))
        f.write(struct.pack("<I", len(bones)))
        for bone in bones:
            parent = index_of[kept_ancestor(bone.parent)] if bone.parent else -1
            rest_armature = GL @ bone.matrix_local @ GL_INVERSE
            if bone.parent:
                parent_rest = GL @ bone.parent.matrix_local @ GL_INVERSE
                local = parent_rest.inverted() @ rest_armature
            else:
                local = rest_armature
            translation = local.to_translation()
            rotation = local.to_quaternion()
            inverse_bind = rest_armature.inverted()
            f.write(struct.pack("<32s", bone.name.encode()))
            f.write(struct.pack("<i", parent))
            f.write(struct.pack("<3f", translation.x, translation.y, translation.z))
            f.write(struct.pack("<4f", rotation.x, rotation.y, rotation.z, rotation.w))
            for column in range(4):
                for row in range(4):
                    f.write(struct.pack("<f", inverse_bind[row][column]))
        f.write(struct.pack("<I", len(vertices)))
        for position, normal, texcoord, best in vertices:
            f.write(struct.pack("<3f", position.x, position.y, position.z))
            f.write(struct.pack("<3f", normal.x, normal.y, normal.z))
            f.write(struct.pack("<2f", texcoord[0], texcoord[1]))
            f.write(struct.pack("<4B", *[b for b, _ in best]))
            f.write(struct.pack("<4f", *[w for _, w in best]))
        f.write(struct.pack("<I", len(ordered)))
        for texture, first, count in ordered:
            f.write(struct.pack("<64s", (texture + ".jpg").encode()))
            f.write(struct.pack("<II", first, count))
        f.write(struct.pack("<I", len(indices)))
        f.write(struct.pack("<%dI" % len(indices), *indices))

    print("built", recipe["name"], "bones", len(bones), "vertices", len(vertices), "triangles", len(indices) // 3, "textures", [t for t, _, _ in ordered])


for recipe in json.load(open(recipe_path)):
    build(recipe)
