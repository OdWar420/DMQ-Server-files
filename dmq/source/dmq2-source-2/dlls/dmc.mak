# Microsoft Developer Studio Generated NMAKE File, Based on dmc.dsp
!IF "$(CFG)" == ""
CFG=dmc - Win32 Release
!MESSAGE No configuration specified. Defaulting to dmc - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "dmc - Win32 Release" && "$(CFG)" != "dmc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dmc.mak" CFG="dmc - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dmc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dmc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dmc - Win32 Release"

OUTDIR=.\Releasedmc
INTDIR=.\Releasedmc
# Begin Custom Macros
OutDir=.\Releasedmc
# End Custom Macros

ALL : "$(OUTDIR)\dmc.dll" "$(OUTDIR)\dmc.bsc"


CLEAN :
	-@erase "$(INTDIR)\animating.obj"
	-@erase "$(INTDIR)\animating.sbr"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\animation.sbr"
	-@erase "$(INTDIR)\bmodels.obj"
	-@erase "$(INTDIR)\bmodels.sbr"
	-@erase "$(INTDIR)\buttons.obj"
	-@erase "$(INTDIR)\buttons.sbr"
	-@erase "$(INTDIR)\cbase.obj"
	-@erase "$(INTDIR)\cbase.sbr"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\client.sbr"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\combat.sbr"
	-@erase "$(INTDIR)\dme_arena.obj"
	-@erase "$(INTDIR)\dme_arena.sbr"
	-@erase "$(INTDIR)\dme_clanarena.obj"
	-@erase "$(INTDIR)\dme_clanarena.sbr"
	-@erase "$(INTDIR)\dme_ctf.obj"
	-@erase "$(INTDIR)\dme_ctf.sbr"
	-@erase "$(INTDIR)\dme_match.obj"
	-@erase "$(INTDIR)\dme_match.sbr"
	-@erase "$(INTDIR)\dme_midair.obj"
	-@erase "$(INTDIR)\dme_midair.sbr"
	-@erase "$(INTDIR)\dme_rumble.obj"
	-@erase "$(INTDIR)\dme_rumble.sbr"
	-@erase "$(INTDIR)\dme_utility.obj"
	-@erase "$(INTDIR)\dme_utility.sbr"
	-@erase "$(INTDIR)\doors.obj"
	-@erase "$(INTDIR)\doors.sbr"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\effects.sbr"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\explode.sbr"
	-@erase "$(INTDIR)\func_break.obj"
	-@erase "$(INTDIR)\func_break.sbr"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\game.sbr"
	-@erase "$(INTDIR)\gamerules.obj"
	-@erase "$(INTDIR)\gamerules.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\h_ai.obj"
	-@erase "$(INTDIR)\h_ai.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\hook.obj"
	-@erase "$(INTDIR)\hook.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\lights.sbr"
	-@erase "$(INTDIR)\maprules.obj"
	-@erase "$(INTDIR)\maprules.sbr"
	-@erase "$(INTDIR)\monsters.obj"
	-@erase "$(INTDIR)\monsters.sbr"
	-@erase "$(INTDIR)\monsterstate.obj"
	-@erase "$(INTDIR)\monsterstate.sbr"
	-@erase "$(INTDIR)\multiplay_gamerules.obj"
	-@erase "$(INTDIR)\multiplay_gamerules.sbr"
	-@erase "$(INTDIR)\nodes.obj"
	-@erase "$(INTDIR)\nodes.sbr"
	-@erase "$(INTDIR)\observer.obj"
	-@erase "$(INTDIR)\observer.sbr"
	-@erase "$(INTDIR)\pathcorner.obj"
	-@erase "$(INTDIR)\pathcorner.sbr"
	-@erase "$(INTDIR)\plane.obj"
	-@erase "$(INTDIR)\plane.sbr"
	-@erase "$(INTDIR)\plats.obj"
	-@erase "$(INTDIR)\plats.sbr"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\player.sbr"
	-@erase "$(INTDIR)\pm_debug.obj"
	-@erase "$(INTDIR)\pm_debug.sbr"
	-@erase "$(INTDIR)\pm_math.obj"
	-@erase "$(INTDIR)\pm_math.sbr"
	-@erase "$(INTDIR)\pm_shared.obj"
	-@erase "$(INTDIR)\pm_shared.sbr"
	-@erase "$(INTDIR)\quake_gun.obj"
	-@erase "$(INTDIR)\quake_gun.sbr"
	-@erase "$(INTDIR)\quake_items.obj"
	-@erase "$(INTDIR)\quake_items.sbr"
	-@erase "$(INTDIR)\quake_nail.obj"
	-@erase "$(INTDIR)\quake_nail.sbr"
	-@erase "$(INTDIR)\quake_player.obj"
	-@erase "$(INTDIR)\quake_player.sbr"
	-@erase "$(INTDIR)\quake_rocket.obj"
	-@erase "$(INTDIR)\quake_rocket.sbr"
	-@erase "$(INTDIR)\quake_weapons_all.obj"
	-@erase "$(INTDIR)\quake_weapons_all.sbr"
	-@erase "$(INTDIR)\schedule.obj"
	-@erase "$(INTDIR)\schedule.sbr"
	-@erase "$(INTDIR)\singleplay_gamerules.obj"
	-@erase "$(INTDIR)\singleplay_gamerules.sbr"
	-@erase "$(INTDIR)\skill.obj"
	-@erase "$(INTDIR)\skill.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\spectator.obj"
	-@erase "$(INTDIR)\spectator.sbr"
	-@erase "$(INTDIR)\subs.obj"
	-@erase "$(INTDIR)\subs.sbr"
	-@erase "$(INTDIR)\teamplay_gamerules.obj"
	-@erase "$(INTDIR)\teamplay_gamerules.sbr"
	-@erase "$(INTDIR)\triggers.obj"
	-@erase "$(INTDIR)\triggers.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\voice_gamemgr.obj"
	-@erase "$(INTDIR)\voice_gamemgr.sbr"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\weapons.sbr"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\world.sbr"
	-@erase "$(OUTDIR)\dmc.bsc"
	-@erase "$(OUTDIR)\dmc.dll"
	-@erase "$(OUTDIR)\dmc.exp"
	-@erase "$(OUTDIR)\dmc.lib"
	-@erase "$(OUTDIR)\dmc.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /I "..\..\engine" /I "..\..\common" /I "." /I "..\..\game_shared" /I "..\dlls" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\dmc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dmc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\quake_gun.sbr" \
	"$(INTDIR)\quake_weapons_all.sbr" \
	"$(INTDIR)\animating.sbr" \
	"$(INTDIR)\animation.sbr" \
	"$(INTDIR)\bmodels.sbr" \
	"$(INTDIR)\buttons.sbr" \
	"$(INTDIR)\cbase.sbr" \
	"$(INTDIR)\client.sbr" \
	"$(INTDIR)\combat.sbr" \
	"$(INTDIR)\dme_arena.sbr" \
	"$(INTDIR)\dme_clanarena.sbr" \
	"$(INTDIR)\dme_ctf.sbr" \
	"$(INTDIR)\dme_match.sbr" \
	"$(INTDIR)\dme_midair.sbr" \
	"$(INTDIR)\dme_rumble.sbr" \
	"$(INTDIR)\dme_utility.sbr" \
	"$(INTDIR)\doors.sbr" \
	"$(INTDIR)\effects.sbr" \
	"$(INTDIR)\explode.sbr" \
	"$(INTDIR)\func_break.sbr" \
	"$(INTDIR)\game.sbr" \
	"$(INTDIR)\gamerules.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\h_ai.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\hook.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\lights.sbr" \
	"$(INTDIR)\maprules.sbr" \
	"$(INTDIR)\monsters.sbr" \
	"$(INTDIR)\monsterstate.sbr" \
	"$(INTDIR)\multiplay_gamerules.sbr" \
	"$(INTDIR)\nodes.sbr" \
	"$(INTDIR)\observer.sbr" \
	"$(INTDIR)\pathcorner.sbr" \
	"$(INTDIR)\plane.sbr" \
	"$(INTDIR)\plats.sbr" \
	"$(INTDIR)\player.sbr" \
	"$(INTDIR)\pm_debug.sbr" \
	"$(INTDIR)\pm_math.sbr" \
	"$(INTDIR)\pm_shared.sbr" \
	"$(INTDIR)\quake_items.sbr" \
	"$(INTDIR)\quake_nail.sbr" \
	"$(INTDIR)\quake_player.sbr" \
	"$(INTDIR)\quake_rocket.sbr" \
	"$(INTDIR)\schedule.sbr" \
	"$(INTDIR)\singleplay_gamerules.sbr" \
	"$(INTDIR)\skill.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\spectator.sbr" \
	"$(INTDIR)\subs.sbr" \
	"$(INTDIR)\teamplay_gamerules.sbr" \
	"$(INTDIR)\triggers.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\voice_gamemgr.sbr" \
	"$(INTDIR)\weapons.sbr" \
	"$(INTDIR)\world.sbr"

