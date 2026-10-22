#!/usr/bin/env python3
"""Build the move plan (old path -> new path) for the DMQ archive.

Every top-level entry must match exactly one rule; anything unmatched aborts,
so nothing is silently left behind. Writes MOVES.tsv into the repo.
"""
import filecmp
import fnmatch
import os
import sys

REPO = "/mnt/d/Quake/DMQ-Server-files"
OLD = "Gold from the past - Old unsortet Quake and DMC(Q) stuff"
MAPS = "Gold from the past - Quake Maps"

# (glob pattern on the entry name, destination directory). First match wins.
ROOT_RULES = [
    ("dmq2-source-2*", "dmq/source"),
    ("DMQ2-beta3.zip", "dmq/releases"),
    ("DMQ2-Required.zip", "dmq/releases"),
    ("DMQ-Required.something.zip", "dmq/releases"),
    ("*DmqMaps.zip", "dmq/map-textures"),
    ("dmq2-mfrx-*.zip", "dmq/server-plugins"),
    ("plugin_mfrx_*.zip", "dmq/server-plugins"),
    ("dmq.geeky.cc", "dmq/website"),
    ("DMC_Pro_2.0_Source*", "dmc/source"),
    ("dmc_pro_v1.0.zip", "dmc/releases"),
    ("client.zip", "dmc/releases"),
    ("standard-maps.zip", "dmc/maps"),
    ("textures.zip", "dmc/maps"),
    ("hlsdk", "dmc"),
    ("BSPeditor", "quake/tools/editors"),
    ("ericw-tools-*", "quake/tools/compilers"),
    ("Quake Injector", "quake/tools/launchers"),
    ("newdir.txt", "misc"),
]

