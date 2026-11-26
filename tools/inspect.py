import bpy, sys
argv = sys.argv[sys.argv.index("--") + 1:]
path = argv[0]
if path.endswith(".blend"):
    bpy.ops.wm.open_mainfile(filepath=path, load_ui=False)
else:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=path)
dg = bpy.context.evaluated_depsgraph_get()
total = 0
for o in bpy.data.objects:
    if o.type != 'MESH':
        print("  [%s] %s" % (o.type, o.name)); continue
    e = o.evaluated_get(dg); m = e.to_mesh()
    m.calc_loop_triangles(); t = len(m.loop_triangles); total += t
    d = o.dimensions
    mats = [s.material.name if s.material else None for s in o.material_slots]
    print("  MESH %-40s tris=%7d dim=(%.2f, %.2f, %.2f) mats=%s parent=%s" % (o.name, t, d.x, d.y, d.z, mats, o.parent.name if o.parent else None))
    e.to_mesh_clear()
print("  TOTAL tris:", total)