"$(OUTDIR)\dmc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\dmc.pdb" /map:"$(INTDIR)\dmc.map" /machine:I386 /def:".\dmc.def" /out:"$(OUTDIR)\dmc.dll" /implib:"$(OUTDIR)\dmc.lib" 
LINK32_OBJS= \
	"$(INTDIR)\quake_gun.obj" \
	"$(INTDIR)\quake_weapons_all.obj" \
	"$(INTDIR)\animating.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\bmodels.obj" \
	"$(INTDIR)\buttons.obj" \
	"$(INTDIR)\cbase.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\dme_arena.obj" \
	"$(INTDIR)\dme_clanarena.obj" \
	"$(INTDIR)\dme_ctf.obj" \
	"$(INTDIR)\dme_match.obj" \
	"$(INTDIR)\dme_midair.obj" \
	"$(INTDIR)\dme_rumble.obj" \
	"$(INTDIR)\dme_utility.obj" \
	"$(INTDIR)\doors.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\func_break.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\gamerules.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\h_ai.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\hook.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\maprules.obj" \
	"$(INTDIR)\monsters.obj" \
	"$(INTDIR)\monsterstate.obj" \
	"$(INTDIR)\multiplay_gamerules.obj" \
	"$(INTDIR)\nodes.obj" \
	"$(INTDIR)\observer.obj" \
	"$(INTDIR)\pathcorner.obj" \
	"$(INTDIR)\plane.obj" \
	"$(INTDIR)\plats.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\pm_debug.obj" \
	"$(INTDIR)\pm_math.obj" \
	"$(INTDIR)\pm_shared.obj" \
	"$(INTDIR)\quake_items.obj" \
	"$(INTDIR)\quake_nail.obj" \
	"$(INTDIR)\quake_player.obj" \
	"$(INTDIR)\quake_rocket.obj" \
	"$(INTDIR)\schedule.obj" \
	"$(INTDIR)\singleplay_gamerules.obj" \
	"$(INTDIR)\skill.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\spectator.obj" \
	"$(INTDIR)\subs.obj" \
	"$(INTDIR)\teamplay_gamerules.obj" \
	"$(INTDIR)\triggers.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\voice_gamemgr.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\world.obj"

