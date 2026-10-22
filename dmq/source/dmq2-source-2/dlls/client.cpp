/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
// Robin, 4-22-98: Moved set_suicide_frame() here from player.cpp to allow us to 
//				   have one without a hardcoded player.mdl in tf_client.cpp

/*

===== client.cpp ========================================================

  client/server game specific stuff

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "player.h"
#include "spectator.h"
#include "client.h"
#include "gamerules.h"
#include "customentity.h"
#include "weapons.h"
#include "weaponinfo.h"
#include "usercmd.h"
#include "netadr.h"
#include "dme_utility.h"
#include "game.h"

//#include "grapple.h"
//trend
//hudtextparms_t m_hMessage;
//trend
//DME

extern void ShowGameHelp(CBasePlayer* pPlayer);
extern void ShowGameModes(CBasePlayer* pPlayer);
extern void ShowGameInfo(CBasePlayer* pPlayer);

	edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer );
//DME END
//mfrx 27FEB06
extern DLL_GLOBAL BOOL		g_runes_exist;	//for reseting runes
//mfrx
extern DLL_GLOBAL BOOL		g_ctf_redflagathome;
extern DLL_GLOBAL BOOL		g_ctf_blueflagathome;

//jammer infinity begin
int NumPlayerStarts;
int BluePlayerStarts;
int RedPlayerStarts;

//jammer ctf flag code
extern bool gRedFlagDroppedInTransit;
extern bool gBlueFlagDroppedInTransit;
extern bool gBlueFlagAtHome;
extern bool gRedFlagAtHome;
extern Vector gBlueFlagStartPoint;
extern Vector gBlueFlagStartAngle;
extern Vector gRedFlagStartPoint;
extern Vector gRedFlagStartAngle;
extern entvars_t *gBlueFlag;
extern entvars_t *gRedFlag;

extern void CtfMsgToAll(char *sztext);

extern BOOL FoundFile;
#include "weapons.h" //for the functions
extern char DatFileName[128];//world.cpp builds this string

//items data struct
itemdata_t FileItemData;

//#include "io.h"
FILE *fp;

//this is for OZ ONLY, willi you trouble maker :-)
#define NUM_WEAPONS 13
//struct for weapons that are disabled
struct _weapons
{
 BOOL IsDisabled;
}Weapons[NUM_WEAPONS]; // 13 weapons to check for disabled

BOOL AllWeaponsDisabled;
void CheckDisabledWeapons(void);
//jammer end
extern DLL_GLOBAL ULONG		g_ulModelIndexPlayer;
extern DLL_GLOBAL BOOL		g_fGameOver;
extern DLL_GLOBAL int		g_iSkillLevel;
extern DLL_GLOBAL ULONG		g_ulFrameCount;

extern bool g_bHaveMOTD;

extern void CopyToBodyQue(entvars_t* pev);
extern int giPrecacheGrunt;
extern int gmsgSayText;

extern int g_teamplay;

extern unsigned short g_sGibbed;
extern unsigned short g_sTeleport;
extern unsigned short g_sTrail;
extern unsigned short g_sExplosion;
extern unsigned short g_usPowerUp;

void LinkUserMessages( void );

/*
 * used by kill command and disconnect command
 * ROBIN: Moved here from player.cpp, to allow multiple player models
 */
void set_suicide_frame(entvars_t* pev)
{       
	if (!FStrEq(STRING(pev->model), "models/player.mdl"))
		return; // allready gibbed

//	pev->frame		= $deatha11;
	pev->solid		= SOLID_NOT;
	pev->movetype	= MOVETYPE_TOSS;
	pev->deadflag	= DEAD_DEAD;
	pev->nextthink	= -1;
}


/*
===========
ClientConnect

called when a player connects to a server
============
*/
BOOL ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ]  )
{	
	return g_pGameRules->ClientConnected( pEntity, pszName, pszAddress, szRejectReason );

// a client connecting during an intermission can cause problems
//	if (intermission_running)
//		ExitIntermission ();

}


/*
===========
ClientDisconnect

called when a player disconnects from a server

GLOBALS ASSUMED SET:  g_fGameOver
============
*/
void ClientDisconnect( edict_t *pEntity )
{
	if (g_fGameOver)
		return;

	entvars_t *pev = &pEntity->v;

	CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);

	// Oz CTF - drop those flags on death!
	if (pPlayer->ctf_hasredflag)
	{
		//jammer ctf flag code
		pPlayer->ctf_hasredflag = false;
		g_ctf_redflagathome = false;
		gRedFlagDroppedInTransit = false;//just in case
		gRedFlagAtHome = true;
		
		gRedFlag->flags = FL_KILLME;
		CBaseEntity *pEntity = CBaseEntity::Create( "ctf_redflag", 
			              gRedFlagStartPoint,gRedFlagStartAngle,pPlayer->edict());


		char szText[128];
		sprintf(szText, "The Fire flag has returned to base!\n");
		CtfMsgToAll(szText);
	}
	if (pPlayer->ctf_hasblueflag)
	{
		//jammer ctf flag code
		pPlayer->ctf_hasblueflag = false;
		g_ctf_blueflagathome = false;
		gBlueFlagDroppedInTransit = false;//just in case
		gBlueFlagAtHome = true;

		gBlueFlag->flags = FL_KILLME;
		CBaseEntity *pEntity = CBaseEntity::Create( "ctf_blueflag", 
			              gBlueFlagStartPoint,gBlueFlagStartAngle,pPlayer->edict());

		char szText[128];
		sprintf(szText, "The Ice flag has returned to base!\n");
		CtfMsgToAll(szText);
	}

	// Oz runes - disconnect spawn
	if (pPlayer->ctf_hasfrag)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hasfrag = false;

		pPlayer->SpawnRunes("ctf_frag");
	}

	if (pPlayer->ctf_hasregen)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hasregen = false;

		pPlayer->SpawnRunes("ctf_regen");
	}

	if (pPlayer->ctf_hasbar)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hasbar = false;

		pPlayer->SpawnRunes("ctf_crowbar");
	}

	if (pPlayer->ctf_hascycle)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hascycle = false;

		pPlayer->SpawnRunes("ctf_cycle");
	}

	if (pPlayer->ctf_hascloak)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hascloak = false;
		pev->rendermode = kRenderNormal;
		pev->renderfx &= ~kRenderFxGlowShell;

		pPlayer->SpawnRunes("ctf_cloak");
	}

	if (pPlayer->ctf_hashook)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hashook = false;
	
		pPlayer->SpawnRunes("ctf_hook");
	}

	if (pPlayer->ctf_hasprotect)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hasprotect = false;
	
		pPlayer->SpawnRunes("ctf_protect");
	}

	if (pPlayer->ctf_haslowgrav)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_haslowgrav = false;
	
		pPlayer->SpawnRunes("ctf_lowgrav");
	}

	if (pPlayer->ctf_hasvamp)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hasvamp = false;
	
		pPlayer->SpawnRunes("ctf_vamp");
	}

	if (pPlayer->ctf_hastrap)
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hastrap = false;
	
		pPlayer->SpawnRunes("ctf_trap");
	}
	if (pPlayer->ctf_hassuperglock)//jammer new superglock
	{
		pPlayer->ctf_hasrune = false;
		pPlayer->ctf_hassuperglock = false;
	
		pPlayer->SpawnRunes("ctf_superglock");
	}

	char text[256];
	sprintf( text, "- %s has left the game\n", STRING(pEntity->v.netname) );
	MESSAGE_BEGIN( MSG_ALL, gmsgSayText, NULL );
		WRITE_BYTE( ENTINDEX(pEntity) );
		WRITE_STRING( text );
	MESSAGE_END();

	// since the edict doesn't get deleted, fix it so it doesn't interfere.
	pEntity->v.takedamage = DAMAGE_NO;// don't attract autoaim
	pEntity->v.solid = SOLID_NOT;// nonsolid
	UTIL_SetOrigin ( &pEntity->v, pEntity->v.origin );

	g_pGameRules->ClientDisconnected( pEntity );
}


// called by ClientKill and DeadThink
void respawn(entvars_t* pev, BOOL fCopyCorpse)
{
	if (gpGlobals->coop || gpGlobals->deathmatch)
	{
		if ( fCopyCorpse )
		{
			// make a copy of the dead body for appearances sake
			CopyToBodyQue(pev);
		}

		// respawn player
		GetClassPtr( (CBasePlayer *)pev)->Spawn( );
	}
	else
	{       // restart the entire server
		SERVER_COMMAND("reload\n");
	}
}

/*
============
ClientKill

Player entered the suicide command

GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
============
*/
void ClientKill( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;

	CBasePlayer *pl = (CBasePlayer*) CBasePlayer::Instance( pev );

	//Chip - Disables observer suicide
    if ( pl->m_afPhysicsFlags & PFLAG_OBSERVER)
    {
        return;
    }
    //Chip
	//DME++
	if ( pev->flags & FL_GODMODE )
	{
			return;
	}
	//DME--

	if ( pl->m_fNextSuicideTime > gpGlobals->time )
		return;  // prevent suiciding too ofter

	pl->m_fNextSuicideTime = gpGlobals->time + 1;  // don't let them suicide for 5 seconds after suiciding

	//mfrx : Oz runes booby trap rune
	if(pl->ctf_hastrap)
		pl->wassuicide = TRUE;

	// have the player kill themself
	pev->health = 0;
	pl->Killed( pev, GIB_ALWAYS );

//	pev->modelindex = g_ulModelIndexPlayer;
//	pev->frags -= 2;		// extra penalty
//	respawn( pev );
}

/*
===========
ClientPutInServer

called each time a player is spawned
============
*/
void ClientPutInServer( edict_t *pEntity )
{
	CBasePlayer *pPlayer;

	entvars_t *pev = &pEntity->v;

	pPlayer = GetClassPtr((CBasePlayer *)pev);
	pPlayer->SetCustomDecalFrames(-1); // Assume none;

	pPlayer->m_bHadFirstSpawn = false;

	pPlayer ->m_nColor = 0;//MATCH

	// Allocate a CBasePlayer for pev, and call spawn
	pPlayer->Spawn();

	// Reset interpolation during first frame
	pPlayer->pev->effects |= EF_NOINTERP;
}

