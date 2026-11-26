import bpy, bmesh, sys, os, json, math, random
from mathutils import Vector, noise

argv = sys.argv[sys.argv.index("--") + 1:]
cfg = json.loads(argv[0])
name = cfg["name"]
outdir = cfg["outdir"]
os.makedirs(outdir, exist_ok=True)


def clear_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)


def load_source(path):
    if path.endswith(".blend"):
        bpy.ops.wm.open_mainfile(filepath=path, load_ui=False)
    else:
        clear_scene()
        bpy.ops.import_scene.gltf(filepath=path)


def keep_meshes(names):
    for o in list(bpy.data.objects):
        if o.type != 'MESH' or (names and o.name not in names):
            if o.type == 'MESH' or o.type in ('CAMERA', 'LIGHT'):
                bpy.data.objects.remove(o, do_unlink=True)
    return [o for o in bpy.data.objects if o.type == 'MESH']


def bake_to_single(objs):
    bpy.ops.object.select_all(action='DESELECT')
    for o in objs:
        o.hide_set(False)
        o.select_set(True)
    bpy.context.view_layer.objects.active = objs[0]
    bpy.ops.object.parent_clear(type='CLEAR_KEEP_TRANSFORM')
    bpy.ops.object.convert(target='MESH')
    bpy.ops.object.make_single_user(object=True, obdata=True)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    if len(objs) > 1:
        bpy.ops.object.join()
    obj = bpy.context.view_layer.objects.active
    for o in list(bpy.data.objects):
        if o != obj:
            bpy.data.objects.remove(o, do_unlink=True)
    obj.name = name
    return obj


def normalize(obj, scale, rotate_z_deg=0.0):
    obj.rotation_euler = (0, 0, math.radians(rotate_z_deg))
    obj.scale = (scale, scale, scale)
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    pts = [obj.matrix_world @ v.co for v in obj.data.vertices]
    mn = Vector((min(p.x for p in pts), min(p.y for p in pts), min(p.z for p in pts)))
    mx = Vector((max(p.x for p in pts), max(p.y for p in pts), max(p.z for p in pts)))
    shift = Vector(((mn.x + mx.x) / 2, (mn.y + mx.y) / 2, mn.z + cfg.get("sink", 0.0)))
    for v in obj.data.vertices:
        v.co -= shift
    obj.data.update()
    return mx - mn


def decimate(obj, target_tris):
    obj.data.calc_loop_triangles()
    tris = len(obj.data.loop_triangles)
    if tris <= target_tris:
        return tris
    mod = obj.modifiers.new("dec", 'DECIMATE')
    mod.ratio = target_tris / tris
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier="dec")
    obj.data.calc_loop_triangles()
    return len(obj.data.loop_triangles)


def make_material(mat_name):
    mat = bpy.data.materials.get(mat_name) or bpy.data.materials.new(mat_name)
    return mat


def gen_rock(seed):
    clear_scene()
    random.seed(seed)
    bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=5, radius=1.0)
    obj = bpy.context.active_object
    off = Vector((random.uniform(0, 100), random.uniform(0, 100), random.uniform(0, 100)))
    sx, sy, sz = random.uniform(1.1, 1.6), random.uniform(0.9, 1.2), random.uniform(0.7, 1.0)
    for v in obj.data.vertices:
        p = v.co.copy()
        n = p.normalized()
        d = 0.35 * noise.noise(p * 1.2 + off) + 0.15 * noise.noise(p * 3.0 + off * 2) + 0.05 * noise.noise(p * 7.0 + off)
        flat = max(0.0, -p.z) * 0.6
        v.co = n * (1.0 + d) * Vector((sx, sy, sz)) - Vector((0, 0, flat))
    obj.data.update()
    obj.data.materials.append(make_material("rock"))
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=math.radians(66), island_margin=0.02)
    bpy.ops.object.mode_set(mode='OBJECT')
    obj.name = name
    return obj