"$(OUTDIR)\dmc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dmc - Win32 Debug"

OUTDIR=.\Debugdmc
INTDIR=.\Debugdmc
# Begin Custom Macros
OutDir=.\Debugdmc
# End Custom Macros

ALL : "$(OUTDIR)\dmc.dll" "$(OUTDIR)\dmc.bsc"


CLEAN :
	-@erase "$(INTDIR)\animating.obj"
	-@erase "$(INTDIR)\animating.sbr"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\animation.sbr"
	-@erase "$(INTDIR)\bmodels.obj"
	-@erase "$(INTDIR)\bmodels.sbr"
	-@erase "$(INTDIR)\buttons.obj"
	-@erase "$(INTDIR)\buttons.sbr"
	-@erase "$(INTDIR)\cbase.obj"
	-@erase "$(INTDIR)\cbase.sbr"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\client.sbr"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\combat.sbr"
	-@erase "$(INTDIR)\dme_arena.obj"
	-@erase "$(INTDIR)\dme_arena.sbr"
	-@erase "$(INTDIR)\dme_clanarena.obj"
	-@erase "$(INTDIR)\dme_clanarena.sbr"
	-@erase "$(INTDIR)\dme_ctf.obj"
	-@erase "$(INTDIR)\dme_ctf.sbr"
	-@erase "$(INTDIR)\dme_match.obj"
	-@erase "$(INTDIR)\dme_match.sbr"
	-@erase "$(INTDIR)\dme_midair.obj"
	-@erase "$(INTDIR)\dme_midair.sbr"
	-@erase "$(INTDIR)\dme_rumble.obj"
	-@erase "$(INTDIR)\dme_rumble.sbr"
	-@erase "$(INTDIR)\dme_utility.obj"
	-@erase "$(INTDIR)\dme_utility.sbr"
	-@erase "$(INTDIR)\doors.obj"
	-@erase "$(INTDIR)\doors.sbr"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\effects.sbr"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\explode.sbr"
	-@erase "$(INTDIR)\func_break.obj"
	-@erase "$(INTDIR)\func_break.sbr"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\game.sbr"
	-@erase "$(INTDIR)\gamerules.obj"
	-@erase "$(INTDIR)\gamerules.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\h_ai.obj"
	-@erase "$(INTDIR)\h_ai.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\hook.obj"
	-@erase "$(INTDIR)\hook.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\lights.sbr"
	-@erase "$(INTDIR)\maprules.obj"
	-@erase "$(INTDIR)\maprules.sbr"
	-@erase "$(INTDIR)\monsters.obj"
	-@erase "$(INTDIR)\monsters.sbr"
	-@erase "$(INTDIR)\monsterstate.obj"
	-@erase "$(INTDIR)\monsterstate.sbr"
	-@erase "$(INTDIR)\multiplay_gamerules.obj"
	-@erase "$(INTDIR)\multiplay_gamerules.sbr"
	-@erase "$(INTDIR)\nodes.obj"
	-@erase "$(INTDIR)\nodes.sbr"
	-@erase "$(INTDIR)\observer.obj"
	-@erase "$(INTDIR)\observer.sbr"
	-@erase "$(INTDIR)\pathcorner.obj"
	-@erase "$(INTDIR)\pathcorner.sbr"
	-@erase "$(INTDIR)\plane.obj"
	-@erase "$(INTDIR)\plane.sbr"
	-@erase "$(INTDIR)\plats.obj"
	-@erase "$(INTDIR)\plats.sbr"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\player.sbr"
	-@erase "$(INTDIR)\pm_debug.obj"
	-@erase "$(INTDIR)\pm_debug.sbr"
	-@erase "$(INTDIR)\pm_math.obj"
	-@erase "$(INTDIR)\pm_math.sbr"
	-@erase "$(INTDIR)\pm_shared.obj"
	-@erase "$(INTDIR)\pm_shared.sbr"
	-@erase "$(INTDIR)\quake_gun.obj"
	-@erase "$(INTDIR)\quake_gun.sbr"
	-@erase "$(INTDIR)\quake_items.obj"
	-@erase "$(INTDIR)\quake_items.sbr"
	-@erase "$(INTDIR)\quake_nail.obj"
	-@erase "$(INTDIR)\quake_nail.sbr"
	-@erase "$(INTDIR)\quake_player.obj"
	-@erase "$(INTDIR)\quake_player.sbr"
	-@erase "$(INTDIR)\quake_rocket.obj"
	-@erase "$(INTDIR)\quake_rocket.sbr"
	-@erase "$(INTDIR)\quake_weapons_all.obj"
	-@erase "$(INTDIR)\quake_weapons_all.sbr"
	-@erase "$(INTDIR)\schedule.obj"
	-@erase "$(INTDIR)\schedule.sbr"
	-@erase "$(INTDIR)\singleplay_gamerules.obj"
	-@erase "$(INTDIR)\singleplay_gamerules.sbr"
	-@erase "$(INTDIR)\skill.obj"
	-@erase "$(INTDIR)\skill.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\spectator.obj"
	-@erase "$(INTDIR)\spectator.sbr"
	-@erase "$(INTDIR)\subs.obj"
	-@erase "$(INTDIR)\subs.sbr"
	-@erase "$(INTDIR)\teamplay_gamerules.obj"
	-@erase "$(INTDIR)\teamplay_gamerules.sbr"
	-@erase "$(INTDIR)\triggers.obj"
	-@erase "$(INTDIR)\triggers.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\voice_gamemgr.obj"
	-@erase "$(INTDIR)\voice_gamemgr.sbr"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\weapons.sbr"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\world.sbr"
	-@erase "$(OUTDIR)\dmc.bsc"
	-@erase "$(OUTDIR)\dmc.dll"
	-@erase "$(OUTDIR)\dmc.exp"
	-@erase "$(OUTDIR)\dmc.ilk"
	-@erase "$(OUTDIR)\dmc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\..\game_shared" /I "..\dlls" /I "..\..\engine" /I "..\..\common" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dmc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dmc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\quake_gun.sbr" \
	"$(INTDIR)\quake_weapons_all.sbr" \
	"$(INTDIR)\animating.sbr" \
	"$(INTDIR)\animation.sbr" \
	"$(INTDIR)\bmodels.sbr" \
	"$(INTDIR)\buttons.sbr" \
	"$(INTDIR)\cbase.sbr" \
	"$(INTDIR)\client.sbr" \
	"$(INTDIR)\combat.sbr" \
	"$(INTDIR)\dme_arena.sbr" \
	"$(INTDIR)\dme_clanarena.sbr" \
	"$(INTDIR)\dme_ctf.sbr" \
	"$(INTDIR)\dme_match.sbr" \
	"$(INTDIR)\dme_midair.sbr" \
	"$(INTDIR)\dme_rumble.sbr" \
	"$(INTDIR)\dme_utility.sbr" \
	"$(INTDIR)\doors.sbr" \
	"$(INTDIR)\effects.sbr" \
	"$(INTDIR)\explode.sbr" \
	"$(INTDIR)\func_break.sbr" \
	"$(INTDIR)\game.sbr" \
	"$(INTDIR)\gamerules.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\h_ai.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\hook.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\lights.sbr" \
	"$(INTDIR)\maprules.sbr" \
	"$(INTDIR)\monsters.sbr" \
	"$(INTDIR)\monsterstate.sbr" \
	"$(INTDIR)\multiplay_gamerules.sbr" \
	"$(INTDIR)\nodes.sbr" \
	"$(INTDIR)\observer.sbr" \
	"$(INTDIR)\pathcorner.sbr" \
	"$(INTDIR)\plane.sbr" \
	"$(INTDIR)\plats.sbr" \
	"$(INTDIR)\player.sbr" \
	"$(INTDIR)\pm_debug.sbr" \
	"$(INTDIR)\pm_math.sbr" \
	"$(INTDIR)\pm_shared.sbr" \
	"$(INTDIR)\quake_items.sbr" \
	"$(INTDIR)\quake_nail.sbr" \
	"$(INTDIR)\quake_player.sbr" \
	"$(INTDIR)\quake_rocket.sbr" \
	"$(INTDIR)\schedule.sbr" \
	"$(INTDIR)\singleplay_gamerules.sbr" \
	"$(INTDIR)\skill.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\spectator.sbr" \
	"$(INTDIR)\subs.sbr" \
	"$(INTDIR)\teamplay_gamerules.sbr" \
	"$(INTDIR)\triggers.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\voice_gamemgr.sbr" \
	"$(INTDIR)\weapons.sbr" \
	"$(INTDIR)\world.sbr"