//// HOST_SAY
// String comes in as
// say blah blah blah
// or as
// blah blah blah
//
void Host_Say( edict_t *pEntity, int teamonly )
{
	CBasePlayer *client;
	int		j;
	char	*p;
	char	text[128];
	char    szTemp[256];
	const char *cpSay = "say";
	const char *cpSayTeam = "say_team";
	const char *pcmd = CMD_ARGV(0);

	// We can get a raw string now, without the "say " prepended
	if ( CMD_ARGC() == 0 )
		return;

	if ( !stricmp( pcmd, cpSay) || !stricmp( pcmd, cpSayTeam ) )
	{
		if ( CMD_ARGC() >= 2 )
		{
			p = (char *)CMD_ARGS();
		}
		else
		{
			// say with a blank message, nothing to do
			return;
		}
	}
	else  // Raw text, need to prepend argv[0]
	{
		if ( CMD_ARGC() >= 2 )
		{
			sprintf( szTemp, "%s %s", ( char * )pcmd, (char *)CMD_ARGS() );
		}
		else
		{
			// Just a one word command, use the first word...sigh
			sprintf( szTemp, "%s", ( char * )pcmd );
		}
		p = szTemp;
	}

// remove quotes if present
	if (*p == '"')
	{
		p++;
		p[strlen(p)-1] = 0;
	}

// make sure the text has content
	for ( char *pc = p; pc != NULL && *pc != 0; pc++ )
	{
		if ( isprint( *pc ) && !isspace( *pc ) )
		{
			pc = NULL;	// we've found an alphanumeric character,  so text is valid
			break;
		}
	}
	if ( pc != NULL )
		return;  // no character found, so say nothing

// turn on color set 2  (color on,  no sound)
	if ( teamonly )
		sprintf( text, "%c(TEAM) %s(%i,%i): ", 2, STRING( pEntity->v.netname ) , (int)pEntity->v.health, (int)pEntity->v.armorvalue  );

	
	else
		sprintf( text, "%c%s: ", 2, STRING( pEntity->v.netname ) );

	j = sizeof(text) - 2 - strlen(text);  // -2 for /n and null terminator
	if ( (int)strlen(p) > j )
		p[j] = 0;

	strcat( text, p );
	strcat( text, "\n" );

	entvars_t *pev = &pEntity->v;
	CBasePlayer* player = GetClassPtr((CBasePlayer *)pev);

	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop

	client = NULL;
	while ( ((client = (CBasePlayer*)UTIL_FindEntityByClassname( client, "player" )) != NULL) && (!FNullEnt(client->edict())) ) 
	{
		if ( !client->pev )
			continue;
		
		if ( client->edict() == pEntity )
			continue;

		if ( !(client->IsNetClient()) )	// Not a client ? (should never be true)
			continue;


		if ( ((CHalfLifeMultiplay *)g_pGameRules)->m_VoiceGameMgr.PlayerHasBlockedPlayer( client, player ) )
			continue;

		entvars_t *pev = &pEntity->v;
		CBasePlayer *pp = GetClassPtr((CBasePlayer *)pev);
		if ( teamonly && !FStrEq( pp->m_szTeamName, client->m_szTeamName))
		continue;

		MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, client->pev );
			WRITE_BYTE( ENTINDEX(pEntity) );
			WRITE_STRING( text );
		MESSAGE_END();

	}

	// print to the sending client
	MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, &pEntity->v );
		WRITE_BYTE( ENTINDEX(pEntity) );
		WRITE_STRING( text );
	MESSAGE_END();

	// echo to server console
	g_engfuncs.pfnServerPrint( text );

	char * temp;
	if ( teamonly )
		temp = "say_team";
	else
		temp = "say";
//
//Modified by zapper
//			
	// team match?
	if ( g_teamplay )
	{
		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" %s \"%s\"\n", 
			STRING( pEntity->v.netname ), 
			GETPLAYERUSERID( pEntity ),
			GETPLAYERAUTHID( pEntity ),
			g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model" ),
			temp,
			p );
	}
	else
	{
	UTIL_LogPrintf( "\"%s<%i><%s><%i>\" %s \"%s\"\n", 
		STRING( pEntity->v.netname ), 
		GETPLAYERUSERID( pEntity ),
		GETPLAYERAUTHID( pEntity ),
		GETPLAYERUSERID( pEntity ),
		temp,
		p );
}
 }

//
// End of mod
//

/*
===========
ClientCommand
called each time a player uses a "cmd" command
============
*/
extern float g_flWeaponCheat;

// Use CMD_ARGV,  CMD_ARGV, and CMD_ARGC to get pointers the character string command.
void ClientCommand( edict_t *pEntity )
{

	char text[1024];

	const char *pcmd = CMD_ARGV(0);
	const char *pstr;
	
	// Is the client spawned yet?
	if ( !pEntity->pvPrivateData )
		return;

	entvars_t *pev = &pEntity->v;



	if ( FStrEq(pcmd, "say" ) )
	{
		Host_Say( pEntity, 0 );
	}
	else if ( FStrEq(pcmd, "say_team" ) )
	{
		Host_Say( pEntity, 1 );
	}
	//DME
	if ( FStrEq( pcmd, "spectate" ) )
	{
		if  (FStrEq(gamemode.string, "ffa") || FStrEq(gamemode.string, "instagib") || FStrEq(gamemode.string, "dmm4") || FStrEq(gamemode.string, "dmm5") || FStrEq(gamemode.string, "excessive") || FStrEq(gamemode.string, "prematch"))
		{			
			// Prevent this is the cvar is set
			if ( allow_spectators.value )
			{
				CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);
				edict_t *pentSpawnSpot = EntSelectSpawnPoint( pPlayer );
				if(!(pPlayer->IsAlive()))
				{
					pPlayer->StartObserver( VARS(pentSpawnSpot)->origin, VARS(pentSpawnSpot)->angles);
					sprintf(text, "* %s Is now a spectator\n", STRING( pPlayer->pev->netname ));
					UTIL_SayTextAll( text, pPlayer );
				}
			}
		        
		
		}
	}
	
	else if ( FStrEq( pcmd, "join" ) )
	{
		if  (FStrEq(gamemode.string, "ffa") || FStrEq(gamemode.string, "instagib") || FStrEq(gamemode.string, "dmm4") || FStrEq(gamemode.string, "dmm5") || FStrEq(gamemode.string, "excessive") || FStrEq(gamemode.string, "prematch"))
		{
			
			CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);
			pPlayer->StopObserver();
			

			sprintf( text, "* %s Is now playing\n", STRING( pPlayer->pev->netname ));
			UTIL_SayTextAll( text, pPlayer );
		}

			
		
		
	}
	
	
	//DME END

	else if ( FStrEq(pcmd, "fullupdate" ) )
	{
		GetClassPtr((CBasePlayer *)pev)->ForceClientDllUpdate(); 
	}
	else if ( FStrEq(pcmd, "give" ) )
	{
		if ( g_flWeaponCheat != 0.0)
		{
			int iszItem = ALLOC_STRING( CMD_ARGV(1) );	// Make a copy of the classname
			GetClassPtr((CBasePlayer *)pev)->GiveNamedItem( STRING(iszItem) );
		}
	}
	else if ( FStrEq(pcmd, "_firstspawn" ) )
	{
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);

		if ( pPlayer->m_bHadFirstSpawn == false && g_bHaveMOTD )
		{
			pPlayer->m_bHadFirstSpawn = true;
			pPlayer->Spawn();
		}
	}

