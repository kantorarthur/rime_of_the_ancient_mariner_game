import os
import sys

out = sys.argv[1]
os.makedirs(out, exist_ok=True)

size = 0.035
pip = 0.0085
lift = 0.0006

vertices = []
faces = []


def quad(corners, material):
    first = len(vertices) + 1
    vertices.extend(corners)
    faces.append((material, [first, first + 1, first + 2, first + 3]))


def face_point(axis, sign, u, v, height):
    point = [0.0, 0.0, 0.0]
    others = [i for i in range(3) if i != axis]
    point[axis] = sign * height
    point[others[0]] = u
    point[others[1]] = v
    return point


def square(axis, sign, cu, cv, half, height, material):
    corners = [face_point(axis, sign, cu + du * half, cv + dv * half, height) for du, dv in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
    if sign < 0:
        corners.reverse()
    if axis == 1:
        corners.reverse()
    quad(corners, material)


PIPS = {
    1: [(0, 0)],
    2: [(-1, -1), (1, 1)],
    3: [(-1, -1), (0, 0), (1, 1)],
    4: [(-1, -1), (1, -1), (-1, 1), (1, 1)],
    5: [(-1, -1), (1, -1), (0, 0), (-1, 1), (1, 1)],
    6: [(-1, -1), (1, -1), (-1, 0), (1, 0), (-1, 1), (1, 1)],
}
SIDES = [(0, 1, 1), (0, -1, 6), (1, 1, 2), (1, -1, 5), (2, 1, 3), (2, -1, 4)]

for axis, sign, number in SIDES:
    square(axis, sign, 0.0, 0.0, size, size, "bone")
    for u, v in PIPS[number]:
        square(axis, sign, u * size * 0.5, v * size * 0.5, pip, size + lift, "pip")

with open(os.path.join(out, "dice.obj"), "w") as f:
    f.write("mtllib dice.mtl\n")
    for v in vertices:
        f.write("v %.5f %.5f %.5f\n" % tuple(v))
    current = None
    for material, corners in faces:
        if material != current:
            f.write("usemtl %s\n" % material)
            current = material
        f.write("f %d %d %d %d\n" % tuple(corners))

with open(os.path.join(out, "dice.mtl"), "w") as f:
    f.write("newmtl bone\nKd 0.86 0.82 0.70\nKs 0.30 0.30 0.30\n\n")
    f.write("newmtl pip\nKd 0.08 0.05 0.04\nKs 0.10 0.10 0.10\n")
