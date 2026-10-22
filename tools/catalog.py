#!/usr/bin/env python3
"""Generate the archive catalog for DMQ-Server-files.

Outputs (inside the repo):
  catalog/maps.csv              every map: loose .bsp files and maps inside archives
  catalog/archive-contents.csv  every entry inside every .zip / .rar / .7z / .tgz
  CATALOG.md                    human-readable overview per folder

Map titles come from the BSP entity lump (worldspawn "message"), so they are
the names players saw in-game, not just file names.
"""
import csv
import json
import os
import re
import struct
import subprocess
import sys
import tarfile
import zipfile
from concurrent.futures import ThreadPoolExecutor

REPO = sys.argv[1] if len(sys.argv) > 1 else "/mnt/d/Quake/DMQ-Server-files"
CACHE = os.path.expanduser("~/.cache/dmq-catalog-cache.json")
SKIP_DIRS = {".git", ".cache", "catalog"}
ARCHIVE_EXT = (".zip", ".rar", ".7z", ".tgz", ".tar.gz", ".pk3")
TITLE_RE = re.compile(r"^\s*title\s*:\s*(.+?)\s*$", re.I | re.M)
AUTHOR_RE = re.compile(r"^\s*author\s*:\s*(.+?)\s*$", re.I | re.M)


def bsp_info(data):
    """Return (format, title) from raw BSP bytes; title may be ''."""
    if len(data) < 8:
        return "unknown", ""
    magic = data[:4]
    if magic in (b"BSP2", b"2PSB"):
        fmt, off = "Quake BSP2", 4
    elif magic == b"IBSP":
        ver = struct.unpack_from("<i", data, 4)[0]
        fmt = {38: "Quake 2", 46: "Quake 3", 47: "Quake Live"}.get(ver, f"IBSP v{ver}")
        off = 8
    else:
        ver = struct.unpack_from("<i", data, 0)[0]
        fmt = {29: "Quake 1", 30: "Half-Life/DMC"}.get(ver, f"BSP v{ver}")
        if ver not in (29, 30):
            return fmt, ""
        off = 4
    ent_off, ent_len = struct.unpack_from("<ii", data, off)
    if ent_off <= 0 or ent_len <= 0 or ent_off + ent_len > len(data):
        return fmt, ""
    ents = data[ent_off:ent_off + min(ent_len, 65536)].decode("latin-1", "replace")
    first = ents.split("}", 1)[0]
    m = re.search(r'"message"\s+"([^"]*)"', first)
    title = m.group(1) if m else ""
    # Quake uses \n escapes and high-bit "gold" chars in titles
    title = "".join(chr(ord(c) & 0x7F) if ord(c) > 127 else c for c in title)
    return fmt, " ".join(title.replace("\\n", " ").split())


def read_head(path, n=1 << 20):
    with open(path, "rb") as f:
        head = f.read(n)
    return head


def bsp_from_file(path):
    data = read_head(path)
    fmt, title = bsp_info(data)
    if not title and fmt in ("Quake 1", "Half-Life/DMC", "Quake BSP2") and len(data) == 1 << 20:
        with open(path, "rb") as f:  # entity lump beyond the first MiB
            fmt, title = bsp_info(f.read())
    return fmt, title


def list_archive(path):
    """Yield (name, size, reader) for archive entries; reader() -> bytes or None."""
    low = path.lower()
    try:
        if low.endswith((".zip", ".pk3")):
            with zipfile.ZipFile(path) as z:
                for i in z.infolist():
                    if not i.is_dir():
                        yield i.filename, i.file_size, (lambda i=i, z=z: z.read(i))
            return
        if low.endswith((".tgz", ".tar.gz")):
            with tarfile.open(path) as t:
                for m in t.getmembers():
                    if m.isfile():
                        yield m.name, m.size, (lambda m=m, t=t: t.extractfile(m).read())
            return
        out = subprocess.run(["7z", "l", "-slt", "-ba", path], capture_output=True, text=True,
                             errors="replace", timeout=300).stdout
        for block in out.split("\n\n"):
            kv = dict(l.split(" = ", 1) for l in block.splitlines() if " = " in l)
            if kv.get("Path") and kv.get("Folder", "-") != "+":
                yield kv["Path"], int(kv.get("Size") or 0), None
    except Exception as e:  # corrupt or exotic archive: record and continue
        yield f"<unreadable: {type(e).__name__}>", 0, None


def info_txt(path):
    """Title/author from a sidecar .txt next to a map or archive."""
    base = os.path.splitext(path)[0]
    for ext in (".txt", ".TXT"):
        if os.path.exists(base + ext):
            text = open(base + ext, "rb").read(65536).decode("latin-1", "replace")
            t, a = TITLE_RE.search(text), AUTHOR_RE.search(text)
            return (t.group(1) if t else ""), (a.group(1) if a else "")
    return "", ""