/*	else if ( FStrEq(pcmd, "spectate" ) )
	{
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);

		if ( pPlayer->pev->flags & FL_PROXY )
		{
			pPlayer->Spawn();
				edict_t *pentSpawnSpot = g_pGameRules->GetPlayerSpawnSpot( pPlayer );
				pPlayer->StartObserver( pev->origin, VARS(pentSpawnSpot)->angles);
				//sprintf(text, "* %s Is now a spectator\n", STRING( pPlayer->pev->netname ));
				//UTIL_SayTextAll( text, pPlayer );
		}
	}*/
	
	else if ( FStrEq(pcmd, "drop" ) )
	{
		// player is dropping an item. 
		GetClassPtr((CBasePlayer *)pev)->DropPlayerItem((char *)CMD_ARGV(1));
	}
	else if ( FStrEq(pcmd, "settings" ) )
	{
		
	CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);		
			ShowGameInfo(pPlayer);
	}

	else if ( FStrEq(pcmd, "fov" ) )
	{
		if ( g_flWeaponCheat && CMD_ARGC() > 1)
		{
			GetClassPtr((CBasePlayer *)pev)->m_iFOV = atoi( CMD_ARGV(1) );
		}
		else
		{
			CLIENT_PRINTF( pEntity, print_console, UTIL_VarArgs( "\"fov\" is \"%d\"\n", (int)GetClassPtr((CBasePlayer *)pev)->m_iFOV ) );
		}
	}
	else if ( FStrEq(pcmd, "use" ) )
	{
		GetClassPtr((CBasePlayer *)pev)->SelectItem((char *)CMD_ARGV(1));
	}
	else if (((pstr = strstr(pcmd, "weapon_")) != NULL)  && (pstr == pcmd))
	{
		GetClassPtr((CBasePlayer *)pev)->SelectItem(pcmd);
	}
	else if ( FStrEq(pcmd, "help" ) )
	{
		
			CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
			ShowGameHelp(pPlayer);
	}

	else if ( FStrEq(pcmd, "gamemodehelp" ) )
	{
		
			CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
			ShowGameModes(pPlayer);
	}
	else if ( FStrEq(pcmd, "votinghelp" ) )
	{
		
			CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
			ShowGameVote(pPlayer);
	}	
	else if ( FStrEq(pcmd, "report" ) )
	{	
		CBaseEntity *pEntity = NULL;
		float radius = 100;
		char item_name[64];
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
		
		while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, radius )) != NULL)
		{
			strcpy(item_name, STRING(pEntity->pev->classname));
			if (strcmp(item_name, "weapon_supershotgun") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [SSG]\n");
			}
			else if (strcmp(item_name, "weapon_supernailgun") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [SNG]\n");
			}		
			else if (strcmp(item_name, "weapon_grenadelauncher") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [GL]\n");
			}		
			else if (strcmp(item_name, "weapon_rocketlauncher") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [RL]\n");
			}
			else if (strcmp(item_name, "weapon_lightning") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [LG]\n");
			}
			else if (strcmp(item_name, "item_artifact_invisibility") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [INVIS]\n");
			}
			else if (strcmp(item_name, "item_artifact_invulnerability") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [PENT]\n");
			}
			else if (strcmp(item_name, "item_artifact_super_damage") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [QUAD]\n");
			}
			else if (strcmp(item_name, "item_armor1") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [GA]\n");
			}
			else if (strcmp(item_name, "item_armor3") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [RA]\n");
			}
			else if (strcmp(item_name, "item_armorInv") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [RA]\n");
			}
			else if (strcmp(item_name, "item_armor2") == 0)
			{
				CLIENT_COMMAND(pPlayer->edict(), "say_team ** AT [YA]\n");
			}
		}
	

	}
	


	else if (FStrEq(pcmd, "lastinv" ))
	{
		GetClassPtr((CBasePlayer *)pev)->SelectLastItem();
	}

	//DME
	else if ( FStrEq(pcmd, "vguimenu" ) )
	{
		if (CMD_ARGC() >= 1)
			GetClassPtr((CBasePlayer *)pev)->ShowVGUIMenu(atoi(CMD_ARGV(1)));
	}

		
	else if (FStrEq(pcmd, "+hook" )) 
	{ 
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
		//if ( CVAR_GET_FLOAT( "disable_grapple" ) == 0 && (!pPlayer->IsObserver() ) && (pPlayer->pev->deadflag != DEAD_RESPAWNABLE ))
		if (pev->deadflag == DEAD_RESPAWNABLE || pev->deadflag == DEAD_DYING || pev->deadflag == DEAD_DEAD || pev->movetype == MOVETYPE_NOCLIP || !pPlayer->m_bHadFirstSpawn && g_bHaveMOTD || grappledisable.value > 0)
			return;
		GetClassPtr((CBasePlayer *)pev)->handleHookButton( TRUE ); 
	} 
	else if (FStrEq(pcmd, "-hook" )) 
	{ 
		if ( CVAR_GET_FLOAT( "disable_grapple" ) == 0 )
		GetClassPtr((CBasePlayer *)pev)->handleHookButton( FALSE ); 
	}
	
	// Oz maddog message end
	// Oz runes - drop command
	else if (FStrEq(pcmd, "droprune" ))
	{
		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);
		if (pPlayer->ctf_hasrune)
		{
			char szText[201];
			hudtextparms_t     hText;
			/*if (pPlayer->ctf_hasboot)
				sprintf(szText, "Boots Rune Discarded!\n");
			else*/ if(pPlayer->ctf_hasfrag)
				sprintf(szText, "Frag Rune Discarded!\n");
			else if(pPlayer->ctf_hasregen)
				sprintf(szText, "Regeneration Rune Discarded!\n");
			else if(pPlayer->ctf_hasbar)
				sprintf(szText, "Crowbar Rune Discarded!\n");
			else if(pPlayer->ctf_hascycle)
				sprintf(szText, "Haste Rune Discarded!\n");
			else if(pPlayer->ctf_hascloak)
				sprintf(szText, "Cloaking Rune Discarded!\n");
			else if(pPlayer->ctf_hashook)
				sprintf(szText, "Super Hook Rune Discarded!\n");
			else if(pPlayer->ctf_hasprotect)
				sprintf(szText, "Protection Rune Discarded!\n");
			else if(pPlayer->ctf_haslowgrav)
			{
				sprintf(szText, "Low Gravity Rune Discarded!\n");
				pev->gravity = 1;
			}
			else if(pPlayer->ctf_hasvamp)
				sprintf(szText, "Vampire Rune Discarded!\n");
			else if(pPlayer->ctf_hastrap)
				sprintf(szText, "Booby Trap Rune Discarded!\n");
			//jammer superglock
			else if(pPlayer->ctf_hassuperglock)
				sprintf(szText, "Super Glock Rune Discarded!\n");
			else
				sprintf(szText, "Rune Discarded!\n");
	
			memset(&hText, 0, sizeof(hText));
			hText.channel = 4;
			// These X and Y coordinates are just above
			//  the health meter.
			hText.x = 0.04;
			hText.y = 0.80;
    
			hText.effect = 0;    // Fade in/out
						
			hText.r1 = 230;
			hText.g1 = 230; 
			hText.b1 = 230;
			hText.a1 = 255;
			
			hText.r2 = hText.g2 = hText.b2 = 255;
			hText.a2 = 255;
			
			hText.fadeinTime = 0;
			hText.fadeoutTime = 0;
			hText.holdTime = 1;
			hText.fxTime = 0.5;
			
			UTIL_HudMessage(pPlayer ,hText, szText);
		}

		if (pPlayer->ctf_hasrune && pPlayer->ctf_hasfrag)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hasfrag = false;

			pPlayer->SpawnRunes("ctf_frag");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hasregen)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hasregen = false;

			pPlayer->SpawnRunes("ctf_regen");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hasbar)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hasbar = false;

			pPlayer->SpawnRunes("ctf_crowbar");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hascycle)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hascycle = false;

			pPlayer->SpawnRunes("ctf_cycle");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hascloak)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hascloak = false;
			pev->rendermode = kRenderNormal;
	
			if (pPlayer->ctf_hasblueflag || pPlayer->ctf_hasredflag)
				pev->renderfx |= kRenderFxGlowShell;
			else
				pev->renderfx &= ~kRenderFxGlowShell;

			pPlayer->SpawnRunes("ctf_cloak");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hashook)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hashook = false;

			pPlayer->SpawnRunes("ctf_hook");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hasprotect)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hasprotect = false;

			pPlayer->SpawnRunes("ctf_protect");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_haslowgrav)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_haslowgrav = false;
			pev->gravity = 1;
	
			pPlayer->SpawnRunes("ctf_lowgrav");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hasvamp)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hasvamp = false;
	
			pPlayer->SpawnRunes("ctf_vamp");
		}
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hastrap)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hastrap = false;

			pPlayer->SpawnRunes("ctf_trap");
		}
		//jammer new superglock
		if (pPlayer->ctf_hasrune && pPlayer->ctf_hassuperglock)
		{
			pPlayer->ctf_hasrune = false;
			pPlayer->ctf_hassuperglock = false;

			pPlayer->SpawnRunes("ctf_superglock");
		}

	}
	// Oz runes - drop command end
	// Oz runes - current rune
	else if (FStrEq(pcmd, "runestatus" ))
	{
		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);

		char szText[201];//wsock32.lib and winmm.lib
		hudtextparms_t     hText;
		/*if(pPlayer->ctf_hasboot)
			sprintf(szText, "Boots Rune\n");
		else*/ if(pPlayer->ctf_hasfrag)
			sprintf(szText, "Frag Rune\n");
		else if(pPlayer->ctf_hasregen)
			sprintf(szText, "Regeneration Rune\n");
		else if(pPlayer->ctf_hasbar)
			sprintf(szText, "Crowbar Rune\n");
		else if(pPlayer->ctf_hascycle)
			sprintf(szText, "Haste Rune\n");
		else if(pPlayer->ctf_hascloak)
			sprintf(szText, "Cloaking Rune\n");
		else if(pPlayer->ctf_hashook)
			sprintf(szText, "Super Hook Rune\n");
		else if(pPlayer->ctf_hasprotect)
			sprintf(szText, "Protection Rune\n");
		else if(pPlayer->ctf_haslowgrav)
			sprintf(szText, "Low Gravity Rune\n");
		else if(pPlayer->ctf_hasvamp)
			sprintf(szText, "Vampire Rune\n");
		else if(pPlayer->ctf_hastrap)
			sprintf(szText, "Booby Trap Rune\n");
		//jammer superglock
		else if(pPlayer->ctf_hassuperglock)
			sprintf(szText, "Super Glock Rune\n");
		else
			sprintf(szText, "You don't have a Rune\n");
	
		memset(&hText, 0, sizeof(hText));
		hText.channel = 4;
		// These X and Y coordinates are just above
		//  the health meter.
		hText.x = 0.04;
		hText.y = 0.80;
    
		hText.effect = 0;    // Fade in/out
					
		hText.r1 = 230;
		hText.g1 = 230; 
		hText.b1 = 230;
		hText.a1 = 255;
			
		hText.r2 = hText.g2 = hText.b2 = 255;
		hText.a2 = 255;
		
		hText.fadeinTime = 0;
		hText.fadeoutTime = 0;
		hText.holdTime = 1;
		hText.fxTime = 0.5;
			
		UTIL_HudMessage(pPlayer ,hText, szText);
	}
	// Oz runes - current rune end
	// Oz CTF - flag dropping command
	else if ( FStrEq(pcmd, "throwflag" ) )
	{

		//jammer new ctf drop flag
		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);

		if(pPlayer->ctf_hasredflag || pPlayer->ctf_hasblueflag)
		{
	
			Vector VecHere = pPlayer->pev->origin + gpGlobals->v_forward*50;
			TraceResult tr;
			
			UTIL_TraceLine( pPlayer->pev->origin + gpGlobals->v_forward * 16, pPlayer->pev->origin + gpGlobals->v_forward * 128, dont_ignore_monsters, NULL, &tr );

			if(tr.fAllSolid == 0 && tr.fStartSolid == 0 && tr.flFraction > .75)
			{
				char *flag = NULL;

				if(pPlayer->ctf_hasredflag)
				{	
					//jammer ctf flag code
				 pPlayer->ctf_hasredflag = false;
				 gRedFlagDroppedInTransit = true;//dropped it not at home
				 STOP_SOUND( ENT(gRedFlag), CHAN_AUTO, "ctf/flagtk.wav" );
				 gRedFlag->flags = FL_KILLME;
				 flag = "ctf_redflag";
				}
				else
				{
					//jammer ctf flag code
				 pPlayer->ctf_hasblueflag = false;
				 gBlueFlagDroppedInTransit = true;//dropped it not at home
				 STOP_SOUND( ENT(gBlueFlag), CHAN_AUTO, "ctf/flagtk.wav" );
				 gBlueFlag->flags = FL_KILLME;
				 flag = "ctf_blueflag";
				}

				CBaseEntity *pEntity = CBaseEntity::Create( flag, VecHere, pPlayer->pev->v_angle, pPlayer->edict());
				pEntity->pev->velocity = gpGlobals->v_forward*150;
				pEntity->pev->velocity.z += 150;
				pEntity->pev->angles.x = 0;
				pEntity->pev->angles.y = pPlayer->pev->angles.y;
				pEntity->pev->angles.z = 0;
				
			}
			else
			{
				hudtextparms_t     hText;
				memset(&hText, 0, sizeof(hText));
				hText.channel = 4;
				hText.x = 0.04;
				hText.y = 0.80;
				hText.effect = 0;    // Fade in/out
				hText.r1 = 230;
				hText.g1 = 230; 
				hText.b1 = 230;
				hText.a1 = 255;
							
				hText.r2 = hText.g2 = hText.b2 = 255;
				hText.a2 = 255;
						
				hText.fadeinTime = 0;
				hText.fadeoutTime = 0;
				hText.holdTime = 1;
				hText.fxTime = 0.5;
				UTIL_HudMessage(pPlayer,hText,"Too close to the wall to drop flag!");
			}
		}//end if has flags
	}
	// Oz CTF - flag dropping command
	else if ( g_pGameRules->ClientCommand( GetClassPtr((CBasePlayer *)pev), pcmd ) )
	{
		// MenuSelect returns true only if the command is properly handled,  so don't print a warning
	}
	else if ( FStrEq( pcmd, "specmode" )  )	// new spectator mode
	{
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
		if ( pPlayer->IsObserver() )
			pPlayer->Observer_SetMode( atoi( CMD_ARGV(1) ) );
	}
	else if ( FStrEq( pcmd, "follownext" )  )	// follow next player
	{
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
		if ( pPlayer->IsObserver() )
		{
				ClientPrint( &pEntity->v, HUD_PRINTCONSOLE, "About to look for the next player");
			pPlayer->Observer_FindNextPlayer(!!atoi( CMD_ARGV(1) ));
		}
	}
	else
	{
		// tell the user they entered an unknown command
		if (!( FStrEq(pcmd, "32" ) ))	ClientPrint( &pEntity->v, HUD_PRINTCONSOLE, UTIL_VarArgs( "Unknown command: %s\n", pcmd ) );
	}
}


/*
========================
ClientUserInfoChanged

called after the player changes
userinfo - gives dll a chance to modify it before
it gets sent into the rest of the engine.
========================
*/
void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer )
{
	// Is the client spawned yet?
	if ( !pEntity->pvPrivateData )
		return;

	// msg everyone if someone changes their name,  and it isn't the first time (changing no name to current name)
	if ( pEntity->v.netname && STRING(pEntity->v.netname)[0] != 0 && !FStrEq( STRING(pEntity->v.netname), g_engfuncs.pfnInfoKeyValue( infobuffer, "name" )) )
	{
		char text[256];
		sprintf( text, "* %s changed name to %s\n", STRING(pEntity->v.netname), g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ) );
		MESSAGE_BEGIN( MSG_ALL, gmsgSayText, NULL );
			WRITE_BYTE( ENTINDEX(pEntity) );
			WRITE_STRING( text );
		MESSAGE_END();

		UTIL_LogPrintf( "\"%s<%i><%s><%i>\" changed name to \"%s\"\n", 
			STRING( pEntity->v.netname ), 
			GETPLAYERUSERID( pEntity ), 
			GETPLAYERAUTHID( pEntity ),
			GETPLAYERUSERID( pEntity ), 
			g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ) );
	}

	// QUAKECLASSIC
	// Weapon switching behaviour
	((CBasePlayer*)pEntity)->m_iWeaponSwitch = 8;
	char *st = g_engfuncs.pfnInfoKeyValue( infobuffer, "w_switch" );
	if (st && st[0])
	{
		((CBasePlayer*)pEntity)->m_iWeaponSwitch = atoi(st);
	}
	((CBasePlayer*)pEntity)->m_iBackpackSwitch = 8;
	st = g_engfuncs.pfnInfoKeyValue( infobuffer, "b_switch" );
	if (st && st[0])
	{
		((CBasePlayer*)pEntity)->m_iBackpackSwitch = atoi(st);
	}

	g_pGameRules->ClientUserInfoChanged( GetClassPtr((CBasePlayer *)&pEntity->v), infobuffer );
	
}

static int g_serveractive = 0;

void ServerDeactivate( void )
{
	// It's possible that the engine will call this function more times than is necessary
	//  Therefore, only run it one time for each call to ServerActivate 
	if ( g_serveractive != 1 )
	{
		return;
	}

	g_serveractive = 0;

	// Peform any shutdown operations here...
	//
}

// DME++
void LaunchFFA(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "ffa")) 
	{
		sprintf(sum, "exec modes/ffa/ffa.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode ffa\n");
		SERVER_COMMAND (szText);
//		sprintf (szText, "restart\n");
//		SERVER_COMMAND (szText);
		SERVER_EXECUTE( );
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//GoToIntermission();
//		SERVER_COMMAND( buf );
	}
		 
}

