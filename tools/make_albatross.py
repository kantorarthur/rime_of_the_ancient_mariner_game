import bpy
import sys
import json
import os
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:]
cfg = json.loads(argv[0])
outdir = cfg["outdir"]
os.makedirs(outdir, exist_ok=True)

bpy.ops.wm.open_mainfile(filepath=cfg["src"], load_ui=False)

bird = bpy.data.objects["Seagull"]
for o in list(bpy.data.objects):
    if o.type in ('CAMERA', 'LIGHT') or (o.type == 'MESH' and o != bird):
        bpy.data.objects.remove(o, do_unlink=True)

for mod in bird.modifiers:
    if mod.type == 'SUBSURF':
        mod.levels = cfg.get("subdivisions", 2)
        mod.render_levels = cfg.get("subdivisions", 2)

depsgraph = bpy.context.evaluated_depsgraph_get()
evaluated = bird.evaluated_get(depsgraph)
mesh = bpy.data.meshes.new_from_object(evaluated, preserve_all_data_layers=True, depsgraph=depsgraph)
albatross = bpy.data.objects.new("albatross", mesh)
albatross.matrix_world = bird.matrix_world.copy()
bpy.context.collection.objects.link(albatross)
for o in list(bpy.data.objects):
    if o != albatross:
        bpy.data.objects.remove(o, do_unlink=True)

bpy.context.view_layer.objects.active = albatross
albatross.select_set(True)
bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)

colors = albatross.data.color_attributes["Color"]
half_span = max(abs(v.co.x) for v in albatross.data.vertices)
dark = cfg.get("wing_tip_color", [0.12, 0.12, 0.13])
start = cfg.get("dark_start", 0.45)
for index, item in enumerate(colors.data):
    if colors.domain == 'CORNER':
        vertex = albatross.data.loops[index].vertex_index
    else:
        vertex = index
    x = abs(albatross.data.vertices[vertex].co.x) / half_span
    k = min(1.0, max(0.0, (x - start) / (1.0 - start)))
    k = k * k * (3.0 - 2.0 * k)
    c = item.color
    item.color = (c[0] * (1 - k) + dark[0] * k, c[1] * (1 - k) + dark[1] * k, c[2] * (1 - k) + dark[2] * k, 1.0)

original_uv = albatross.data.uv_layers[0]
original_uv.name = "FeatherUV"
bake_uv = albatross.data.uv_layers.new(name="BakeUV")
albatross.data.uv_layers.active = bake_uv
bake_uv.active_render = True
bpy.ops.object.mode_set(mode='EDIT')
bpy.ops.mesh.select_all(action='SELECT')
bpy.ops.uv.smart_project(angle_limit=1.15, island_margin=0.02)
bpy.ops.object.mode_set(mode='OBJECT')

feathers = None
for img in bpy.data.images:
    if "feather" in img.name:
        feathers = img

size = cfg.get("texture_size", 1024)
target = bpy.data.images.new("albatross_diff", size, size, alpha=False)

material = bpy.data.materials.new("albatross")
material.use_nodes = True
nodes = material.node_tree.nodes
links = material.node_tree.links
nodes.clear()
output = nodes.new("ShaderNodeOutputMaterial")
emission = nodes.new("ShaderNodeEmission")
attribute = nodes.new("ShaderNodeVertexColor")
attribute.layer_name = "Color"
multiply = nodes.new("ShaderNodeMix")
multiply.data_type = 'RGBA'
multiply.blend_type = 'MULTIPLY'
multiply.inputs["Factor"].default_value = cfg.get("feather_strength", 0.35)
links.new(attribute.outputs["Color"], multiply.inputs[6])
if feathers is not None:
    feather_node = nodes.new("ShaderNodeTexImage")
    feather_node.image = feathers
    feather_uv = nodes.new("ShaderNodeUVMap")
    feather_uv.uv_map = "FeatherUV"
    links.new(feather_uv.outputs["UV"], feather_node.inputs["Vector"])
    links.new(feather_node.outputs["Color"], multiply.inputs[7])
else:
    multiply.inputs[7].default_value = (1.0, 1.0, 1.0, 1.0)
links.new(multiply.outputs[2], emission.inputs["Color"])
links.new(emission.outputs["Emission"], output.inputs["Surface"])
bake_node = nodes.new("ShaderNodeTexImage")
bake_node.image = target
nodes.active = bake_node

albatross.data.materials.clear()
albatross.data.materials.append(material)

scene = bpy.context.scene
scene.render.engine = 'CYCLES'
scene.cycles.device = 'CPU'
scene.cycles.samples = 4
scene.render.bake.margin = 8
bpy.ops.object.bake(type='EMIT')
target.filepath_raw = os.path.join(outdir, "albatross_diff.png")
target.file_format = 'PNG'
target.save()

span = cfg.get("wingspan", 3.0)
current = albatross.dimensions.x
albatross.scale = (span / current,) * 3
bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
points = [v.co for v in albatross.data.vertices]
center = Vector((
    (min(p.x for p in points) + max(p.x for p in points)) / 2,
    (min(p.y for p in points) + max(p.y for p in points)) / 2,
    (min(p.z for p in points) + max(p.z for p in points)) / 2))
for v in albatross.data.vertices:
    v.co -= center
albatross.data.update()
for p in albatross.data.polygons:
    p.use_smooth = True
albatross.data.uv_layers.remove(albatross.data.uv_layers["FeatherUV"])

bpy.ops.wm.obj_export(
    filepath=os.path.join(outdir, "albatross.obj"),
    export_selected_objects=True,
    export_triangulated_mesh=True,
    export_normals=True,
    export_uv=True,
    export_materials=True,
    export_colors=False,
    path_mode='STRIP',
    forward_axis='NEGATIVE_Z',
    up_axis='Y')

albatross.data.calc_loop_triangles()
d = albatross.dimensions
print("RESULT", json.dumps({"tris": len(albatross.data.loop_triangles), "dims": [round(d.x, 2), round(d.y, 2), round(d.z, 2)], "feathers": feathers is not None}))
