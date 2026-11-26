import bpy
import sys
import json
import os
import numpy as np

argv = sys.argv[sys.argv.index("--") + 1:]
cfg = json.loads(argv[0])

image = bpy.data.images.load(cfg["src"])
width, height = image.size
pixels = np.empty(width * height * 4, dtype=np.float32)
image.pixels.foreach_get(pixels)
hdr = pixels.reshape(height, width, 4)[::-1, :, :3]


def sample(directions):
    x, y, z = directions[..., 0], directions[..., 1], directions[..., 2]
    u = 0.5 + np.arctan2(x, -z) / (2.0 * np.pi) + cfg.get("rotate", 0.0) / 360.0
    v = 0.5 - np.arcsin(np.clip(y, -1.0, 1.0)) / np.pi
    px = (np.mod(u, 1.0) * (width - 1)).astype(np.int64)
    py = (np.clip(v, 0.0, 1.0) * (height - 1)).astype(np.int64)
    return hdr[py, px]


def face_directions(face, size):
    t, s = np.meshgrid(np.linspace(-1.0, 1.0, size), np.linspace(-1.0, 1.0, size), indexing="ij")
    one = np.ones_like(s)
    table = {
        "right": (one, -t, -s),
        "left": (-one, -t, s),
        "top": (s, one, t),
        "bottom": (s, -one, -t),
        "front": (s, -t, one),
        "back": (-s, -t, -one),
    }
    d = np.stack(table[face], axis=-1)
    return d / np.linalg.norm(d, axis=-1, keepdims=True)


if cfg.get("preview"):
    sky = hdr[: height // 2]
    exposure = cfg.get("exposure", 1.0) / max(np.percentile(sky, 99.0), 1e-6)
    ldr = hdr * exposure
    ldr = ldr / (1.0 + ldr)
    ldr = np.clip(ldr, 0.0, 1.0) ** (1.0 / 2.2)
    small = ldr[::8, ::8]
    out = bpy.data.images.new("preview", small.shape[1], small.shape[0], alpha=False)
    rgba = np.concatenate([small[::-1], np.ones(small.shape[:2] + (1,), dtype=np.float32)], axis=-1)
    out.pixels.foreach_set(rgba.astype(np.float32).ravel())
    out.filepath_raw = cfg["preview"]
    out.file_format = 'PNG'
    out.save()
    print("PREVIEW", cfg["preview"])
    sys.exit(0)

size = cfg["size"]
outdir = cfg["outdir"]
os.makedirs(outdir, exist_ok=True)

upper = sample(face_directions("front", 256))
reference = np.percentile(np.concatenate([sample(face_directions(f, 256)).reshape(-1, 3) for f in ("front", "back", "left", "right", "top")]), 99.0)
exposure = cfg.get("exposure", 1.0) / max(reference, 1e-6)
horizon = np.array(cfg.get("horizon_color_srgb", [0.06, 0.08, 0.11]), dtype=np.float32) ** 2.2
saturation = cfg.get("saturation", 1.0)
tint = np.array(cfg.get("tint", [1.0, 1.0, 1.0]), dtype=np.float32)
blend_low_near = np.radians(cfg.get("horizon_low_near_moon", cfg.get("horizon_low", 0.0)))
blend_low_far = np.radians(cfg.get("horizon_low", 0.0))
blend_high_near = np.radians(cfg.get("horizon_high_near_moon", 8.0))
blend_high_far = np.radians(cfg.get("horizon_high", 8.0))
moon_width = np.radians(cfg.get("moon_width", 35.0))

luminance = hdr[: height // 2].sum(axis=-1)
moon_row, moon_col = np.unravel_index(np.argmax(luminance), luminance.shape)
moon_u = moon_col / (width - 1) - cfg.get("rotate", 0.0) / 360.0
moon_azimuth = (moon_u - 0.5) * 2.0 * np.pi
moon_elevation = (0.5 - moon_row / (height - 1)) * np.pi
moon_direction = [float(np.sin(moon_azimuth) * np.cos(moon_elevation)), float(np.sin(moon_elevation)), float(-np.cos(moon_azimuth) * np.cos(moon_elevation))]
print("MOON", json.dumps([round(c, 3) for c in moon_direction]), "elevation", round(float(np.degrees(moon_elevation)), 1))

for face in ("right", "left", "top", "bottom", "front", "back"):
    d = face_directions(face, size)
    color = sample(d) * exposure
    color = color / (1.0 + color)
    gray = color.mean(axis=-1, keepdims=True)
    color = gray + (color - gray) * saturation
    color = color * tint
    elevation = np.arcsin(np.clip(d[..., 1], -1.0, 1.0))
    azimuth = np.arctan2(d[..., 0], -d[..., 2])
    distance = np.abs(np.angle(np.exp(1j * (azimuth - moon_azimuth))))
    near = np.exp(-(distance / moon_width) ** 2)
    blend_high = blend_high_far + (blend_high_near - blend_high_far) * near
    blend_low = blend_low_far + (blend_low_near - blend_low_far) * near
    k = np.clip((elevation - blend_low) / (blend_high - blend_low), 0.0, 1.0)
    k = k * k * (3.0 - 2.0 * k)
    color = color * k[..., None] + horizon * (1.0 - k[..., None])
    color = np.clip(color, 0.0, 1.0) ** (1.0 / 2.2)
    out = bpy.data.images.new(face, size, size, alpha=False)
    rgba = np.concatenate([color[::-1], np.ones((size, size, 1), dtype=np.float32)], axis=-1)
    out.pixels.foreach_set(rgba.astype(np.float32).ravel())
    out.filepath_raw = os.path.join(outdir, face + ".png")
    out.file_format = 'PNG'
    out.save()
    print("FACE", face)