void LaunchTEAM(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "team")) 
	{	
		sprintf(sum, "exec modes/team/team.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode team\n");
		SERVER_COMMAND (szText);				
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}

//MAC++
void Launch1on1(void)

{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;


	if  (!FStrEq(gamemode.string, "1on1")) 
	{	
		sprintf(sum, "exec modes/1on1/1on1.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode 1on1\n");
		SERVER_COMMAND (szText);

		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
		
	}
		 
}
void Launch2on2(void)

{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "2on2")) 
	{	
		sprintf(sum, "exec modes/2on2/2on2.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode 2on2\n");
		SERVER_COMMAND (szText);
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}
void Launch3on3(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "3on3")) 
	{	
		sprintf(sum, "exec modes/3on3/3on3.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode 3on3\n");
		SERVER_COMMAND (szText);
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}
void Launch4on4(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "4on4")) 
	{	
		sprintf(sum, "exec modes/4on4/4on4.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode 4on4\n");
		SERVER_COMMAND (szText);
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}

void LaunchLMS(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "lms")) 
	{	
		sprintf(sum, "exec modes/lms/lms.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode lms\n");
		SERVER_COMMAND (szText);
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}


void LaunchMIDAIR(void)
{	
	char sum[64];
	*sum=NULL;
	char szText [256];
	char buf[64];
	*buf=NULL;

	if  (!FStrEq(gamemode.string, "midair")) 
	{	
		sprintf(sum, "exec modes/midair/midair.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode midair\n");
		SERVER_COMMAND (szText);
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}

void LaunchDMM4(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "dmm4")) 
	{	
		sprintf(sum, "exec modes/dmm4/dmm4.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode dmm4\n");
		SERVER_COMMAND (szText);				
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}


void LaunchROCKETARENA(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "arena")) 
	{	
		sprintf(sum, "exec modes/arena/arena.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode arena\n");
		SERVER_COMMAND (szText);
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}

void LaunchPREMATCH(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "standby")) 
	{	
		sprintf(sum, "exec modes/standby/standby.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode standby\n");
		SERVER_COMMAND (szText);				
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}

void LaunchRUMBLE(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "rumble")) 
	{	
		sprintf(sum, "exec modes/rumble/rumble.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode rumble\n");
		SERVER_COMMAND (szText);				
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}

void LaunchCTF(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "ctf")) 
	{	
		sprintf(sum, "exec modes/ctf/ctf.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode ctf\n");
		SERVER_COMMAND (szText);				
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}

void LaunchCLANARENA(void)
{	
	char szText [256];
	char buf[64];
	*buf=NULL;
	char sum[64];
	*sum=NULL;

	if  (!FStrEq(gamemode.string, "clanarena")) 
	{	
		sprintf(sum, "exec modes/clanarena/clanarena.cfg\n");
		SERVER_COMMAND( sum );
		SERVER_EXECUTE( );

		sprintf (szText, "gamemode clanarena\n");
		SERVER_COMMAND (szText);
		sprintf (szText, "restart\n");
		SERVER_COMMAND (szText);
		SERVER_EXECUTE( );
		CVAR_SET_STRING("changedmode","1");
//		sprintf(buf, "changelevel %s\n",default_map.string);
//		SERVER_COMMAND( buf );
	}
		 
}

void StartMatch(void)
{	
	char szText [256];

		sprintf (szText, "startmatch 1\n");
		SERVER_COMMAND (szText);
		
		 
}

void StopMatch(void)
{	
	char szText [256];

		sprintf (szText, "startmatch 0\n");
		SERVER_COMMAND (szText);
		
		 
}

void RandomMap(void)
{
	char* maplist[12];
	maplist[0]="aerolite";
	maplist[1]="dmc_dm2";
	maplist[2]="dmc_dmz1";
	maplist[3]="dmc_dm4";
	maplist[4]="dmc_dm6";
	maplist[5]="dmc_skull";
	maplist[6]="dmc_rwild";
	maplist[7]="dmc_ultrav";
	maplist[8]="dmc_ztndm3";
	maplist[9]="dmc_qcon1";
	maplist[10]="dmc_pkeg1";
	maplist[11]="dmc_zed";

	char szText [256];
	sprintf (szText,"changelevel %s\n",maplist[RANDOM_LONG(0,11)]);
	SERVER_COMMAND(szText);
}

// DME--

void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
	int				i;
	CBaseEntity		*pClass;

	//mfrx 27FEB06
	g_runes_exist = false;	//so that runes spawn on each map
	//mfrx
	// DME++
	if(!gpGlobals->addedCommands)
	{

	g_engfuncs.pfnAddServerCommand("ffa", LaunchFFA);
	g_engfuncs.pfnAddServerCommand("team", LaunchTEAM);
	g_engfuncs.pfnAddServerCommand("arena", LaunchROCKETARENA);	
	g_engfuncs.pfnAddServerCommand("1on1", Launch1on1);//MAC
	g_engfuncs.pfnAddServerCommand("2on2", Launch2on2);
	g_engfuncs.pfnAddServerCommand("3on3", Launch3on3);
	g_engfuncs.pfnAddServerCommand("4on4", Launch4on4);
	g_engfuncs.pfnAddServerCommand("lms", LaunchLMS);
	g_engfuncs.pfnAddServerCommand("clanarena", LaunchCLANARENA);
	g_engfuncs.pfnAddServerCommand("midair", LaunchMIDAIR);
	g_engfuncs.pfnAddServerCommand("dmm4", LaunchDMM4);
	g_engfuncs.pfnAddServerCommand("rumble", LaunchRUMBLE);	
	g_engfuncs.pfnAddServerCommand("standby", LaunchPREMATCH);
	g_engfuncs.pfnAddServerCommand("ctf", LaunchCTF);
	g_engfuncs.pfnAddServerCommand("matchstart", StartMatch);
	g_engfuncs.pfnAddServerCommand("break", StopMatch);
	g_engfuncs.pfnAddServerCommand("random", RandomMap);
    gpGlobals->addedCommands=true;
	}

	// Every call to ServerActivate should be matched by a call to ServerDeactivate
	g_serveractive = 1;
	//jammer begin
	NumPlayerStarts = 0; //set to 0 or else blamo
	//jammer ctf
	RedPlayerStarts = 0;
	BluePlayerStarts = 0;

	CBaseEntity *pOther = NULL;
	while((pOther = UTIL_FindEntityByClassname(pOther,"info_player_deathmatch")) != NULL)
	{
		NumPlayerStarts++;
	}

	if(ctf.value)
	{
	  while((pOther = UTIL_FindEntityByClassname(pOther,"ctf_redspawn")) != NULL)
		{
		  RedPlayerStarts++;
		}
	  while((pOther = UTIL_FindEntityByClassname(pOther,"ctf_bluespawn")) != NULL)
		{
		  BluePlayerStarts++;
		}
	}


/*	//reading in the dat file
    if(FoundFile)//found file. set in world.cpp
	{
		CheckDisabledWeapons();//what weapons can we NOT spawn

		if((access(DatFileName,4)) == 0)//found actual file, just make sure
		{
			FoundFile = FALSE;//set to false so my items can respawn, if this left true nothing will repsawn

			if((fp = fopen(DatFileName,"rb")) != NULL)
			{
				//start reading in structs
				while(fread(&FileItemData.data,sizeof(item_t),1,fp) != NULL)
				{
					//actual spawn func
					SpawnItem(FileItemData.data.posx,FileItemData.data.posy,
				       FileItemData.data.posz,FileItemData.data.angle,
					   FileItemData.data.weapon);
				}//end while

				fclose(fp);
			}//end if fopen
		}//end if access
	}//end if foundfile
	//jammer end
*/

	// Clients have not been initialized yet
	for ( i = 0; i < edictCount; i++ )
	{
		if ( pEdictList[i].free )
			continue;
		
		// Clients aren't necessarily initialized until ClientPutInServer()
		if ( i < clientMax || !pEdictList[i].pvPrivateData )
			continue;

		pClass = CBaseEntity::Instance( &pEdictList[i] );
		// Activate this entity if it's got a class & isn't dormant
		if ( pClass && !(pClass->pev->flags & FL_DORMANT) )
		{
			pClass->Activate();
		}
		else
		{
			ALERT( at_console, "Can't instance %s\n", STRING(pEdictList[i].v.classname) );
		}
	}

	// Link user messages here to make sure first client can get them...
	LinkUserMessages();
}


/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
void PlayerPreThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->PreThink( );
}

