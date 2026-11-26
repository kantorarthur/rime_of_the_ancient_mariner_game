import bpy
import mathutils
import struct
import sys
import os

args = sys.argv[sys.argv.index("--") + 1:]
SRC = args[0].rstrip("/") + "/"
out_dir = args[1]
wanted = dict(pair.split("=") for pair in args[2:])

LIBRARIES = [
    SRC + "Universal Animation Library[Standard]/Unreal-Godot/UAL1_Standard.glb",
    SRC + "Universal Animation Library 2[Standard]/Unreal-Godot/UAL2_Standard.glb",
]

FINGER_WORDS = ["index_", "middle_", "pinky_", "ring_", "thumb_", "_leaf"]
GL = mathutils.Matrix(((1, 0, 0, 0), (0, 0, 1, 0), (0, -1, 0, 0), (0, 0, 0, 1)))
GL_INVERSE = GL.inverted()


def kept(name):
    return not any(word in name for word in FINGER_WORDS)


def to_gl(matrix):
    return GL @ matrix @ GL_INVERSE


for library in LIBRARIES:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=library)
    scene = bpy.context.scene
    armature = [o for o in bpy.data.objects if o.type == 'ARMATURE'][0]
    bones = [b for b in armature.data.bones if kept(b.name)]
    fps = scene.render.fps / scene.render.fps_base

    for action in bpy.data.actions:
        source = action.name.replace("_Armature", "")
        if source not in wanted:
            continue
        if armature.animation_data is None:
            armature.animation_data_create()
        armature.animation_data.action = action
        start, end = int(action.frame_range[0]), int(action.frame_range[1])

        frames = []
        for frame in range(start, end + 1):
            scene.frame_set(frame)
            pose = []
            for bone in bones:
                posed = to_gl(armature.pose.bones[bone.name].matrix)
                if bone.parent:
                    local = to_gl(armature.pose.bones[bone.parent.name].matrix).inverted() @ posed
                    rest = to_gl(bone.parent.matrix_local).inverted() @ to_gl(bone.matrix_local)
                else:
                    local = posed
                    rest = to_gl(bone.matrix_local)
                rotation = local.to_quaternion()
                offset = local.to_translation() - rest.to_translation()
                pose.append((rotation, offset))
            frames.append(pose)

        path = os.path.join(out_dir, wanted[source] + ".anim")
        with open(path, "wb") as f:
            f.write(b"ANIM")
            f.write(struct.pack("<I", 1))
            f.write(struct.pack("<f", fps))
            f.write(struct.pack("<I", len(frames)))
            f.write(struct.pack("<I", len(bones)))
            for bone in bones:
                f.write(struct.pack("<32s", bone.name.encode()))
            for pose in frames:
                for rotation, offset in pose:
                    f.write(struct.pack("<4f", rotation.x, rotation.y, rotation.z, rotation.w))
                    f.write(struct.pack("<3f", offset.x, offset.y, offset.z))
        print("exported", source, "->", wanted[source], "frames", len(frames), "fps", fps)