OLD_RULES = [
    # --- DMQ / DMC ---
    ("plugin_mfrx_vote*.zip", "dmq/server-plugins"),
    ("qs_manstanding.zip", "dmc/amxmodx-plugins"),
    # --- other games ---
    ("doom-1.8.wad.gz", "other-games/doom"),
    ("gzdoom-*", "other-games/doom"),
    ("q2dmv1*.zip", "other-games/quake2"),
    ("dm4ishq2.zip", "other-games/quake2"),
    ("xmod-*.zip", "other-games/quake2"),
    ("TourettesGuy_v4.zip", "other-games/quake2"),
    ("q2toq3t.zip", "other-games/quake3"),
    ("q3dm*.zip", "other-games/quake3"),
    ("curvy.zip", "other-games/quake3"),
    ("makumba.zip", "other-games/quake3"),
    ("Q3RA3_plus_music.rar", "other-games/quake3"),
    # --- sounds ---
    ("GameSounds", "quake/sounds"),
    ("sound.zip", "quake/sounds"),
    ("casounds.zip", "quake/sounds"),
    ("tts.rar", "quake/sounds"),
    ("Ultimate Man Sound Pack*", "quake/sounds"),
    ("PC Computer - * - Announcer.zip", "quake/sounds/announcers"),
    # --- textures ---
    ("start.zip", "quake/textures"),
    ("wad.rar", "quake/textures"),
    # --- engines / clients ---
    ("FTE quake", "quake/engines"),
    ("quakespasm-*", "quake/engines"),
    ("qw230.zip", "quake/engines"),
    ("QuakePatches_Win_x64.7z", "quake/engines"),
    ("bpsQuake*.rar", "quake/engines"),
    ("locke_nQuake_till_bp.zip", "quake/engines"),
    # --- servers / admin ---
    ("QWAdmin_*.zip", "quake/server"),
    ("admin*.zip", "quake/server"),
    ("qvote27.zip", "quake/server"),
    ("ktx.zip", "quake/server"),
    ("qtv10full.zip", "quake/server"),
    ("qsmack*.zip", "quake/server"),
    ("tcpserver-*.tar.gz", "quake/server"),
    ("missionpacks.qwprogs.zip", "quake/server"),
    # --- QuakeC source ---
    ("cleanqc-gpl*", "quake/qc-source"),
    ("cr5src.zip", "quake/qc-source"),
    ("cr62src.zip", "quake/qc-source"),
    ("progs_dump_devkit_*", "quake/qc-source"),
    ("quake_authmdl-master.zip", "quake/qc-source"),
    # --- mods ---
    ("CAx_*.zip", "quake/mods"),
    ("casrv13.zip", "quake/mods"),
    ("QECTF*", "quake/mods"),
    ("qectf_*", "quake/mods"),
    ("QERocketOnly*", "quake/mods"),
    ("rocketonly_quake_enhanced.zip", "quake/mods"),
    ("rarena11.zip", "quake/mods"),
    ("oztf145.zip", "quake/mods"),
    ("hook30b2.zip", "quake/mods"),
    ("wickhook.zip", "quake/mods"),
    ("multimod.zip", "quake/mods"),
    ("camera.zip", "quake/mods"),
    ("mcamnew.zip", "quake/mods"),
    ("creepc17.zip", "quake/mods"),
    ("apocalypze.zip", "quake/mods"),
    ("bsi_aa19.zip", "quake/mods"),
    ("QuakeMegaMod-main.zip", "quake/mods"),
    ("qintro_c.zip", "quake/mods"),
    ("runequake_*", "quake/mods"),
    # --- tools ---
    ("TrenchBroom-*", "quake/tools/editors"),
    ("Qoole250.exe", "quake/tools/editors"),
    ("quark-*", "quake/tools/editors"),
    ("Map_edit_pro.zip", "quake/tools/editors"),
    ("ericw-tools-*", "quake/tools/compilers"),
    ("tyrutils-*", "quake/tools/compilers"),
    ("BSP97q5.zip", "quake/tools/compilers"),
    ("bsp.zip", "quake/tools/compilers"),
    ("ne_q1spCompilingGui*", "quake/tools/compilers"),
    ("fteqccgui*.exe", "quake/tools/compilers"),
    ("qccx100.zip", "quake/tools/compilers"),
    ("pak*.zip", "quake/tools/pak"),
    ("QuakePakSharp-master.zip", "quake/tools/pak"),
    ("extract_313.exe", "quake/tools/pak"),
    ("QuakeInjectorQt-*", "quake/tools/launchers"),
    ("quakeinjector-*", "quake/tools/launchers"),
    ("QEModList-master.zip", "quake/tools/launchers"),
    ("QuakeMapDBEditor-master.zip", "quake/tools/launchers"),
    ("Quake-Nav-Depot-master.zip", "quake/tools/bots"),
    # --- maps ---
    ("*.bsp", "quake/maps/bsp"),
    ("100brush.zip", "quake/maps/packs"),
    ("RA-maps.rar", "quake/maps/packs"),
    ("onlinemaps mixed bundle.tgz", "quake/maps/packs"),
    ("maphub-maps-*", "quake/maps/packs"),
    ("quakeone_maps.zip", "quake/maps/packs"),
    ("mappack1.zip", "quake/maps/packs"),
    ("ultrav_*", "quake/maps/packs"),
    ("quake1.zip", "quake/maps/packs"),
    ("ukpak.zip", "quake/maps/packs"),
    ("sm4_plus_pm4.rar", "quake/maps/packs"),
    ("dmc[123].zip", "quake/maps/packs"),
    # --- non-Quake installers and leftovers ---
    ("GameRangerSetup.exe", "misc"),
    ("HexEditor_*", "misc"),
    ("JDownloaderSetup.exe", "misc"),
    ("LiveSplit_*", "misc"),
    ("PluginManager_*", "misc"),
    ("VC_redist.x64.exe", "misc"),
    ("VSCodeUserSetup-*", "misc"),
    ("dotnet-sdk-*", "misc"),
    ("jre-*", "misc"),
    ("httrack_*", "misc"),
    ("nppConvert.*", "misc"),
    ("settings_355_1M+.dat", "misc"),
    # single-map zips (checked last so the specific rules above win)
    ("*.zip", "quake/maps/packs"),
]