/*
================
PlayerPostThink

Called every frame after physics are run
================
*/
void PlayerPostThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);
	CBaseEntity *plr = (CBaseEntity *)GET_PRIVATE(pEntity);
	//MAC++

	//mfrx 27FEB06
	//got a bug with quad drop and the bots
	//pPlayer is null when quad-holder dies
	//lets check for it
	/*if (!pPlayer)
	{
		ALERT(at_console, "PlayerPostThink: Null Player\n");
		return;
	}*/
	if (pPlayer)
	{
		pPlayer->PostThink( );
		//moved the oz stuff after the player check
	if(pPlayer->m_bHadFirstSpawn && g_bHaveMOTD)
	{

		// Oz messaging
		if (pPlayer->m_iMess < 27)
			pPlayer->m_iMess++;

		if (pPlayer->m_iCommands < 27)
			pPlayer->m_iCommands++;

		if (pPlayer->m_iStart < 27)
			pPlayer->m_iStart++;

		if (pPlayer->m_iStart == 26)
		{
			char szText[201];
			hudtextparms_t     hText;

			sprintf(szText, "Gamemode is : %s\n", CVAR_GET_STRING("gamemode"));
				
			memset(&hText, 0, sizeof(hText));
			hText.channel = 1;
			// These X and Y coordinates are just above
			//  the health meter.
			hText.x = 0.01;
			hText.y = 0.85;
		    
			hText.effect = 0;    // Fade in/out
			            
			hText.r1 = hText.g1 = hText.b1 = 255;
			hText.a1 = 255;
			
			hText.r2 = hText.g2 = hText.b2 = 255;
			hText.a2 = 255;
			
			hText.fadeinTime = 1;
			hText.fadeoutTime = 5;
			hText.holdTime = 6;
			hText.fxTime = 0.5;
			
			//UTIL_HudMessageAll(hText, szText);
			UTIL_HudMessage(pPlayer ,hText, szText);
			pPlayer->m_iStart = 27;
		}

		if (pPlayer->m_iStart == 25)
		{
			char szText[201];
			hudtextparms_t     hText;

			sprintf(szText, "Gamemode is : %s\n", CVAR_GET_STRING("gamemode"));
				
			memset(&hText, 0, sizeof(hText));
			hText.channel = 1;
			// These X and Y coordinates are just above
			//  the health meter.
			hText.x = 0.01;
			hText.y = 0.85;
		    
			hText.effect = 0;    // Fade in/out
			            
			hText.r1 = hText.g1 = hText.b1 = 255;
			hText.a1 = 255;
			
			hText.r2 = hText.g2 = hText.b2 = 255;
			hText.a2 = 255;
			
			hText.fadeinTime = 1;
			hText.fadeoutTime = 5;
			hText.holdTime = 6;
			hText.fxTime = 0.5;
			
			//UTIL_HudMessageAll(hText, szText);
			UTIL_HudMessage(pPlayer ,hText, szText);
			pPlayer->m_iStart = 26;
		}

		if (pPlayer->m_iCommands == 25)
		{
			//mfrx 14FEB05
			char szInfo[255];
			char szTemp[100];
			sprintf(szTemp, "//---------------------------------------------//\n");
			strcat(szInfo, szTemp);
			sprintf(szTemp, "Custom Server Information by mfrx ;-)\n");
			strcat(szInfo, szTemp);
			ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, szInfo, NULL, NULL, NULL, NULL);
			if (CVAR_GET_FLOAT("disable_grapple") == 1)
			{
				sprintf(szInfo, "Grappling Hook is OFF\n");
			}
			else
			{
				sprintf(szInfo, "Grappling Hook is ON\n");
			}
			ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, szInfo, NULL, NULL, NULL, NULL);
			sprintf(szInfo, "'Pentagram of Protection' respawns after %d seconds and lasts for %d seconds\n", 
				(int)CVAR_GET_FLOAT("respawntime_666"), (int)CVAR_GET_FLOAT("duration_666"));
			ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, szInfo, NULL, NULL, NULL, NULL);
			sprintf(szInfo, "'Quad Damage' respawns after %d seconds and lasts for %d seconds\n", 
				(int)CVAR_GET_FLOAT("respawntime_quad"), (int)CVAR_GET_FLOAT("duration_quad"));
			ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, szInfo, NULL, NULL, NULL, NULL);
			sprintf(szInfo, "'Ring of Shadows' respawns after %d seconds and lasts for %d seconds\n", 
				(int)CVAR_GET_FLOAT("respawntime_eyes"), (int)CVAR_GET_FLOAT("duration_eyes"));
			ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, szInfo, NULL, NULL, NULL, NULL);
			sprintf(szInfo, "'Super Shotgun' will fire %d shots (14 is standard)\n", 
				(int)CVAR_GET_FLOAT("ssg_shots"));
			sprintf(szTemp, "//---------------------------------------------//\n");
			strcat(szInfo, szTemp);
			ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, szInfo, NULL, NULL, NULL, NULL);
			//mfrx

			char szText[201];
			hudtextparms_t     hText;
			
			
			sprintf(szText, "Welcome to %s\n\nRunning DMQ by Pix\n\nBased on DMC Pro 2 by TrenD*KilleR\n\nCheck in your console for respawns", CVAR_GET_STRING("hostname"));
			
			// ctf and grapple
		/*	if ((ctf.value) && (grappledisable.value < 1))
				sprintf(szText, "DMC Pro v1.0\n\nCTF\n\n+hook - Fires the offhand grappling hook\nthrowflag - Drops the flag you are carrying\nchangeteam - Shows the teamchange menu");
			// ctf
			else if ((ctf.value) && (grappledisable.value))
				sprintf(szText, "DMC Pro v1.0\n\nCTF\n\nthrowflag - Drops the flag you are carrying\n");
			// team and grapple
			else if((FStrEq(gamemode.string, "team") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\n\nTeamDM\n\n+hook - Fires the offhand grappling hook\nchangeteam - Shows the change team menu\n");
			// team
			else if ((FStrEq(gamemode.string, "team") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\n\nTeamDM\n\nchangeteam - Shows the change team menu\n");
			// duel & grapple
			else if ((FStrEq(gamemode.string, "duel") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\nDuel Mode\n+hook - Fires the offhand grappling hook\nmenu - Displays Duel Mode Menu\n");
				//ShowGameInfoDuel(pPlayer);
				// duel
			else if ((FStrEq(gamemode.string, "duel") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\nDuel Mode\nmenu - Displays Duel Mode Menu\n");
				//ShowGameInfoDuel(pPlayer);
				// rocketarena & grapple
			else if ((FStrEq(gamemode.string, "rocketarena") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\n\nRocket Arena\n\n+hook - Fires the offhand grappling hook\nbreak - Remove yourself from play\nready - Add yourself back into play\n");
			// rocketarena
			else if ((FStrEq(gamemode.string, "rocketarena") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\n\nRocket Arena\n\nbreak - Remove yourself from play\nready - Add yourself back into play\n");
			// midair and grapple
			else if((FStrEq(gamemode.string, "midair") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\n\nMidair\n\n+hook - Fires the offhand grappling hook\nbreak - Remove yourself from play\nready - Add yourself back into play\n");
			// midair
			else if ((FStrEq(gamemode.string, "midair") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\nMidair\n\nYou are on the Red team\n\nbreak - Remove yourself from play\nready - Add yourself back into play\n");
			// dmm4 & grapple
			else if ((FStrEq(gamemode.string, "dmm4") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\n\nDeathmatch Mode 4\n\n+hook - Fires the offhand grappling hook\n");
			// dmm4
			else if ((FStrEq(gamemode.string, "dmm4") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\n\nDeathmatch Mode 4\n");
			// lms & grapple
			else if ((FStrEq(gamemode.string, "lms") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\n\nLast Man Standing\n\n+hook - Fires the offhand grappling hook\n");
			// lms
			else if ((FStrEq(gamemode.string, "lms") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\n\nLast Man Standing\n");
			// excessive & grapple
			else if ((FStrEq(gamemode.string, "excessive") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\n\nExcessive\n\n+hook - Fires the offhand grappling hook\n");
			// excessive
			else if ((FStrEq(gamemode.string, "excessive") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\n\nExcessive\n");
			// ffa & grapple
			else if ((FStrEq(gamemode.string, "ffa") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\n\nFFA\n\n+hook - Fires the offhand grappling hook\n");
			// ffa
			else if ((FStrEq(gamemode.string, "ffa") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\n\nFFA\n");
			// instagib & grapple
			else if ((FStrEq(gamemode.string, "instagib") && (grappledisable.value < 1)))
				sprintf(szText, "DMC Pro v1.0\n\nInstagib\n\n+hook - Fires the offhand grappling hook\n");
			// instagib
			else if ((FStrEq(gamemode.string, "instagib") && (grappledisable.value)))
				sprintf(szText, "DMC Pro v1.0\\n");
			
			
			else
				sprintf(szText, "\n");*/

		char buf[1024];	
		float fLength = (float)strlen(buf);
		float fOffset = fLength*=0.0035;
		hudtextparms_t      h_text;
		h_text.x            = 0.375-fOffset;
		h_text.y            = 0.45f;
		h_text.a1           = 0.0f;
		h_text.a2           = 0.0f;
		h_text.b1           = 30.0f;
		h_text.b2           = 255.0f;
		h_text.g1           = 85.0f;
		h_text.g2           = 255.0f;
		h_text.r1           = 255.0f;
		h_text.r2           = 255.0f;
		h_text.channel      = 4;
		h_text.effect       = 2;
		h_text.fxTime       = 0.5;
		h_text.fadeinTime   = 0.01;
		h_text.fadeoutTime  = 2;    
		h_text.holdTime     = 4;
			memset(&hText, 0, sizeof(hText));
			/*hText.channel = 4;
			// These X and Y coordinates are just above
			//  the health meter.
			hText.x = 0.01;
			hText.y = 0.01; //60
		    
			hText.effect = 0;    // Fade in/out
			            
			hText.r1 = hText.g1 = hText.b1 = 255;
			hText.a1 = 255;
			
			hText.r2 = hText.g2 = hText.b2 = 255;
			hText.a2 = 255;
			
			hText.fadeinTime = 1;
			hText.fadeoutTime = 5;
			hText.holdTime = 60;
			hText.fxTime = 0.5;*/
			//float fLength = (float)strlen(szText);
					//float fOffset = fLength*=0.0035;
					//h_text.x = 0.5-fOffset;
					UTIL_HudMessage(pPlayer ,h_text, szText);
			//UTIL_HudMessageAll(hText, szText);
		//	UTIL_HudMessage(pPlayer ,hText, szText);
			pPlayer->m_iCommands = 26;
		}

		if (pPlayer->m_iOz == 25)
		{
			char szText[201];
			hudtextparms_t     hText;
		
			if (plr->ctf_team == 1 && ctf.value > 0)
				sprintf(szText, "You are on the Red team\n");
			else if (plr->ctf_team == 2 && ctf.value > 0)
				sprintf(szText, "You are on the Blue team\n");
			else if (plr->team == 1 && (FStrEq(gamemode.string, "team")))
				sprintf(szText, "You are on the Red team\n");
			else if (plr->team == 2 && (FStrEq(gamemode.string, "team")))
				sprintf(szText, "You are on the Blue team\n");
			else
				sprintf(szText, "\n");
			
				
			memset(&hText, 0, sizeof(hText));
			hText.channel = 2;
			// These X and Y coordinates are just above
			//  the health meter.
			hText.x = 0.01;
			hText.y = 0.10;
		    
			hText.effect = 0;    // Fade in/out
			            
			hText.r1 = hText.g1 = hText.b1 = 255;
			hText.a1 = 255;
			
			hText.r2 = hText.g2 = hText.b2 = 255;
			hText.a2 = 255;
			
			hText.fadeinTime = 1;
			hText.fadeoutTime = 5;
			hText.holdTime = 7;
			hText.fxTime = 0.5;
			
			//UTIL_HudMessageAll(hText, szText);
			UTIL_HudMessage(pPlayer ,hText, szText);

			pPlayer->m_iOz = 26;
		}
		// Oz messaging end
		}
	}

}



void ParmsNewLevel( void )
{
}


void ParmsChangeLevel( void )
{
	// retrieve the pointer to the save data
	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;

	if ( pSaveData )
		pSaveData->connectionCount = BuildChangeList( pSaveData->levelList, MAX_LEVEL_CONNECTIONS );
}


//
// GLOBALS ASSUMED SET:  g_ulFrameCount
//
void StartFrame( void )
{
	if ( g_pGameRules )
		g_pGameRules->Think();

	if ( g_fGameOver )
		return;

	gpGlobals->teamplay = CVAR_GET_FLOAT("teamplay");
	g_iSkillLevel = CVAR_GET_FLOAT("skill");
	g_ulFrameCount++;
}


