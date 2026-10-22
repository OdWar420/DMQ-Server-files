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
extern cvar_t   infinite_ammo;
extern cvar_t	allow_spectators;
extern cvar_t	roundtime;
extern cvar_t	maxclients;
extern cvar_t	gamemode;
extern cvar_t	default_map;
extern cvar_t	powerups;
extern cvar_t   startmatch;
extern cvar_t   selfdamage;
extern cvar_t   spawnmode;
extern cvar_t   rltrail;
extern cvar_t   spawn_protection;
extern cvar_t	protection_time;
extern cvar_t   warmup;
extern cvar_t   regenerate;
extern cvar_t   infinite_ammo;

extern cvar_t	powerups;
extern cvar_t	clansize;


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