"$(OUTDIR)\dmc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dmc.pdb" /machine:I386 /def:".\dmc.def" /out:"$(OUTDIR)\dmc.dll" /implib:"$(OUTDIR)\dmc.lib" 
LINK32_OBJS= \
	"$(INTDIR)\quake_gun.obj" \
	"$(INTDIR)\quake_weapons_all.obj" \
	"$(INTDIR)\animating.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\bmodels.obj" \
	"$(INTDIR)\buttons.obj" \
	"$(INTDIR)\cbase.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\dme_arena.obj" \
	"$(INTDIR)\dme_clanarena.obj" \
	"$(INTDIR)\dme_ctf.obj" \
	"$(INTDIR)\dme_match.obj" \
	"$(INTDIR)\dme_midair.obj" \
	"$(INTDIR)\dme_rumble.obj" \
	"$(INTDIR)\dme_utility.obj" \
	"$(INTDIR)\doors.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\func_break.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\gamerules.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\h_ai.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\hook.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\maprules.obj" \
	"$(INTDIR)\monsters.obj" \
	"$(INTDIR)\monsterstate.obj" \
	"$(INTDIR)\multiplay_gamerules.obj" \
	"$(INTDIR)\nodes.obj" \
	"$(INTDIR)\observer.obj" \
	"$(INTDIR)\pathcorner.obj" \
	"$(INTDIR)\plane.obj" \
	"$(INTDIR)\plats.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\pm_debug.obj" \
	"$(INTDIR)\pm_math.obj" \
	"$(INTDIR)\pm_shared.obj" \
	"$(INTDIR)\quake_items.obj" \
	"$(INTDIR)\quake_nail.obj" \
	"$(INTDIR)\quake_player.obj" \
	"$(INTDIR)\quake_rocket.obj" \
	"$(INTDIR)\schedule.obj" \
	"$(INTDIR)\singleplay_gamerules.obj" \
	"$(INTDIR)\skill.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\spectator.obj" \
	"$(INTDIR)\subs.obj" \
	"$(INTDIR)\teamplay_gamerules.obj" \
	"$(INTDIR)\triggers.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\voice_gamemgr.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\world.obj"

