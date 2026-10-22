#!/usr/bin/env python3
"""Render an isometric floor view of a Quake 1 BSP map to PNG.

Reads the world model's faces straight from the BSP (version 29), keeps the
upward-facing surfaces (floors, stairs, ledges), projects them isometrically
and colours them by height in a Quake-style palette. Liquids are drawn blue,
sky and tool brushes are skipped.

    python3 tools/render_map.py quake/maps/bsp/aerowalk.bsp out.png "Aerowalk"
"""
import math
import struct
import sys

from PIL import Image, ImageDraw, ImageFilter, ImageFont

W, H, SS = 960, 600, 2  # output size and supersampling factor
BG_TOP, BG_BOTTOM = (28, 20, 16), (10, 8, 7)
LOW, MID, HIGH = (110, 40, 20), (205, 105, 35), (255, 214, 120)
LIQUID = (40, 110, 150)


def lumps(data):
    version = struct.unpack_from("<i", data, 0)[0]
    if version != 29:
        raise SystemExit(f"not a Quake 1 BSP (version {version})")
    return [struct.unpack_from("<ii", data, 4 + 8 * i) for i in range(15)]


def texture_names(data, ofs, length):
    count = struct.unpack_from("<i", data, ofs)[0]
    names = []
    for i in range(count):
        mofs = struct.unpack_from("<i", data, ofs + 4 + 4 * i)[0]
        if mofs < 0:
            names.append("")
            continue
        raw = data[ofs + mofs: ofs + mofs + 16]
        names.append(raw.split(b"\0", 1)[0].decode("latin-1").lower())
    return names


def load_floors(path):
    data = open(path, "rb").read()
    L = lumps(data)
    planes = [struct.unpack_from("<4fi", data, L[1][0] + 20 * i) for i in range(L[1][1] // 20)]
    verts = [struct.unpack_from("<3f", data, L[3][0] + 12 * i) for i in range(L[3][1] // 12)]
    texinfo_tex = [struct.unpack_from("<i", data, L[6][0] + 40 * i + 32)[0] for i in range(L[6][1] // 40)]
    names = texture_names(data, *L[2])
    edges = [struct.unpack_from("<2H", data, L[12][0] + 4 * i) for i in range(L[12][1] // 4)]
    surfedges = struct.unpack_from(f"<{L[13][1] // 4}i", data, L[13][0])
    first_face, num_faces = struct.unpack_from("<2i", data, L[14][0] + 56)  # model 0 = world

    floors = []
    for f in range(first_face, first_face + num_faces):
        planenum, side, firstedge, numedges, texinfo = struct.unpack_from("<HHiHH", data, L[7][0] + 20 * f)
        name = names[texinfo_tex[texinfo]] if texinfo_tex[texinfo] < len(names) else ""
        if name.startswith(("sky", "trigger", "clip", "hint", "skip")):
            continue
        nz = planes[planenum][2] * (-1 if side else 1)
        liquid = name.startswith("*")
        if nz < 0.7 and not liquid:
            continue
        poly = []
        for e in surfedges[firstedge: firstedge + numedges]:
            a, b = edges[abs(e)]
            poly.append(verts[a if e >= 0 else b])
        floors.append((poly, liquid))
    return floors


def lerp(c1, c2, t):
    return tuple(int(a + (b - a) * t) for a, b in zip(c1, c2))


def height_colour(t):
    return lerp(LOW, MID, t * 2) if t < 0.5 else lerp(MID, HIGH, (t - 0.5) * 2)


def render(path, out, title=""):
    floors = load_floors(path)
    yaw, pitch = math.radians(45), math.radians(35)
    cy, sy, cp, sp = math.cos(yaw), math.sin(yaw), math.cos(pitch), math.sin(pitch)

    def project(v):
        x, y, z = v
        rx, ry = x * cy - y * sy, x * sy + y * cy
        return rx, -(ry * sp) - z * cp, ry * cp - z * sp  # screen x, screen y, depth

    zs = [v[2] for poly, _ in floors for v in poly]
    zmin, zmax = min(zs), max(zs)
    items = []
    for poly, liquid in floors:
        pts = [project(v) for v in poly]
        depth = sum(p[2] for p in pts) / len(pts)
        z = sum(v[2] for v in poly) / len(poly)
        items.append((depth, [(p[0], p[1]) for p in pts], z, liquid))

    xs = [x for _, pts, _, _ in items for x, _ in pts]
    ys = [y for _, pts, _, _ in items for _, y in pts]
    margin = 0.08
    scale = min(W * (1 - 2 * margin) / (max(xs) - min(xs)),
                H * (1 - 2 * margin - 0.08) / (max(ys) - min(ys))) * SS
    ox = (W * SS - (max(xs) - min(xs)) * scale) / 2 - min(xs) * scale
    oy = (H * SS - (max(ys) - min(ys)) * scale) / 2 - min(ys) * scale + H * SS * 0.03

    img = Image.new("RGB", (W * SS, H * SS))
    bg = ImageDraw.Draw(img)
    for row in range(H * SS):
        bg.line([(0, row), (W * SS, row)], fill=lerp(BG_TOP, BG_BOTTOM, row / (H * SS)))

    # soft shadow under the whole layout
    shadow = Image.new("L", img.size, 0)
    sd = ImageDraw.Draw(shadow)
    for _, pts, _, _ in items:
        sd.polygon([(x * scale + ox + 14 * SS, y * scale + oy + 22 * SS) for x, y in pts], fill=150)
    shadow = shadow.filter(ImageFilter.GaussianBlur(12 * SS))
    img.paste((0, 0, 0), mask=shadow)

    draw = ImageDraw.Draw(img)
    for depth, pts, z, liquid in sorted(items, key=lambda i: -i[0]):
        t = (z - zmin) / ((zmax - zmin) or 1)
        fill = LIQUID if liquid else height_colour(t)
        draw.polygon([(x * scale + ox, y * scale + oy) for x, y in pts],
                     fill=fill, outline=lerp(fill, (0, 0, 0), 0.45))

    img = img.resize((W, H), Image.LANCZOS)
    if title:
        d = ImageDraw.Draw(img)
        try:
            font = ImageFont.truetype("DejaVuSans-Bold.ttf", 30)
            small = ImageFont.truetype("DejaVuSans.ttf", 16)
        except OSError:
            font = small = ImageFont.load_default()
        d.text((28, 22), title, font=font, fill=(255, 200, 110))
        d.text((30, 60), path.rsplit("/", 1)[-1], font=small, fill=(170, 140, 110))
    img.save(out, optimize=True)


if __name__ == "__main__":
    render(sys.argv[1], sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else "")