def beach_weight(angle_deg):
    start = cfg.get("beach_start", 280.0)
    end = cfg.get("beach_end", 40.0)
    fade = cfg.get("beach_fade", 12.0)
    a = angle_deg % 360.0
    if start > end:
        inside_distance = min((a - start) % 360.0, (end - a) % 360.0)
        inside = ((a - start) % 360.0) <= ((end - start) % 360.0)
    else:
        inside_distance = min(a - start, end - a)
        inside = start <= a <= end
    if inside:
        return min(1.0, 0.5 + inside_distance / (2.0 * fade))
    outside_distance = min((start - a) % 360.0, (a - end) % 360.0)
    return max(0.0, 0.5 - outside_distance / (2.0 * fade))


def gen_island():
    clear_scene()
    size = cfg["size"]
    res = cfg["res"]
    bpy.ops.mesh.primitive_grid_add(x_subdivisions=res, y_subdivisions=res, size=size)
    obj = bpy.context.active_object
    radius = cfg["radius"]
    cliff_radius = cfg.get("cliff_radius", radius)
    top = cfg["top"]
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    for v in bm.verts:
        x, y = v.co.x, v.co.y
        gl_x, gl_z = x, -y
        angle = math.degrees(math.atan2(gl_x, gl_z)) % 360.0
        r = math.hypot(x, y)
        ang = math.atan2(y, x)
        beach = beach_weight(angle)

        edge = radius * (1.0 + 0.12 * noise.noise(Vector((math.cos(ang) * 2, math.sin(ang) * 2, 3.7))))
        t = r / edge
        if t < 0.75:
            slope_h = top
        elif t < 1.0:
            k = (t - 0.75) / 0.25
            slope_h = top * (1.0 - k * k * (3 - 2 * k))
        else:
            slope_h = -3.0 * min(1.0, (t - 1.0) * 4.0)

        wall_start = cliff_radius + cfg.get("wall_start_offset", 0.5)
        wall_end = cliff_radius + cfg.get("wall_end_offset", 1.5)
        rim_start = wall_start - cfg.get("rim_width", 0.0)
        rim_drop = cfg.get("rim_drop", 0.0)
        if r < rim_start:
            cliff_h = top
        elif r < wall_start:
            k = (r - rim_start) / (wall_start - rim_start)
            cliff_h = top - rim_drop * k * k * (3 - 2 * k)
        elif r < wall_end:
            k = (r - wall_start) / (wall_end - wall_start)
            cliff_h = (top - rim_drop) + (-3.0 - (top - rim_drop)) * k
        else:
            cliff_h = -3.0

        h = beach * slope_h + (1.0 - beach) * cliff_h
        on_land = h > 0.0
        edge_calm = 1.0
        if beach < 1.0:
            calm_start = cfg.get("calm_start", radius)
            if r > calm_start:
                edge_calm = max(0.15, 1.0 - (r - calm_start) / 2.0)
            edge_calm = beach + (1.0 - beach) * edge_calm
        h += 0.6 * noise.noise(Vector((x * 0.08, y * 0.08, 1.3))) * (1.0 if on_land else 0.3) * (0.4 + 0.6 * beach) * edge_calm
        h += 0.15 * noise.noise(Vector((x * 0.4, y * 0.4, 7.1))) * edge_calm
        v.co.z = h
    uv = bm.loops.layers.uv.verify()
    tile = cfg["uv_tile"]
    for f in bm.faces:
        for loop in f.loops:
            loop[uv].uv = (loop.vert.co.x / tile, loop.vert.co.y / tile)
    bm.to_mesh(obj.data)
    bm.free()
    for p in obj.data.polygons:
        p.use_smooth = True
    obj.data.materials.append(make_material("ground"))
    obj.name = name
    return obj