void ClientPrecache( void )
{

	PRECACHE_SOUND("items/protect2.wav");//DME dmm4
	PRECACHE_SOUND("items/protect3.wav");//DME dmm4
	// setup precaches always needed
	PRECACHE_SOUND("player/sprayer.wav");			// spray paint sound for PreAlpha
	
	// PRECACHE_SOUND("player/pl_jumpland2.wav");		// UNDONE: play 2x step sound
	
	PRECACHE_SOUND("player/pl_fallpain2.wav");		
	PRECACHE_SOUND("player/pl_fallpain3.wav");		
	
	PRECACHE_SOUND("player/pl_step1.wav");		// walk on concrete
	PRECACHE_SOUND("player/pl_step2.wav");
	PRECACHE_SOUND("player/pl_step3.wav");
	PRECACHE_SOUND("player/pl_step4.wav");

	PRECACHE_SOUND("common/npc_step1.wav");		// NPC walk on concrete
	PRECACHE_SOUND("common/npc_step2.wav");
	PRECACHE_SOUND("common/npc_step3.wav");
	PRECACHE_SOUND("common/npc_step4.wav");

	PRECACHE_SOUND("player/pl_metal1.wav");		// walk on metal
	PRECACHE_SOUND("player/pl_metal2.wav");
	PRECACHE_SOUND("player/pl_metal3.wav");
	PRECACHE_SOUND("player/pl_metal4.wav");

	PRECACHE_SOUND("player/pl_dirt1.wav");		// walk on dirt
	PRECACHE_SOUND("player/pl_dirt2.wav");
	PRECACHE_SOUND("player/pl_dirt3.wav");
	PRECACHE_SOUND("player/pl_dirt4.wav");

	PRECACHE_SOUND("player/pl_duct1.wav");		// walk in duct
	PRECACHE_SOUND("player/pl_duct2.wav");
	PRECACHE_SOUND("player/pl_duct3.wav");
	PRECACHE_SOUND("player/pl_duct4.wav");

	PRECACHE_SOUND("player/pl_grate1.wav");		// walk on grate
	PRECACHE_SOUND("player/pl_grate2.wav");
	PRECACHE_SOUND("player/pl_grate3.wav");
	PRECACHE_SOUND("player/pl_grate4.wav");

	PRECACHE_SOUND("player/pl_slosh1.wav");		// walk in shallow water
	PRECACHE_SOUND("player/pl_slosh2.wav");
	PRECACHE_SOUND("player/pl_slosh3.wav");
	PRECACHE_SOUND("player/pl_slosh4.wav");

	PRECACHE_SOUND("player/pl_tile1.wav");		// walk on tile
	PRECACHE_SOUND("player/pl_tile2.wav");
	PRECACHE_SOUND("player/pl_tile3.wav");
	PRECACHE_SOUND("player/pl_tile4.wav");
	PRECACHE_SOUND("player/pl_tile5.wav");

	PRECACHE_SOUND("player/pl_swim1.wav");		// breathe bubbles
	PRECACHE_SOUND("player/pl_swim2.wav");
	PRECACHE_SOUND("player/pl_swim3.wav");
	PRECACHE_SOUND("player/pl_swim4.wav");

	PRECACHE_SOUND("player/pl_ladder1.wav");	// climb ladder rung
	PRECACHE_SOUND("player/pl_ladder2.wav");
	PRECACHE_SOUND("player/pl_ladder3.wav");
	PRECACHE_SOUND("player/pl_ladder4.wav");

	PRECACHE_SOUND("player/pl_wade1.wav");		// wade in water
	PRECACHE_SOUND("player/pl_wade2.wav");
	PRECACHE_SOUND("player/pl_wade3.wav");
	PRECACHE_SOUND("player/pl_wade4.wav");

	PRECACHE_SOUND("debris/wood1.wav");			// hit wood texture
	PRECACHE_SOUND("debris/wood2.wav");
	PRECACHE_SOUND("debris/wood3.wav");

	PRECACHE_SOUND("plats/train_use1.wav");		// use a train

	PRECACHE_SOUND("buttons/spark5.wav");		// hit computer texture
	PRECACHE_SOUND("buttons/spark6.wav");
	PRECACHE_SOUND("debris/glass1.wav");
	PRECACHE_SOUND("debris/glass2.wav");
	PRECACHE_SOUND("debris/glass3.wav");

	PRECACHE_SOUND( SOUND_FLASHLIGHT_ON );
	PRECACHE_SOUND( SOUND_FLASHLIGHT_OFF );

// player gib sounds
	PRECACHE_SOUND("common/bodysplat.wav");		               

// player pain sounds
	PRECACHE_SOUND("player/pain1.wav");
	PRECACHE_SOUND("player/pain2.wav");
	PRECACHE_SOUND("player/pain3.wav");
	PRECACHE_SOUND("player/pain4.wav");
	PRECACHE_SOUND("player/pain5.wav");
	PRECACHE_SOUND("player/pain6.wav");
	PRECACHE_SOUND("player/drown1.wav");
	PRECACHE_SOUND("player/drown2.wav");
	PRECACHE_SOUND("player/lburn1.wav");
	PRECACHE_SOUND("player/lburn2.wav");
	PRECACHE_SOUND("player/death1.wav");
	PRECACHE_SOUND("player/death2.wav");
	PRECACHE_SOUND("player/death3.wav");
	PRECACHE_SOUND("player/death4.wav");
	PRECACHE_SOUND("player/death5.wav");

	PRECACHE_SOUND("player/h2odeath.wav");
	
	PRECACHE_MODEL("models/player.mdl");

	// hud sounds

	PRECACHE_SOUND("common/wpn_hudoff.wav");
	PRECACHE_SOUND("common/wpn_hudon.wav");
	PRECACHE_SOUND("common/wpn_moveselect.wav");
	PRECACHE_SOUND("common/wpn_select.wav");
	PRECACHE_SOUND("common/wpn_denyselect.wav");

	PRECACHE_SOUND("player/gib.wav");

	PRECACHE_MODEL("models/gib_1.mdl");
	PRECACHE_MODEL("models/gib_2.mdl");
	PRECACHE_MODEL("models/gib_3.mdl");

	PRECACHE_SOUND("player/plyrjmp8.wav");

	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/p_crowbar.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/p_light.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/p_nail.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/p_nail2.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/p_rock.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/p_rock2.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/p_shot.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/p_shot2.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/spike.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/rocket.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/grenade.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/backpack.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/backpack.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/armor_g.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/armor_r.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/armor_y.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/armor_y.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/b_nail0.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/b_nail1.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/g_light.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/g_nail.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/g_nail2.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/g_rock.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/g_rock2.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/g_shot2.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/pow_invis.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/pow_quad.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/pow_invuln.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/suit.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_battery.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_batteryl.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_medkit.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_medkitl.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_medkits.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_rpgammo.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_rpgammo_big.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_shotbox.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ),"models/w_shotbox_big.mdl");

	g_sGibbed = PRECACHE_EVENT( 1, "events/gibs.sc" );
	g_sTeleport = PRECACHE_EVENT( 1, "events/teleport.sc" );
	g_sTrail = PRECACHE_EVENT( 1, "events/trail.sc" );
	g_sExplosion = PRECACHE_EVENT( 1, "events/explosion.sc" );
	g_usPowerUp			= PRECACHE_EVENT( 1, "events/powerup.sc" );

	if (giPrecacheGrunt)
		UTIL_PrecacheOther("monster_human_grunt");
}

/*
================
Sys_Error

Engine is going to shut down, allows setting a breakpoint in game .dll to catch that occasion
================
*/
void Sys_Error( const char *error_string )
{
	// Default case, do nothing.  MOD AUTHORS:  Add code ( e.g., _asm { int 3 }; here to cause a breakpoint for debugging your game .dlls
}

/*
===============
const char *GetGameDescription()

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
	if (FStrEq(gamemode.string, "1on1"))
		return "DMQ2 Match 1on1";
	else if (FStrEq(gamemode.string, "2on2"))
		return "DMQ2 Match 2on2";
	else if (FStrEq(gamemode.string, "3on3"))
		return "DMQ2 Match 3on3";
	else if (FStrEq(gamemode.string, "4on4"))
		return "DMQ2 Match 4on4";
	else if (FStrEq(gamemode.string, "dmm4"))
		return "DMQ2 Match DMM4";	
	else if (FStrEq(gamemode.string, "standby"))
		return "DMQ2 : Mode Standby";
	if ( g_pGameRules ) // this function may be called before the world has spawned, and the game rules initialized
		return g_pGameRules->GetGameDescription();
	else
		return "DMQ2 : Mode FFA";
}

/*
================
PlayerCustomization

A new player customization has been registered on the server
UNDONE:  This only sets the # of frames of the spray can logo
animation right now.
================
*/
void PlayerCustomization( edict_t *pEntity, customization_t *pCust )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (!pPlayer)
	{
		ALERT(at_console, "PlayerCustomization:  Couldn't get player!\n");
		return;
	}

	if (!pCust)
	{
		ALERT(at_console, "PlayerCustomization:  NULL customization!\n");
		return;
	}

	switch (pCust->resource.type)
	{
	case t_decal:
		pPlayer->SetCustomDecalFrames(pCust->nUserData2); // Second int is max # of frames.
		break;
	case t_sound:
	case t_skin:
	case t_model:
		// Ignore for now.
		break;
	default:
		ALERT(at_console, "PlayerCustomization:  Unknown customization type!\n");
		break;
	}
}

/*
================
SpectatorConnect

A spectator has joined the game
================
*/
void SpectatorConnect( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorConnect( );
}

/*
================
SpectatorConnect

A spectator has left the game
================
*/
void SpectatorDisconnect( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorDisconnect( );
}

/*
================
SpectatorConnect

A spectator has sent a usercmd
================
*/
void SpectatorThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorThink( );
}

//*****************************************************************
//jammer infinity spawn the item read from the file
//item spawn

void SpawnItem(int x, int y, int z,int angle,int type)
{
  int weapon , random;

 if(type == W_RANDOM)
   {
	 //for OZ only here, because oz diables weapons
	 if(AllWeaponsDisabled)//all weapons are disabled dooh!





























		 return;

	 weapon = RANDOM_LONG(0,(NUM_WEAPONS - 1));//- 1 because there is not a Weapons[13] only 12, 0 - 12
	 
	 while(Weapons[weapon].IsDisabled)//weapon is diabled so grab another one
	 {
	   weapon = RANDOM_LONG(0,(NUM_WEAPONS - 1));//if a lot of weapons are disable this could
	 }											  //take a while to get one
	 
	 random = TRUE; //is random
	 //ok if we get a 0 from the random above we need the first weapon
	 //so add the lowest weapon number which is w_first to get
	 //200 which is the glock, etc......
	 weapon += W_FIRST;
   }
 else if(type == A_RANDOM)
   {
	 //oz does not disable ammo so no need to check anything out here
	 weapon = RANDOM_LONG(A_FIRST,A_LAST);
	 random = TRUE; //is random
   }
 else
 {
	 weapon = type;//no randoms so spawn regular weapon
	 random = FALSE;
 }

   
	
	//new jammer dm player starts
	if(weapon == I_PLAYERSTART)
	{
		SpawnPlayerStart(x,y,z,angle,random);
		NumPlayerStarts++;
	}

	//jammer ctf stuff
	if(ctf.value > 0)
	{
		if(weapon == CTF_REDFLAG)
			SpawnRedFlag(x,y,z,angle,random);
		else if(weapon == CTF_BLUEFLAG)
  			SpawnBlueFlag(x,y,z,angle,random);
		else if(weapon == CTF_BLUESPAWN)
		{
			BlueSpawnPoint(x,y,z,angle,random);
			BluePlayerStarts++;
		}
		else if(weapon == CTF_REDSPAWN)
		{
			RedSpawnPoint(x,y,z,angle,random);
			RedPlayerStarts++;
		}
	}
}


//call this one time when server changes levels
//from serveractivate, if one of these values is changed in the
//middle of the level things could get weird. example: if the server
//admin were to type in disableglock 1, but who would do that
/*void CheckDisabledWeapons(void)
{
 
//just here for reference below
/*#define W_GLOCK		200
#define W_PYTHON	201
#define W_SHOTGUN	202
#define W_MP5		203
#define W_CROSSBOW	204
#define W_RPG		205
#define W_GAUSS		206
#define W_EGON		207
#define W_HORNET	208
#define W_HANDGRENADE	209
#define W_TRIPMINE	210
#define W_SATCHEL	211
#define W_SNARK		212
*/
/*
//these must in the same order as above for use with the spawnitem
int x = 0;

if(disableglock.value > 0)
{
   //this would work also
	//Weapons[W_GLOCK = W_FIRST].ISDisabled
   Weapons[0].IsDisabled = TRUE;
   x++;
}
if(disable357.value > 0)
{
   Weapons[1].IsDisabled = TRUE;
   x++;
}
if(disableshotgun.value > 0)
{
   Weapons[2].IsDisabled = TRUE;
   x++;
}
if(disablemp5.value > 0)
{
   Weapons[3].IsDisabled = TRUE;
   x++;
}
if(disablexbow.value > 0)
{
   Weapons[4].IsDisabled = TRUE;
   x++;
}
if(disablerpg.value > 0)
{
   Weapons[5].IsDisabled = TRUE;
   x++;
}
if(disablegauss.value > 0)
{
   Weapons[6].IsDisabled = TRUE;
   x++;
}
if(disableegon.value > 0)
{
   Weapons[7].IsDisabled = TRUE;
   x++;
}
if(disablehivehand.value > 0)
{
   Weapons[8].IsDisabled = TRUE;
   x++;
}
if(disablehgrenade.value > 0)
{
   Weapons[9].IsDisabled = TRUE;
   x++;
}
if(disabletripmine.value > 0)
{
   Weapons[10].IsDisabled = TRUE;
   x++;
}
if(disablesatchel.value > 0)
{
   Weapons[11].IsDisabled = TRUE;
   x++;
}
if(disablesnark.value > 0)
{
   Weapons[12].IsDisabled = TRUE;
   x++;
}


 if(x >= NUM_WEAPONS)
	 AllWeaponsDisabled = TRUE;

 else
	AllWeaponsDisabled = FALSE;
}
//jammer end*/