"$(OUTDIR)\dmc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dmc.dep")
!INCLUDE "dmc.dep"
!ELSE 
!MESSAGE Warning: cannot find "dmc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dmc - Win32 Release" || "$(CFG)" == "dmc - Win32 Debug"
SOURCE=.\quake_gun.cpp

"$(INTDIR)\quake_gun.obj"	"$(INTDIR)\quake_gun.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\quake_weapons_all.cpp

"$(INTDIR)\quake_weapons_all.obj"	"$(INTDIR)\quake_weapons_all.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\animating.cpp

"$(INTDIR)\animating.obj"	"$(INTDIR)\animating.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\animation.cpp

"$(INTDIR)\animation.obj"	"$(INTDIR)\animation.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bmodels.cpp

"$(INTDIR)\bmodels.obj"	"$(INTDIR)\bmodels.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\buttons.cpp

"$(INTDIR)\buttons.obj"	"$(INTDIR)\buttons.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cbase.cpp

"$(INTDIR)\cbase.obj"	"$(INTDIR)\cbase.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\client.cpp

"$(INTDIR)\client.obj"	"$(INTDIR)\client.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\combat.cpp

"$(INTDIR)\combat.obj"	"$(INTDIR)\combat.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dme_arena.cpp

"$(INTDIR)\dme_arena.obj"	"$(INTDIR)\dme_arena.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dme_clanarena.cpp