def gen_cliff_ring():
    load_source(cfg["src"])
    meshes = keep_meshes(cfg.get("keep"))
    source = bake_to_single(meshes)
    decimate(source, cfg["segment_tris"])
    radius = cfg["radius"]
    length_scale = cfg["length_scale"]
    height_scale = cfg["height_scale"]
    depth_scale = cfg["depth_scale"]
    base_y = cfg["base_y"]
    start = cfg["start_angle"]
    end = cfg["end_angle"]
    count = cfg["segments"]
    taper = cfg.get("taper", 0.15)

    xs = [v.co.x for v in source.data.vertices]
    zs = [v.co.z for v in source.data.vertices]
    x_min, x_max = min(xs), max(xs)
    z_min = min(zs)
    full_height = (max(zs) - z_min) * height_scale
    span = x_max - x_min
    arc_degrees = math.degrees(span * length_scale / radius)
    total = (end - start) % 360.0
    step = (total - arc_degrees) / max(1, count - 1)

    parts = []
    for i in range(count):
        part = source.copy()
        part.data = source.data.copy()
        bpy.context.collection.objects.link(part)
        center = start + arc_degrees / 2.0 + i * step
        for v in part.data.vertices:
            u = (v.co.x - x_min) / span
            phi = math.radians(center) + (v.co.x - (x_min + x_max) / 2.0) * length_scale / radius
            first = i == 0
            last = i == count - 1
            fade = 1.0
            if first and u < taper:
                k = u / taper
                fade = k * k * (3 - 2 * k)
            if last and u > 1.0 - taper:
                k = (1.0 - u) / taper
                fade = k * k * (3 - 2 * k)
            rho = radius - v.co.y * depth_scale
            height = (v.co.z - z_min) * height_scale
            sink = (1.0 - fade) * (full_height + 1.0)
            v.co = Vector((rho * math.sin(phi), -rho * math.cos(phi), base_y + height - sink))
        part.data.update()
        parts.append(part)

    bpy.data.objects.remove(source, do_unlink=True)
    bpy.ops.object.select_all(action='DESELECT')
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    obj = bpy.context.view_layer.objects.active
    clip = cfg.get("clip_height")
    if clip is not None:
        bm = bmesh.new()
        bm.from_mesh(obj.data)
        doomed = [f for f in bm.faces if max(v.co.z for v in f.verts) > clip]
        bmesh.ops.delete(bm, geom=doomed, context='FACES')
        bm.to_mesh(obj.data)
        bm.free()
        obj.data.update()
    if obj.data.has_custom_normals:
        bpy.ops.mesh.customdata_custom_splitnormals_clear()
    for p in obj.data.polygons:
        p.use_smooth = True
    obj.name = name
    return obj


kind = cfg.get("kind", "import")
if kind == "rock":
    obj = gen_rock(cfg["seed"])
elif kind == "island":
    obj = gen_island()
elif kind == "cliff_ring":
    obj = gen_cliff_ring()
else:
    load_source(cfg["src"])
    meshes = keep_meshes(cfg.get("keep"))
    for prefix in cfg.get("exclude_prefix", []):
        for o in [m for m in meshes if m.name.startswith(prefix)]:
            bpy.data.objects.remove(o, do_unlink=True)
        meshes = [m for m in bpy.data.objects if m.type == 'MESH']
    obj = bake_to_single(meshes)

if kind not in ("island", "cliff_ring"):
    dims = normalize(obj, cfg.get("scale", 1.0), cfg.get("rotate", 0.0))
else:
    dims = Vector(obj.dimensions)

tris = decimate(obj, cfg.get("max_tris", 10 ** 9))

if cfg.get("smooth", False):
    for p in obj.data.polygons:
        p.use_smooth = True

for img in bpy.data.images:
    if img.packed_file and cfg.get("unpack_images"):
        target = os.path.join(outdir, "_packed_" + img.name + ".png")
        img.filepath_raw = target
        img.file_format = 'PNG'
        img.save()

bpy.ops.object.select_all(action='DESELECT')
obj.select_set(True)
bpy.context.view_layer.objects.active = obj
objpath = os.path.join(outdir, name + ".obj")
bpy.ops.wm.obj_export(
    filepath=objpath,
    export_selected_objects=True,
    apply_modifiers=True,
    export_triangulated_mesh=True,
    export_normals=True,
    export_uv=True,
    export_materials=True,
    path_mode='STRIP',
    forward_axis='NEGATIVE_Z',
    up_axis='Y')

mats = [s.material.name for s in obj.material_slots if s.material]
print("RESULT", json.dumps({"name": name, "tris": tris, "dims": [round(dims.x, 2), round(dims.y, 2), round(dims.z, 2)], "materials": mats}))
