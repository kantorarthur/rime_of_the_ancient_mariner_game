import bpy, sys, numpy as np
argv = sys.argv[sys.argv.index("--") + 1:]
src, dst, size, channel = argv[0], argv[1], int(argv[2]), int(argv[3])
img = bpy.data.images.load(src)
img.scale(size, size)
px = np.empty(size * size * 4, dtype=np.float32)
img.pixels.foreach_get(px)
px = px.reshape(-1, 4)
rough = np.clip(px[:, channel], 0.0, 1.0)
spec = np.clip((1.0 - rough) ** 2 * 0.9, 0.0, 1.0)
out = bpy.data.images.new("spec", size, size, alpha=False, float_buffer=False)
out.colorspace_settings.name = 'Non-Color'
res = np.ones((size * size, 4), dtype=np.float32)
res[:, 0] = spec; res[:, 1] = spec; res[:, 2] = spec
out.pixels.foreach_set(res.ravel())
out.filepath_raw = dst
out.file_format = 'PNG'
out.save()
print("SPEC", dst, "mean", float(spec.mean()))
