import math
import os
import sys

out = sys.argv[1]
os.makedirs(out, exist_ok=True)

vertices = []
faces = []


def beam(start, end, width, height, material):
    direction = [end[i] - start[i] for i in range(3)]
    length = math.sqrt(sum(d * d for d in direction))
    forward = [d / length for d in direction]
    up = [0.0, 1.0, 0.0] if abs(forward[1]) < 0.9 else [1.0, 0.0, 0.0]
    side = [forward[1] * up[2] - forward[2] * up[1], forward[2] * up[0] - forward[0] * up[2], forward[0] * up[1] - forward[1] * up[0]]
    norm = math.sqrt(sum(s * s for s in side))
    side = [s / norm for s in side]
    top = [side[1] * forward[2] - side[2] * forward[1], side[2] * forward[0] - side[0] * forward[2], side[0] * forward[1] - side[1] * forward[0]]

    first = len(vertices) + 1
    for point in (start, end):
        for sx, sy in ((-1, -1), (1, -1), (1, 1), (-1, 1)):
            vertices.append([point[i] + side[i] * sx * width * 0.5 + top[i] * sy * height * 0.5 for i in range(3)])

    quads = [(0, 1, 2, 3), (7, 6, 5, 4), (0, 4, 5, 1), (1, 5, 6, 2), (2, 6, 7, 3), (3, 7, 4, 0)]
    for quad in quads:
        faces.append((material, [first + index for index in quad]))


beam((0.0, 0.0, 0.18), (0.0, 0.0, -0.62), 0.05, 0.06, "wood")
beam((0.0, -0.03, 0.0), (0.0, -0.11, 0.06), 0.03, 0.025, "wood")
beam((0.0, 0.035, -0.22), (0.0, 0.035, -0.66), 0.012, 0.012, "bolt")

segments = 8
previous = None
tips = []
for i in range(segments + 1):
    t = i / segments * 2.0 - 1.0
    point = (t * 0.36, 0.0, -0.56 + 0.1 * t * t)
    if previous:
        beam(previous, point, 0.035, 0.022, "iron")
    previous = point
    if i == 0 or i == segments:
        tips.append(point)

beam(tips[0], (0.0, 0.02, -0.24), 0.006, 0.006, "string")
beam(tips[1], (0.0, 0.02, -0.24), 0.006, 0.006, "string")

with open(os.path.join(out, "crossbow.obj"), "w") as f:
    f.write("mtllib crossbow.mtl\n")
    for v in vertices:
        f.write("v %.4f %.4f %.4f\n" % tuple(v))
    current = None
    for material, quad in faces:
        if material != current:
            f.write("usemtl %s\n" % material)
            current = material
        f.write("f %d %d %d %d\n" % tuple(quad))

with open(os.path.join(out, "crossbow.mtl"), "w") as f:
    f.write("newmtl wood\nKd 0.32 0.19 0.09\nKs 0.10 0.10 0.10\n\n")
    f.write("newmtl iron\nKd 0.18 0.18 0.19\nKs 0.60 0.60 0.60\n\n")
    f.write("newmtl bolt\nKd 0.45 0.36 0.24\nKs 0.10 0.10 0.10\n\n")
    f.write("newmtl string\nKd 0.75 0.72 0.62\nKs 0.05 0.05 0.05\n")
