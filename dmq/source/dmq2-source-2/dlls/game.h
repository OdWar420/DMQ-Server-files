/***
*
*	Copyright (c) 1996-2002 Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#ifndef GAME_H
#define GAME_H

extern void GameDLLInit( void );


extern cvar_t	displaysoundlist;
extern cvar_t	mapcyclefile;
extern cvar_t	servercfgfile;
extern cvar_t	lservercfgfile;

// multiplayer server rules
extern cvar_t	fraglimit;
extern cvar_t	timelimit;
extern cvar_t	deathmatch;
extern cvar_t	friendlyfire;
extern cvar_t	falldamage;
extern cvar_t	weaponstay;
extern cvar_t	forcerespaw;
extern cvar_t	flashlight;
extern cvar_t	aimcrosshair;
extern cvar_t	decalfrequency;
extern cvar_t	teamlist;
extern cvar_t	teamoverride;
extern cvar_t	defaultteam;
extern cvar_t	forcerespawn;
//DME  
extern cvar_t   urban;
extern cvar_t	allow_spectators;
extern cvar_t	roundtime;
extern cvar_t	maxclients;
extern cvar_t	gamemode;
extern cvar_t	default_map;
extern cvar_t	powerups;
extern cvar_t   startmatch;
extern cvar_t   outclassed;
extern cvar_t   selfdamage;
extern cvar_t   spawnmode;
extern cvar_t   rltrail;
extern cvar_t   spawn_protection;
extern cvar_t	protection_time;
extern cvar_t   warmup;
extern cvar_t   startmatchvoted;
//mfrx 05DEC05
extern cvar_t	requirematchvoted;
extern cvar_t	shaftpushground;
extern cvar_t	shaftpushair;
//mfrx
extern cvar_t   regenerate;
extern cvar_t   infinite_ammo;
extern cvar_t	banpowerup_quad;
extern cvar_t	banpowerup_dropquad;
extern cvar_t	banpowerup_666;
extern cvar_t	banpowerup_eyes;
extern cvar_t	banpowerup_dropeyes;
extern cvar_t	banarmor;
extern cvar_t   banhealth;
extern cvar_t   banweapon_ssg;
extern cvar_t   banweapon_ng;
extern cvar_t   banweapon_sng;
extern cvar_t   banweapon_gl;
extern cvar_t   banweapon_rl;
extern cvar_t   banweapon_lg;
extern cvar_t   banammo_shells;
extern cvar_t	banammo_nails;
extern cvar_t	banammo_rockets;
extern cvar_t	banammo_cells;
extern cvar_t	banallpowerups;
extern cvar_t   banallweapons;
extern cvar_t   banallammo;
extern cvar_t   banall;

extern cvar_t	directhitsonly;
extern cvar_t	trueshaft;

extern cvar_t	powerups;
extern cvar_t	clansize;

//TrenD++ 10NOV05
extern cvar_t   discharge;
//TrenD--

extern cvar_t	grappleairspeed;
extern cvar_t	grappledragspeed;
extern cvar_t	grappledisable;
extern cvar_t	grappledamage;


extern cvar_t	ctf;
extern cvar_t	ctfteamlist;
extern cvar_t	ctfflagscore;
extern cvar_t	ctfstealscore;
extern cvar_t	ctfreturnscore;


extern cvar_t	damagehomingrpg;
//DME END

// Engine Cvars
extern cvar_t	*g_psv_gravity;
extern cvar_t	*g_psv_aim;
extern cvar_t	*g_footsteps;



#endif		// GAME_H