"$(INTDIR)\dme_clanarena.obj"	"$(INTDIR)\dme_clanarena.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dme_ctf.cpp

"$(INTDIR)\dme_ctf.obj"	"$(INTDIR)\dme_ctf.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dme_match.cpp

"$(INTDIR)\dme_match.obj"	"$(INTDIR)\dme_match.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dme_midair.cpp

"$(INTDIR)\dme_midair.obj"	"$(INTDIR)\dme_midair.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dme_rumble.cpp

"$(INTDIR)\dme_rumble.obj"	"$(INTDIR)\dme_rumble.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dme_utility.cpp

"$(INTDIR)\dme_utility.obj"	"$(INTDIR)\dme_utility.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\doors.cpp

"$(INTDIR)\doors.obj"	"$(INTDIR)\doors.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\effects.cpp

"$(INTDIR)\effects.obj"	"$(INTDIR)\effects.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\explode.cpp

"$(INTDIR)\explode.obj"	"$(INTDIR)\explode.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\func_break.cpp

"$(INTDIR)\func_break.obj"	"$(INTDIR)\func_break.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\game.cpp

"$(INTDIR)\game.obj"	"$(INTDIR)\game.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gamerules.cpp

"$(INTDIR)\gamerules.obj"	"$(INTDIR)\gamerules.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\globals.cpp