# Sub-folders of OLD that are themselves split up.
QUAKE_STUFF_RULES = [
    ("3wave41.zip", "quake/mods"),
    ("3wctfc.zip", "quake/mods"),
    ("farena12.zip", "quake/mods"),
    ("fasrv12.zip", "quake/mods"),
    ("bbelief.zip", "quake/mods"),
    ("pak0.pak", "quake/mods"),
    ("QERocketOnly-master.zip", "quake/mods"),
    ("Kurok compatible quakec.rar", "quake/qc-source"),
    ("progs106.zip", "quake/qc-source"),
    ("progs_dump_devkit_*", "quake/qc-source"),
    ("q1source*.zip", "quake/qc-source"),
    ("Quake compat Kurok PC.rar", "quake/engines"),
    ("q101-106.zip", "quake/engines"),
    ("QWAdmin_*.zip", "quake/server"),
    ("quake108.zip", "quake/engines"),
    ("tcpserver-*.tar.gz", "quake/server"),
    ("tele_q1.zip", "quake/mods"),
    ("tf28inst.exe", "quake/mods"),
    ("tf29qw.zip", "quake/mods"),
    # Rocket Arena readme; renamed so it cannot clash with other readmes
    ("readme.txt", "=quake/mods/farena12-readme.txt"),
]
QUAKE_MAPS_RULES = [
    ("* Quake3map.zip", "other-games/quake3"),
    ("maps", "quake/maps/single-map-zips"),  # contents moved, not the folder
    ("*", "quake/maps/packs"),
]


def match(name, rules):
    for pat, dst in rules:
        if fnmatch.fnmatchcase(name, pat):
            return dst
    return None


def plan():
    moves, unmatched = [], []

    def add(src, rules, flatten=False):
        name = os.path.basename(src)
        dst = match(name, rules)
        if dst is None:
            unmatched.append(src)
        elif dst.startswith("="):
            moves.append((src, dst[1:]))
        elif flatten:
            for child in sorted(os.listdir(os.path.join(REPO, src))):
                moves.append((f"{src}/{child}", f"{dst}/{child}"))
        else:
            moves.append((src, f"{dst}/{name}"))

    for name in sorted(os.listdir(REPO)):
        if name.startswith(".") or name in ("README.md", "MOVES.tsv"):
            continue
        if name == MAPS:
            for child in sorted(os.listdir(os.path.join(REPO, MAPS))):
                moves.append((f"{MAPS}/{child}", f"quake/maps/bsp/{child}"))
        elif name == OLD:
            for child in sorted(os.listdir(os.path.join(REPO, OLD))):
                src = f"{OLD}/{child}"
                if child == "Quake stuff":
                    for c in sorted(os.listdir(os.path.join(REPO, src))):
                        add(f"{src}/{c}", QUAKE_STUFF_RULES)
                elif child == "Quake maps":
                    for c in sorted(os.listdir(os.path.join(REPO, src))):
                        add(f"{src}/{c}", QUAKE_MAPS_RULES, flatten=(c == "maps"))
                elif child in ("plugins", "updater"):
                    continue  # empty folders, git does not track them
                else:
                    add(src, OLD_RULES)
        else:
            add(name, ROOT_RULES)
    return moves, unmatched


def main():
    moves, unmatched = plan()
    if unmatched:
        print("UNMATCHED:", *unmatched, sep="\n  ")
        sys.exit(1)
    seen = {}
    for src, dst in moves:
        seen.setdefault(dst.lower(), []).append(src)
    clashes = {d: s for d, s in seen.items() if len(s) > 1}
    # Same name twice: identical bytes -> keep one (the other copy stays in
    # the import commits); different bytes -> keep both, suffix the second.
    final = []
    dropped = {}
    for src, dst in moves:
        group = seen[dst.lower()]
        if len(group) > 1 and src != group[0]:
            same = filecmp.cmp(os.path.join(REPO, src), os.path.join(REPO, group[0]), shallow=False)
            if same:
                dropped[src] = dst
                print("DUPLICATE", src, "==", dst)
                continue
            stem, ext = os.path.splitext(dst)
            dst = f"{stem}-alt{group.index(src)}{ext}"
            print("RENAMED", src, "->", dst)
        final.append((src, dst))
    moves = final
    with open(os.path.join(REPO, "MOVES.tsv"), "w", newline="\n") as f:
        f.write("old_path\tnew_path\n")
        for src, dst in moves:
            f.write(f"{src}\t{dst}\n")
        for src, dst in dropped.items():
            f.write(f"{src}\t=DUPLICATE OF {dst}\n")
    print(f"{len(moves)} moves, {len(dropped)} duplicates removed")


if __name__ == "__main__":
    main()