////////////////////////////////////////////////////////
// PAS and PVS routines for client messaging
//

/*
================
SetupVisibility

A client can have a separate "view entity" indicating that his/her view should depend on the origin of that
view entity.  If that's the case, then pViewEntity will be non-NULL and will be used.  Otherwise, the current
entity's origin is used.  Either is offset by the view_ofs to get the eye position.

From the eye position, we set up the PAS and PVS to use for filtering network messages to the client.  At this point, we could
 override the actual PAS or PVS values, or use a different origin.

NOTE:  Do not cache the values of pas and pvs, as they depend on reusable memory in the engine, they are only good for this one frame
================
*/
void SetupVisibility( edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas )
{
	Vector org;
	edict_t *pView = pClient;

	// Find the client's PVS
	if ( pViewEntity )
	{
		pView = pViewEntity;
	}
	//DME
	// Tracking Spectators use the visibility of their target
	CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );
	//mfrx 27FEB06
	//got a bug with quad drop and the bots
	//pPlayer is null when quad-holder dies
	//lets check for it
	/*if (!pPlayer)
	{
		ALERT(at_console, "SetupVisibility: Null Player\n");
	}
	
	else mfrx*/ if ( (pPlayer->pev->iuser2 != 0) && (pPlayer->m_hObserverTarget != NULL) )
	{
		pView = pPlayer->m_hObserverTarget->edict();
	}
	//DME END

	if ( pClient->v.flags & FL_PROXY )
	{
		*pvs = NULL;	// the spectator proxy sees
		*pas = NULL;	// and hears everything
		return;
	}
	org = pView->v.origin + pView->v.view_ofs;
	if ( pView->v.flags & FL_DUCKING )
	{
		org = org + ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );
	}

	*pvs = ENGINE_SET_PVS ( (float *)&org );
	*pas = ENGINE_SET_PAS ( (float *)&org );
}

#include "entity_state.h"

/*
AddToFullPack

Return 1 if the entity state has been filled in for the ent and the entity will be propagated to the client, 0 otherwise

state is the server maintained copy of the state info that is transmitted to the client
a MOD could alter values copied into state to send the "host" a different look for a particular entity update, etc.
e and ent are the entity that is being added to the update, if 1 is returned
host is the player's edict of the player whom we are sending the update to
player is 1 if the ent/e is a player and 0 otherwise
pSet is either the PAS or PVS that we previous set up.  We can use it to ask the engine to filter the entity against the PAS or PVS.
we could also use the pas/ pvs that we set in SetupVisibility, if we wanted to.  Caching the value is valid in that case, but still only for the current frame
*/
int AddToFullPack( struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet )
{
	int					i;

	// don't send if flagged for NODRAW and it's not the host getting the message
	if ( ( ent->v.effects == EF_NODRAW ) &&
		 ( ent != host ) )
		return 0;

	// Ignore ents without valid / visible models
	if ( !ent->v.modelindex || !STRING( ent->v.model ) )
		return 0;

	// Don't send spectators to other players
	if ( ( ent->v.flags & FL_SPECTATOR ) && ( ent != host ) )
	{
		return 0;
	}

	// Ignore if not the host and not touching a PVS/PAS leaf
	// If pSet is NULL, then the test will always succeed and the entity will be added to the update
	if ( ent != host )
	{
		//We still want to show all ents for a quick period ( max 700ms of lag ) for when we predict teleporters
		//if we don't do this, the entities on the other side wont show until the real origin comes thru and reaches
		//the new PVS/PAS.
		if ( !ENGINE_CHECK_VISIBILITY( (const struct edict_s *)ent, pSet ) && ent->v.fuser4 < gpGlobals->time )
		{
			return 0;
		}
	}


	// Don't send entity to local client if the client says it's predicting the entity itself.
	if ( ent->v.flags & FL_SKIPLOCALHOST )
	{
		if ( ( hostflags & 1 ) && ( ent->v.owner == host ) )
			return 0;
	}
	
	if ( host->v.groupinfo )
	{
		UTIL_SetGroupTrace( host->v.groupinfo, GROUP_OP_AND );

		// Should always be set, of course
		if ( ent->v.groupinfo )
		{
			if ( g_groupop == GROUP_OP_AND )
			{
				if ( !(ent->v.groupinfo & host->v.groupinfo ) )
					return 0;
			}
			else if ( g_groupop == GROUP_OP_NAND )
			{
				if ( ent->v.groupinfo & host->v.groupinfo )
					return 0;
			}
		}

		UTIL_UnsetGroupTrace();
	}

	memset( state, 0, sizeof( *state ) );

	// Assign index so we can track this entity from frame to frame and
	//  delta from it.
	state->number	  = e;
	state->entityType = ENTITY_NORMAL;
	
	// Flag custom entities.
	if ( ent->v.flags & FL_CUSTOMENTITY )
	{
		state->entityType = ENTITY_BEAM;
	}

	// 
	// Copy state data
	//

	// Round animtime to nearest millisecond
	state->animtime   = (int)(1000.0 * ent->v.animtime ) / 1000.0;

	memcpy( state->origin, ent->v.origin, 3 * sizeof( float ) );
	memcpy( state->angles, ent->v.angles, 3 * sizeof( float ) );
	memcpy( state->mins, ent->v.mins, 3 * sizeof( float ) );
	memcpy( state->maxs, ent->v.maxs, 3 * sizeof( float ) );

	memcpy( state->startpos, ent->v.startpos, 3 * sizeof( float ) );
	memcpy( state->endpos, ent->v.endpos, 3 * sizeof( float ) );

	state->impacttime = ent->v.impacttime;
	state->starttime = ent->v.starttime;

	state->modelindex = ent->v.modelindex;
		
	state->frame      = ent->v.frame;
	state->skin       = ent->v.skin;
	state->effects    = ent->v.effects;

	// This non-player entity is being moved by the game .dll and not the physics simulation system
	//  make sure that we interpolate it's position on the client if it moves
	if ( !player &&
		 ent->v.animtime &&
		 ent->v.velocity[ 0 ] == 0 && 
		 ent->v.velocity[ 1 ] == 0 && 
		 ent->v.velocity[ 2 ] == 0 )
	{
		state->eflags |= EFLAG_SLERP;
	}

	state->scale	  = ent->v.scale;
	state->solid	  = ent->v.solid;
	state->colormap   = ent->v.colormap;
	state->movetype   = ent->v.movetype;
	state->sequence   = ent->v.sequence;
	state->framerate  = ent->v.framerate;
	state->body       = ent->v.body;

	for (i = 0; i < 4; i++)
	{
		state->controller[i] = ent->v.controller[i];
	}

	for (i = 0; i < 2; i++)
	{
		state->blending[i]   = ent->v.blending[i];
	}

	state->rendermode    = ent->v.rendermode;
	state->renderamt     = ent->v.renderamt; 
	state->renderfx      = ent->v.renderfx;
	state->rendercolor.r = ent->v.rendercolor[0];
	state->rendercolor.g = ent->v.rendercolor[1];
	state->rendercolor.b = ent->v.rendercolor[2];

	state->aiment = 0;
	if ( ent->v.aiment )
	{
		state->aiment = ENTINDEX( ent->v.aiment );
	}

	state->owner = 0;
	if ( ent->v.owner )
	{
		int owner = ENTINDEX( ent->v.owner );
		
		// Only care if owned by a player
		if ( owner >= 1 && owner <= gpGlobals->maxClients )
		{
			state->owner = owner;	
		}
	}
	// Special stuff for players only
	if ( player )
	{
		memcpy( state->basevelocity, ent->v.basevelocity, 3 * sizeof( float ) );

		state->weaponmodel  = MODEL_INDEX( STRING( ent->v.weaponmodel ) );
		state->gaitsequence = ent->v.gaitsequence;
		state->spectator = ent->v.flags & FL_SPECTATOR;
		state->friction     = ent->v.friction;

		state->gravity      = ent->v.gravity;
//		state->team			= ent->v.team;
//		state->playerclass  = ent->v.playerclass;
		state->usehull      = ( ent->v.flags & FL_DUCKING ) ? 1 : 0;
		state->health		= ent->v.health;
	}

	return 1;
}

// defaults for clientinfo messages
#define	DEFAULT_VIEWHEIGHT	28

/*
===================
CreateBaseline

Creates baselines used for network encoding, especially for player data since players are not spawned until connect time.
===================
*/
void CreateBaseline( int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs )
{
	baseline->origin		= entity->v.origin;
	baseline->angles		= entity->v.angles;
	baseline->frame			= entity->v.frame;
	baseline->skin			= (short)entity->v.skin;

	// render information
	baseline->rendermode	= (byte)entity->v.rendermode;
	baseline->renderamt		= (byte)entity->v.renderamt;
	baseline->rendercolor.r	= (byte)entity->v.rendercolor[0];
	baseline->rendercolor.g	= (byte)entity->v.rendercolor[1];
	baseline->rendercolor.b	= (byte)entity->v.rendercolor[2];
	baseline->renderfx		= (byte)entity->v.renderfx;

	if ( player )
	{
		baseline->mins			= player_mins;
		baseline->maxs			= player_maxs;

		baseline->colormap		= eindex;
		baseline->modelindex	= playermodelindex;
		baseline->friction		= 1.0;
		baseline->movetype		= MOVETYPE_WALK;

		baseline->scale			= entity->v.scale;
		baseline->solid			= SOLID_SLIDEBOX;
		baseline->framerate		= 1.0;
		baseline->gravity		= 1.0;

	}
	else
	{
		baseline->mins			= entity->v.mins;
		baseline->maxs			= entity->v.maxs;

		baseline->colormap		= 0;
		baseline->modelindex	= entity->v.modelindex;//SV_ModelIndex(pr_strings + entity->v.model);
		baseline->movetype		= entity->v.movetype;

		baseline->scale			= entity->v.scale;
		baseline->solid			= entity->v.solid;
		baseline->framerate		= entity->v.framerate;
		baseline->gravity		= entity->v.gravity;
	}
}

typedef struct
{
	char name[32];
	int	 field;
} entity_field_alias_t;

#define FIELD_ORIGIN0			0
#define FIELD_ORIGIN1			1
#define FIELD_ORIGIN2			2
#define FIELD_ANGLES0			3
#define FIELD_ANGLES1			4
#define FIELD_ANGLES2			5

static entity_field_alias_t entity_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
};

void Entity_FieldInit( struct delta_s *pFields )
{
	entity_field_alias[ FIELD_ORIGIN0 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN0 ].name );
	entity_field_alias[ FIELD_ORIGIN1 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN1 ].name );
	entity_field_alias[ FIELD_ORIGIN2 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN2 ].name );
	entity_field_alias[ FIELD_ANGLES0 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES0 ].name );
	entity_field_alias[ FIELD_ANGLES1 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES1 ].name );
	entity_field_alias[ FIELD_ANGLES2 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES2 ].name );
}

/*
==================
Entity_Encode

Callback for sending entity_state_t info over network. 
FIXME:  Move to script
==================
*/
void Entity_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;

	int localplayer = 0;
	static int initialized = 0;

	if ( !initialized )
	{
		Entity_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	localplayer =  ( t->number - 1 ) == ENGINE_CURRENT_PLAYER();
	if ( localplayer )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}

	if ( ( t->impacttime != 0 ) && ( t->starttime != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );

		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES2 ].field );
	}

	if ( ( t->movetype == MOVETYPE_FOLLOW ) &&
		 ( t->aiment != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
	else if ( t->aiment != f->aiment )
	{
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
}

static entity_field_alias_t player_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
};

void Player_FieldInit( struct delta_s *pFields )
{
	player_field_alias[ FIELD_ORIGIN0 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN0 ].name );
	player_field_alias[ FIELD_ORIGIN1 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN1 ].name );
	player_field_alias[ FIELD_ORIGIN2 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN2 ].name );
}

