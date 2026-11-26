import bpy, sys, math
from mathutils import Vector
argv = sys.argv[sys.argv.index("--") + 1:]
path, out = argv[0], argv[1]
keep = argv[2].split(",") if len(argv) > 2 and argv[2] else None
if path.endswith(".blend"):
    bpy.ops.wm.open_mainfile(filepath=path, load_ui=False)
    for o in list(bpy.data.objects):
        if o.type in ('CAMERA', 'LIGHT'):
            bpy.data.objects.remove(o)
else:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=path)
meshes = [o for o in bpy.context.scene.objects if o.type == 'MESH']
if keep:
    for o in meshes:
        if o.name not in keep:
            o.hide_render = True
    meshes = [o for o in meshes if o.name in keep]
pts = [o.matrix_world @ Vector(c) for o in meshes for c in o.bound_box]
mn = Vector((min(p.x for p in pts), min(p.y for p in pts), min(p.z for p in pts)))
mx = Vector((max(p.x for p in pts), max(p.y for p in pts), max(p.z for p in pts)))
center = (mn + mx) / 2; size = (mx - mn).length
print("WORLD BBOX min", tuple(round(v, 2) for v in mn), "max", tuple(round(v, 2) for v in mx))
scene = bpy.context.scene
cam = bpy.data.objects.new("PreviewCam", bpy.data.cameras.new("PreviewCam"))
scene.collection.objects.link(cam)
cam.location = center + Vector((size * 0.9, -size * 1.1, size * 0.55))
d = center - cam.location
cam.rotation_euler = d.to_track_quat('-Z', 'Y').to_euler()
scene.camera = cam
sun = bpy.data.objects.new("PreviewSun", bpy.data.lights.new("PreviewSun", 'SUN'))
sun.data.energy = 4.0
sun.rotation_euler = (math.radians(50), 0, math.radians(30))
scene.collection.objects.link(sun)
if scene.world is None:
    scene.world = bpy.data.worlds.new("W")
scene.world.color = (0.35, 0.38, 0.42)
scene.render.engine = 'CYCLES'
scene.cycles.samples = 12
scene.cycles.use_denoising = False
scene.cycles.use_preview_denoising = False
for vl in scene.view_layers:
    vl.cycles.denoising_store_passes = False
scene.cycles.device = 'CPU'
scene.render.resolution_x = 420
scene.render.resolution_y = 420
scene.render.filepath = out
bpy.ops.render.render(write_still=True)