"$(INTDIR)\globals.obj"	"$(INTDIR)\globals.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\h_ai.cpp

"$(INTDIR)\h_ai.obj"	"$(INTDIR)\h_ai.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\h_export.cpp

"$(INTDIR)\h_export.obj"	"$(INTDIR)\h_export.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hook.cpp

"$(INTDIR)\hook.obj"	"$(INTDIR)\hook.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\items.cpp

"$(INTDIR)\items.obj"	"$(INTDIR)\items.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lights.cpp

"$(INTDIR)\lights.obj"	"$(INTDIR)\lights.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\maprules.cpp

"$(INTDIR)\maprules.obj"	"$(INTDIR)\maprules.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\monsters.cpp

"$(INTDIR)\monsters.obj"	"$(INTDIR)\monsters.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\monsterstate.cpp

"$(INTDIR)\monsterstate.obj"	"$(INTDIR)\monsterstate.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\multiplay_gamerules.cpp

"$(INTDIR)\multiplay_gamerules.obj"	"$(INTDIR)\multiplay_gamerules.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\nodes.cpp

"$(INTDIR)\nodes.obj"	"$(INTDIR)\nodes.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\observer.cpp

"$(INTDIR)\observer.obj"	"$(INTDIR)\observer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pathcorner.cpp

"$(INTDIR)\pathcorner.obj"	"$(INTDIR)\pathcorner.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plane.cpp

"$(INTDIR)\plane.obj"	"$(INTDIR)\plane.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plats.cpp

"$(INTDIR)\plats.obj"	"$(INTDIR)\plats.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\player.cpp

"$(INTDIR)\player.obj"	"$(INTDIR)\player.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\pm_shared\pm_debug.c

"$(INTDIR)\pm_debug.obj"	"$(INTDIR)\pm_debug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\pm_shared\pm_math.c

"$(INTDIR)\pm_math.obj"	"$(INTDIR)\pm_math.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\pm_shared\pm_shared.c

"$(INTDIR)\pm_shared.obj"	"$(INTDIR)\pm_shared.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\quake_items.cpp

"$(INTDIR)\quake_items.obj"	"$(INTDIR)\quake_items.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\quake_nail.cpp

"$(INTDIR)\quake_nail.obj"	"$(INTDIR)\quake_nail.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\quake_player.cpp

"$(INTDIR)\quake_player.obj"	"$(INTDIR)\quake_player.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\quake_rocket.cpp

"$(INTDIR)\quake_rocket.obj"	"$(INTDIR)\quake_rocket.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\schedule.cpp

"$(INTDIR)\schedule.obj"	"$(INTDIR)\schedule.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\singleplay_gamerules.cpp

"$(INTDIR)\singleplay_gamerules.obj"	"$(INTDIR)\singleplay_gamerules.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\skill.cpp

"$(INTDIR)\skill.obj"	"$(INTDIR)\skill.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sound.cpp

"$(INTDIR)\sound.obj"	"$(INTDIR)\sound.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\spectator.cpp

"$(INTDIR)\spectator.obj"	"$(INTDIR)\spectator.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\subs.cpp

"$(INTDIR)\subs.obj"	"$(INTDIR)\subs.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\teamplay_gamerules.cpp

"$(INTDIR)\teamplay_gamerules.obj"	"$(INTDIR)\teamplay_gamerules.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\triggers.cpp

"$(INTDIR)\triggers.obj"	"$(INTDIR)\triggers.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\util.cpp

"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\game_shared\voice_gamemgr.cpp

"$(INTDIR)\voice_gamemgr.obj"	"$(INTDIR)\voice_gamemgr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons.cpp

"$(INTDIR)\weapons.obj"	"$(INTDIR)\weapons.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\world.cpp

"$(INTDIR)\world.obj"	"$(INTDIR)\world.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