/*
==================
Player_Encode

Callback for sending entity_state_t for players info over network. 
==================
*/
void Player_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;
	int localplayer = 0;
	static int initialized = 0;

	if ( !initialized )
	{
		Player_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	localplayer =  ( t->number - 1 ) == ENGINE_CURRENT_PLAYER();
	if ( localplayer )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}

	if ( ( t->movetype == MOVETYPE_FOLLOW ) &&
		 ( t->aiment != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
	else if ( t->aiment != f->aiment )
	{
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
}

#define CUSTOMFIELD_ORIGIN0			0
#define CUSTOMFIELD_ORIGIN1			1
#define CUSTOMFIELD_ORIGIN2			2
#define CUSTOMFIELD_ANGLES0			3
#define CUSTOMFIELD_ANGLES1			4
#define CUSTOMFIELD_ANGLES2			5
#define CUSTOMFIELD_SKIN			6
#define CUSTOMFIELD_SEQUENCE		7
#define CUSTOMFIELD_ANIMTIME		8

entity_field_alias_t custom_entity_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
	{ "skin",				0 },
	{ "sequence",			0 },
	{ "animtime",			0 },
};

void Custom_Entity_FieldInit( struct delta_s *pFields )
{
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_SKIN ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_SKIN ].name );
	custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].field= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].field= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].name );
}

/*
==================
Custom_Encode

Callback for sending entity_state_t info ( for custom entities ) over network. 
FIXME:  Move to script
==================
*/
void Custom_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;
	int beamType;
	static int initialized = 0;

	if ( !initialized )
	{
		Custom_Entity_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	beamType = t->rendermode & 0x0f;
		
	if ( beamType != BEAM_POINTS && beamType != BEAM_ENTPOINT )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].field );
	}

	if ( beamType != BEAM_POINTS )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].field );
	}

	if ( beamType != BEAM_ENTS && beamType != BEAM_ENTPOINT )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_SKIN ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].field );
	}

	// animtime is compared by rounding first
	// see if we really shouldn't actually send it
	if ( (int)f->animtime == (int)t->animtime )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].field );
	}
}

/*
=================
RegisterEncoders

Allows game .dll to override network encoding of certain types of entities and tweak values, etc.
=================
*/
void RegisterEncoders( void )
{
	DELTA_ADDENCODER( "Entity_Encode", Entity_Encode );
	DELTA_ADDENCODER( "Custom_Encode", Custom_Encode );
	DELTA_ADDENCODER( "Player_Encode", Player_Encode );
}

int GetWeaponData( struct edict_s *player, struct weapon_data_s *info )
{
	int i;
	weapon_data_t *item;
	entvars_t *pev = &player->v;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );
	CBasePlayerWeapon *gun;
	
	ItemInfo II;

	memset( info, 0, 32 * sizeof( weapon_data_t ) );

	if ( !pl )
		return 1;

	// go through all of the weapons and make a list of the ones to pack
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		if ( pl->m_rgpPlayerItems[ i ] )
		{
			// there's a weapon here. Should I pack it?
			CBasePlayerItem *pPlayerItem = pl->m_rgpPlayerItems[ i ];

			while ( pPlayerItem )
			{
				gun = (CBasePlayerWeapon *)pPlayerItem->GetWeaponPtr();
				if ( gun && gun->UseDecrement() )
				{
					// Get The ID.
					memset( &II, 0, sizeof( II ) );
					gun->GetItemInfo( &II );

					if ( II.iId >= 0 && II.iId < 32 )
					{
						item = &info[ II.iId ];
						
						item->m_iId						= II.iId;
						item->m_iClip					= gun->m_iClip;

						item->m_flTimeWeaponIdle		= max( gun->m_flTimeWeaponIdle, -0.001 );
						item->m_flNextPrimaryAttack		= max( gun->m_flNextPrimaryAttack, -0.001 );
						item->m_flNextSecondaryAttack	= max( gun->m_flNextSecondaryAttack, -0.001 );
						item->m_fInReload				= gun->m_fInReload;
					}
				}
				pPlayerItem = pPlayerItem->m_pNext;
			}
		}
	}
	return 1;
}

/*
=================
UpdateClientData

Data sent to current client only
engine sets cd to 0 before calling.
=================
*/
void UpdateClientData ( const struct edict_s *ent, int sendweapons, struct clientdata_s *cd )
{
	cd->flags			= ent->v.flags;
	cd->health			= ent->v.health;

	cd->viewmodel		= MODEL_INDEX( STRING( ent->v.viewmodel ) );
	cd->waterlevel		= ent->v.waterlevel;
	cd->watertype		= ent->v.watertype;
	//cd->weapons			= ent->v.weapons;

	// Vectors
	cd->origin			= ent->v.origin;
	cd->velocity		= ent->v.velocity;
	cd->view_ofs		= ent->v.view_ofs;
	cd->punchangle		= ent->v.punchangle;

	cd->bInDuck			= ent->v.bInDuck;
	cd->flTimeStepSound = ent->v.flTimeStepSound;
	cd->flDuckTime		= ent->v.flDuckTime;
	cd->flSwimTime		= ent->v.flSwimTime;
	cd->waterjumptime	= ent->v.teleport_time;

	strcpy( cd->physinfo, ENGINE_GETPHYSINFO( ent ) );

	cd->maxspeed		= ent->v.maxspeed;
	cd->fov				= ent->v.fov;
	cd->weaponanim		= ent->v.weaponanim;

	cd->pushmsec		= ent->v.pushmsec;
	//DME
	// Spectator
	cd->iuser1			= ent->v.iuser1;
	cd->iuser2			= ent->v.iuser2;
	//DME END

	// Observer
	//cd->iuser1			= ent->v.iuser1;
	//cd->iuser2			= ent->v.iuser2;
	cd->iuser3			= ent->v.iuser3;

	if ( sendweapons )
	{
		entvars_t *pev = (entvars_t *)&ent->v;
		CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );

		if ( pl )
		{
			cd->m_flNextAttack	= pl->m_flNextAttack;
			cd->weapons = pl->m_iQuakeItems;

			if ( pl->m_pActiveItem )
			{
				CBasePlayerWeapon *gun;
				gun = (CBasePlayerWeapon *)pl->m_pActiveItem->GetWeaponPtr();
				if ( gun && gun->UseDecrement() )
				{
					ItemInfo II;
					memset( &II, 0, sizeof( II ) );
					gun->GetItemInfo( &II );

					cd->m_iId = II.iId;
				}
			}

			cd->fuser1 = (float)pl->m_iQuakeWeapon;
			cd->iuser4 = gpGlobals->deathmatch;
			cd->fuser2 = pl->m_iNailOffset > 0 ? 1.0 : 0.0;

			cd->iuser3 = pl->m_iQuakeItems;

			cd->ammo_shells = pl->m_iAmmoShells;
			cd->ammo_nails = pl->m_iAmmoNails;
			cd->ammo_cells = pl->m_iAmmoCells;
			cd->ammo_rockets = pl->m_iAmmoRockets;
		}
	}
}

/*
=================
CmdStart

We're about to run this usercmd for the specified player.  We can set up groupinfo and masking here, etc.
This is the time to examine the usercmd for anything extra.  This call happens even if think does not.
=================
*/
void CmdStart( const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed )
{
	entvars_t *pev = (entvars_t *)&player->v;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );

	if( !pl )
		return;

	if ( cmd->weaponselect != 0 )
	{
		usercmd_t *c = (usercmd_t *)cmd;
		pl->W_ChangeWeapon( c->weaponselect );
		c->weaponselect = 0;
	}

	if ( pl->pev->groupinfo != 0 )
	{
		UTIL_SetGroupTrace( pl->pev->groupinfo, GROUP_OP_AND );
	}

	pl->random_seed = random_seed;
}

/*
=================
CmdEnd

Each cmdstart is exactly matched with a cmd end, clean up any group trace flags, etc. here
=================
*/
void CmdEnd ( const edict_t *player )
{
	entvars_t *pev = (entvars_t *)&player->v;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );

	if( !pl )
		return;
	if ( pl->pev->groupinfo != 0 )
	{
		UTIL_UnsetGroupTrace();
	}
}

/*
================================
ConnectionlessPacket

 Return 1 if the packet is valid.  Set response_buffer_size if you want to send a response packet.  Incoming, it holds the max
  size of the response_buffer, so you must zero it out if you choose not to respond.
================================
*/
int	ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	// Parse stuff from args
	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

/*
================================
GetHullBounds

  Engine calls this to enumerate player collision hulls, for prediction.  Return 0 if the hullnumber doesn't exist.
================================
*/
int GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	int iret = 0;

	switch ( hullnumber )
	{
	case 0:				// Normal player
		mins = Vector(-16, -16, -32);
		maxs = Vector(16, 16, 32);
		iret = 1;
		break;
	case 1:				// Crouched player
		mins = Vector(-16, -16, -32);
		maxs = Vector(16, 16, 32);
		iret = 1;
		break;
	case 2:				// Point based hull
		mins = Vector( 0, 0, 0 );
		maxs = Vector( 0, 0, 0 );
		iret = 1;
		break;
	}

	return iret;
}

/*
================================
CreateInstancedBaselines

Create pseudo-baselines for items that aren't placed in the map at spawn time, but which are likely
to be created during play ( e.g., grenades, ammo packs, projectiles, corpses, etc. )
================================
*/
void CreateInstancedBaselines ( void )
{
	int iret = 0;
	entity_state_t state;

	memset( &state, 0, sizeof( state ) );

	// Create any additional baselines here for things like grendates, etc.
	// iret = ENGINE_INSTANCE_BASELINE( pc->pev->classname, &state );

	// Destroy objects.
	//UTIL_Remove( pc );
}

/*
================================
InconsistentFile

One of the ENGINE_FORCE_UNMODIFIED files failed the consistency check for the specified player
 Return 0 to allow the client to continue, 1 to force immediate disconnection ( with an optional disconnect message of up to 256 characters )
================================
*/
int	InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message )
{
	// Server doesn't care?
	if ( CVAR_GET_FLOAT( "mp_consistency" ) != 1 )
		return 0;

	// Default behavior is to kick the player
	sprintf( disconnect_message, "Server is enforcing file consistency for %s\n", filename );

	// Kick now with specified disconnect message.
	return 1;
}

/*
================================
AllowLagCompensation

 The game .dll should return 1 if lag compensation should be allowed ( could also just set
  the sv_unlag cvar.
 Most games right now should return 0, until client-side weapon prediction code is written
  and tested for them ( note you can predict weapons, but not do lag compensation, too, 
  if you want.
================================
*/
int AllowLagCompensation( void )
{
	return 1;
}
float m_flIntermissionEndTime;
float m_flIntermissionStartTime;
BOOL m_iEndIntermissionButtonHit;
extern cvar_t mp_chattime;
#define INTERMISSION_TIME		10
void GoToIntermission( void )
{
	if ( g_fGameOver )
		return;  // intermission has already been triggered, so ignore.

	MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
	MESSAGE_END();

	// bounds check
	int time = (int)CVAR_GET_FLOAT( "mp_chattime" );
	if ( time < 1 )
		CVAR_SET_STRING( "mp_chattime", "1" );
	else if ( time > INTERMISSION_TIME )
		CVAR_SET_STRING( "mp_chattime", UTIL_dtos1( INTERMISSION_TIME ) );

	m_flIntermissionEndTime = gpGlobals->time + 10;
	m_flIntermissionStartTime = gpGlobals->time;
	
	g_fGameOver = TRUE;
	m_iEndIntermissionButtonHit = FALSE;
}