def clean(value):
    """Strip control characters so CSV/Markdown output stays well-formed."""
    if not isinstance(value, str):
        return value
    return "".join(c for c in value if c >= " " and c != "\x7f").strip()


def is_junk(entry):
    return "__MACOSX/" in entry or os.path.basename(entry).startswith("._")


def scan_file(job):
    rel, size = job
    path = os.path.join(REPO, rel)
    name = os.path.basename(rel)
    maps, contents = [], []
    if name.lower().endswith(".bsp"):
        fmt, title = bsp_from_file(path)
        t, a = info_txt(path)
        maps.append([name, rel, "", fmt, title or t, a, size // 1024])
    else:
        t, a = info_txt(path)
        for entry, esize, reader in list_archive(path):
            if is_junk(entry):
                continue
            contents.append([rel, entry, esize])
            if entry.lower().endswith(".bsp"):
                fmt, title = "", ""
                if reader:
                    try:
                        fmt, title = bsp_info(reader())
                    except Exception:
                        pass
                maps.append([os.path.basename(entry), rel, entry, fmt, title or t, a, esize // 1024])
    return f"{rel}|{size}", (maps, contents)


def write_browsable(maps, contents):
    """Small per-letter / per-folder files that GitHub renders as tables.

    GitHub shows no table for CSV files this large, so the full CSVs are
    split: catalog/maps/<letter>.csv and catalog/archives/<folder>.md.
    """
    header = ["map_file", "location", "path_inside_archive", "format", "title", "author", "size_kb"]
    by_letter = {}
    for r in sorted(maps, key=lambda r: (r[0].lower(), r[1])):
        c = r[0][:1].lower()
        by_letter.setdefault(c if c.isalpha() else "0-9", []).append(r)
    os.makedirs(os.path.join(REPO, "catalog/maps"), exist_ok=True)
    for letter, rows in by_letter.items():
        with open(os.path.join(REPO, f"catalog/maps/{letter}.csv"), "w", newline="", encoding="utf-8") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(header)
            w.writerows(rows)

    by_archive = {}
    for archive, entry, size in contents:
        by_archive.setdefault(archive, []).append((entry, size))
    by_folder = {}
    for archive in by_archive:
        by_folder.setdefault(os.path.dirname(archive), []).append(archive)
    os.makedirs(os.path.join(REPO, "catalog/archives"), exist_ok=True)
    keep = {f"{folder.replace('/', '--').replace(' ', '_')}.md" for folder in by_folder}
    for stale in set(os.listdir(os.path.join(REPO, "catalog/archives"))) - keep:
        os.remove(os.path.join(REPO, "catalog/archives", stale))  # generated page for a folder now gone
    pages = []
    for folder, archives in sorted(by_folder.items()):
        slug = folder.replace("/", "--").replace(" ", "_")
        pages.append((folder, slug, len(archives)))
        with open(os.path.join(REPO, f"catalog/archives/{slug}.md"), "w", newline="\n", encoding="utf-8") as f:
            f.write(f"# Inside the archives in `{folder}/`\n\n{len(archives)} archives. Click one to see its files.\n\n")
            for archive in sorted(archives, key=str.lower):
                entries = by_archive[archive]
                name = os.path.basename(archive)
                f.write(f"<details>\n<summary><code>{name}</code> ({len(entries)} files)</summary>\n\n")
                if name == "deathmatch-maps.zip":
                    f.write("Same 5 330 files as `quake/maps/single-map-zips/` (identical names and sizes); "
                            "see [the map list](../maps/) instead.\n\n</details>\n\n")
                    continue
                for entry, size in entries:
                    kb = f"{int(size) / 1024:.0f} KB" if size else ""
                    f.write(f"- `{entry}` {kb}\n")
                f.write("\n</details>\n\n")
    with open(os.path.join(REPO, "catalog/README.md"), "w", newline="\n", encoding="utf-8") as f:
        f.write("# Catalog\n\n## Maps by first letter\n\n")
        f.write(" · ".join(f"[{l.upper()}](maps/{l}.csv)" for l in sorted(by_letter)) + "\n\n")
        f.write("Columns: map file, where it is, path inside the archive, engine, in-game title, author, size.\n\n")
        f.write("## Inside the archives, per folder\n\n| Folder | Archives |\n|---|---:|\n")
        for folder, slug, n in pages:
            f.write(f"| [`{folder}/`](archives/{slug}.md) | {n} |\n")
        f.write("\n## Full lists (download)\n\n- [maps.csv](maps.csv): every map in one file\n"
                "- [archive-contents.csv](archive-contents.csv): every archive entry in one file\n")


def private_paths():
    """Paths listed in tools/PRIVATE.txt are not in the public dataset."""
    path = os.path.join(REPO, "tools/PRIVATE.txt")
    if not os.path.exists(path):
        return set()
    with open(path, encoding="utf-8") as f:
        return {l.strip() for l in f if l.strip() and not l.startswith("#")}


def main():
    private = private_paths()
    os.makedirs(os.path.join(REPO, "catalog"), exist_ok=True)
    jobs, folders = [], {}
    for root, dirs, files in os.walk(REPO):
        dirs[:] = sorted(d for d in dirs if d not in SKIP_DIRS)
        rel_root = os.path.relpath(root, REPO).replace(os.sep, "/")
        for name in sorted(files):
            rel = name if rel_root == "." else f"{rel_root}/{name}"
            if rel in private:
                continue
            size = os.path.getsize(os.path.join(root, name))
            entry = folders.setdefault(os.path.dirname(rel) or ".", [0, 0])
            entry[0] += 1
            entry[1] += size
            if name.lower().endswith((".bsp",) + ARCHIVE_EXT):
                jobs.append((rel, size))

    # Results are cached per (path, size), so a crash or rerun does not
    # re-read gigabytes of archives.
    cache = {}
    if os.path.exists(CACHE):
        with open(CACHE, encoding="utf-8") as f:
            cache = json.load(f)
    todo = [(rel, size) for rel, size in jobs if f"{rel}|{size}" not in cache]
    print(f"{len(jobs)} files to catalog, {len(todo)} not cached", file=sys.stderr)
    with ThreadPoolExecutor(max_workers=12) as pool:
        for i, (key, result) in enumerate(pool.map(scan_file, todo), 1):
            cache[key] = result
            if i % 500 == 0 or i == len(todo):
                with open(CACHE, "w", encoding="utf-8") as f:
                    json.dump(cache, f)
                print(f"  {i}/{len(todo)}", file=sys.stderr)

    maps, contents = [], []
    for rel, size in jobs:
        m, c = cache[f"{rel}|{size}"]
        maps += m
        contents += c
    maps = [[clean(v) for v in r] for r in maps]
    for r in maps:  # garbage version numbers = damaged file or not a map at all
        if r[3].startswith(("BSP v", "IBSP v")):
            r[3] = "unrecognized (damaged or not a map)"
    contents = [[clean(v) for v in r] for r in contents]

    with open(os.path.join(REPO, "catalog/maps.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["map_file", "location", "path_inside_archive", "format", "title", "author", "size_kb"])
        w.writerows(sorted(maps, key=lambda r: (r[0].lower(), r[1])))
    with open(os.path.join(REPO, "catalog/archive-contents.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["archive", "entry", "size_bytes"])
        w.writerows(contents)

    write_browsable(maps, contents)

    fmt_count = {}
    for r in maps:
        fmt_count[r[3] or "inside .rar/.7z (not read)"] = fmt_count.get(r[3] or "inside .rar/.7z (not read)", 0) + 1
    with open(os.path.join(REPO, "CATALOG.md"), "w", newline="\n", encoding="utf-8") as f:
        f.write("# Catalog\n\nGenerated by `tools/catalog.py`. Do not edit by hand; rerun the script.\n\n")
        f.write(f"- **{sum(v[0] for v in folders.values())} files**, "
                f"{sum(v[1] for v in folders.values()) / 2**30:.1f} GiB\n")
        f.write(f"- **{len(maps)} maps** (loose and inside archives), "
                f"{len({r[0].lower() for r in maps})} unique map file names: "
                "see [catalog/maps.csv](catalog/maps.csv)\n")
        f.write(f"- **{len(contents)} archive entries**: see [catalog/archive-contents.csv](catalog/archive-contents.csv)\n\n")
        f.write("## Maps by engine\n\n| Format | Maps |\n|---|---:|\n")
        for k, v in sorted(fmt_count.items(), key=lambda kv: -kv[1]):
            f.write(f"| {k} | {v} |\n")
        f.write("\n## Folders\n\n| Folder | Files | Size |\n|---|---:|---:|\n")
        for d, (n, s) in sorted(folders.items()):
            if d.startswith("catalog"):
                continue
            f.write(f"| `{d}` | {n} | {s / 2**20:.1f} MiB |\n")
        f.write("\n## Everything outside the big map folders\n\n")
        for d in sorted(folders):
            if d in (".", "quake/maps/bsp", "quake/maps/single-map-zips") or d.startswith("catalog") \
                    or d.count("/") > 2:
                continue
            entries = sorted(os.listdir(os.path.join(REPO, d)))
            files = [e for e in entries if os.path.isfile(os.path.join(REPO, d, e))
                     and f"{d}/{e}" not in private]
            if not files or len(files) > 60:
                continue
            f.write(f"### `{d}`\n\n")
            for e in files:
                t, a = (clean(x) for x in info_txt(os.path.join(REPO, d, e)))
                extra = f" — {t}" + (f" (by {a})" if a else "") if t else ""
                f.write(f"- `{e}`{extra}\n")
            f.write("\n")
    print(f"{len(maps)} maps, {len(contents)} archive entries", file=sys.stderr)


if __name__ == "__main__":
    main()
