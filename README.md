# DMQ Server Files — Quake / DMC / DMQ archive

**Almost 30 years of collecting.** Since the late 1990s, Billy has gathered and
kept server files, maps, mods, tools and source code from the Quake 1,
Deathmatch Classic (DMC) and DMQ community scene: from old community servers,
from the people he played with, and from his own backups. Much of it is no
longer available anywhere else online. This archive preserves that collection
in one place.

- **Full archive (9.3 GiB, ~13 300 files, 10 963 maps):** https://huggingface.co/datasets/Nomatter/dmq-server-files
- **What is in it:** [INDEX.md](INDEX.md) — a guide to every item: what it is and who made it
- **Folder overview:** [CATALOG.md](CATALOG.md) — file counts and sizes per folder
- **Find a map:** [catalog/maps.csv](catalog/maps.csv) — every map, including
  maps inside archives, with the in-game title read from the BSP file
- **Find anything inside a zip/rar/7z:** [catalog/archive-contents.csv](catalog/archive-contents.csv)

## In memory of PIX

**DMQ 2 exists because of PIX.** He wrote most of its code on his own, building
on DMC Pro by TrenD*KilleR, and the DMQ servers this archive comes from ran on
his work. Every map rotation, every vote and every duel in `dmq/` traces back
to what he built.

To the people who played DMQ he was more than a name in a readme. He was a
friend from another country: someone you knew through the game, never met in
person, and faced in more matches than anyone could count.

This archive is kept in his honour. His source code is preserved unchanged in
[`dmq/source/dmq2-source-2/`](dmq/source/dmq2-source-2/), with these words
added at the top of its readme:

> Memorial for PIX, the legend that was a god in coding, that did most of the
> coding for DMQ2 alone. Everytime Quake is on my mind I think about you old friend!
> Rest in peace PIX!
> &nbsp;-Billy

## What is on GitHub and what is on Hugging Face

This GitHub repository holds the catalog, the scripts and the two source
trees you can browse online (`dmq/source/dmq2-source-2/` and
`dmc/source/DMC_Pro_2.0_Source/`). **All files** — maps, mods, tools, the
same source trees and everything else in the table below — live in the
Hugging Face dataset linked above, under the same paths the catalog uses.

## Layout

| Folder | What is there |
|---|---|
| `dmq/source/` | DMQ 2 source code by PIX (based on DMC Pro by TrenD*KilleR) |
| `dmq/releases/` | DMQ2 beta 3, three mfrx server builds and the required server packs (mapcycles, vote config) |
| `dmq/map-textures/` | High- and low-res texture WADs for the DMQ maps |
| `dmq/server-plugins/` | mfrx plugins (vote, details) for DMQ servers |
| `dmq/website/` | Saved pages from the old dmq.geeky.cc site |
| `dmc/source/` | DMC Pro 2.0 source code |
| `dmc/releases/` | DMC Pro 1.0 and a DMC client.dll |
| `dmc/maps/` | Standard DMC maps, the dmc.wad textures and 11 loose Half-Life-engine maps |
| `dmc/hlsdk/` | Half-Life SDK |
| `dmc/amxmodx-plugins/` | AMX Mod X plugins |
| `quake/maps/bsp/` | 6 526 loose Quake 1 maps (.bsp) |
| `quake/maps/single-map-zips/` | 3 331 single-map zips with their readme .txt files |
| `quake/maps/packs/` | Map packs (incl. the 1 GB deathmatch-maps.zip, Rocket Arena maps) |
| `quake/mods/` | Rocket Arena 1.2 (client + server), Clan Arena X, 3Wave CTF, Team Fortress, hook mods, QE mods … |
| `quake/qc-source/` | QuakeC source: Rocket Arena 1.2, CRMod, clean QC, progs 1.06, id's q1source … |
| `quake/engines/` | FTE, QuakeSpasm, QuakeWorld 2.30, ezQuake builds, Quake 1.08, QuakePatches |
| `quake/server/` | QWAdmin, KTX, QTV, qsmack, qvote, admin mods, mission-pack qwprogs |
| `quake/tools/` | Editors (TrenchBroom, BSP, QuArK, Qoole), compilers (ericw-tools, tyrutils, fteqcc), pak tools, launchers |
| `quake/sounds/` | Announcer packs (Quake 3, Unreal Tournament) and game sounds |
| `quake/textures/` | Texture WADs |
| `other-games/` | Quake 2 maps (65), Quake 3 maps and Doom files that ended up in the collection |
| `misc/` | Unrelated installers kept as they were found (VSCode, .NET, JRE …) |
| `tools/` | Scripts that built this layout and the catalog |

## How this archive was put together

The files were imported in their original layout first (the first commits),
then sorted in one rename commit. `tools/MOVES.tsv` and `tools/MOVES-2.tsv` list every move from the
old path to the new one, so anything can be traced back to where it was found.
13 byte-identical duplicates were dropped during sorting.

The catalog is generated: run `python3 tools/catalog.py .` from the archive root.

## Rights

This is a preservation archive. The maps, mods and tools belong to their
original authors; their readme files are kept next to them. Contact the
maintainer if you are an author and want something credited differently or removed.
