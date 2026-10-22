//
// teamplay_gamerules.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"game.h"
//
//Added by Zapper
//
extern int g_teamplay;
//
//End of Mod

static char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
static int team_scores[MAX_TEAMS];
static int num_teams = 0;
extern int gmsgTeamNames;
extern DLL_GLOBAL BOOL		g_fGameOver;

CHalfLifeTeamplay :: CHalfLifeTeamplay()
{

	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;

	memset( team_names, 0, sizeof(team_names) );
	memset( team_scores, 0, sizeof(team_scores) );
	num_teams = 0;

	// Copy over the team from the server config
	m_szTeamList[0] = 0;

	// Cache this because the team code doesn't want to deal with changing this in the middle of a game
	strncpy( m_szTeamList, teamlist.string, TEAMPLAY_TEAMLISTLENGTH );

	edict_t *pWorld = INDEXENT(0);
	if ( pWorld && pWorld->v.team )
	{
		if ( teamoverride.value )
		{
			const char *pTeamList = STRING(pWorld->v.team);
			if ( pTeamList && strlen(pTeamList) )
			{
				strncpy( m_szTeamList, pTeamList, TEAMPLAY_TEAMLISTLENGTH );
			}
		}
	}
	// Has the server set teams
	if ( strlen( m_szTeamList ) )
		m_teamLimit = TRUE;
	else
		m_teamLimit = FALSE;

	RecountTeams();
}

extern cvar_t timeleft, fragsleft;
void CHalfLifeTeamplay :: Think ( void )
{
	
	///// Check game rules /////
	static int last_frags;
	static int last_time;

	int frags_remaining = 0;
	int time_remaining = 0;

	if ( g_fGameOver )   // someone else quit the game already
	{
		CHalfLifeMultiplay::Think();
		return;
	}

	float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;
	
	time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);

	if ( flTimeLimit != 0 && gpGlobals->time >= flTimeLimit )
	{
		GoToIntermission();
		return;
	}

	float flFragLimit = fraglimit.value;
	if ( flFragLimit )
	{
		int bestfrags = 9999;
		int remain;

		// check if any team is over the frag limit
		for ( int i = 0; i < num_teams; i++ )
		{
			if ( team_scores[i] >= flFragLimit )
			{
				GoToIntermission();
				return;
			}

			remain = flFragLimit - team_scores[i];
			if ( remain < bestfrags )
			{
				bestfrags = remain;
			}
		}
		frags_remaining = bestfrags;
	}

	// Updates when frags change
	if ( frags_remaining != last_frags )
	{
		g_engfuncs.pfnCvar_DirectSet( &fragsleft, UTIL_VarArgs( "%i", frags_remaining ) );
	}

	// Updates once per second
	if ( timeleft.value != last_time )
	{
		g_engfuncs.pfnCvar_DirectSet( &timeleft, UTIL_VarArgs( "%i", time_remaining ) );
	}

	last_frags = frags_remaining;
	last_time  = time_remaining;
}

// DME

void CHalfLifeTeamplay::JoinTeam(CBasePlayer *pPlayer, const char *pTeamName)
{
	char text [1024]; 

	int damageFlags = DMG_GENERIC;

	damageFlags |= DMG_NEVERGIB;

	//If already on this team, do nothing
	if (FStrEq(pPlayer->m_szTeamName, pTeamName))
	{
		return;
	}	
	//Join the team 
    if ( !stricmp( TEAM1_NAME, pTeamName ) )
	{
        sprintf( text, "%s joined the %s team\n", STRING(pPlayer->pev->netname), "Red" );
        UTIL_SayTextAll( text, pPlayer );
        //Zapper - Altered to comply with Half-Life log format standard
		//UTIL_LogPrintf( "\"%s<%i>\" joined the %s team\n", 
        //    STRING( pPlayer->pev->netname ), GETPLAYERUSERID( pPlayer->edict() ), "Red" );
		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" joined team \"%s\"\n",
		STRING(pPlayer->pev->netname),
		GETPLAYERUSERID( pPlayer->edict() ),
		GETPLAYERAUTHID( pPlayer->edict() ),
		g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" ), "obi03" );
    } 
	else 
	{
        sprintf( text, "%s joined the %s team\n", STRING(pPlayer->pev->netname), "Blue" );
        UTIL_SayTextAll( text, pPlayer );
        //Zapper - Altered to comply with Half-Life log format standard
		//UTIL_LogPrintf( "\"%s<%i>\" joined the %s team\n", 
        //    STRING( pPlayer->pev->netname ), GETPLAYERUSERID( pPlayer->edict() ), "Blue" );
		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" joined team \"%s\"\n",
		STRING(pPlayer->pev->netname),
		GETPLAYERUSERID( pPlayer->edict() ),
		GETPLAYERAUTHID( pPlayer->edict() ),
		g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" ), "obi03" );
    }

	//When observer, make it respawnable, let it join a team and then respawn
	if ( pPlayer->m_afPhysicsFlags & PFLAG_OBSERVER)
	{
		pPlayer->pev->deadflag = DEAD_RESPAWNABLE;
		ChangePlayerTeam( pPlayer, pTeamName, FALSE, FALSE );
		// recound stuff
		RecountTeams();
		pPlayer->Spawn();
	}
	//Else just switch teams
	else
	{
		ChangePlayerTeam( pPlayer, pTeamName, TRUE, TRUE );
		// recound stuff
		RecountTeams();
	}
}

//=========================================================
// ClientCommand
// the user has typed a command which is unrecognized by everything else;
// this check to see if the gamerules knows anything about the command
//=========================================================
BOOL CHalfLifeTeamplay :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{ 

	int damageFlags = DMG_GENERIC;
	int clientIndex = pPlayer->entindex();


	damageFlags |= DMG_NEVERGIB;

	entvars_t *pevWorld = VARS( INDEXENT(0) );

	if ( FStrEq( pcmd, "menuselect" ) )
	{
		if ( CMD_ARGC() < 2 )
			return TRUE;

		int slot = atoi( CMD_ARGV(1) );

		// select the item from the current menu
        switch(pPlayer->m_nMenu)
        case CBasePlayer::Menu_ChangeTeam:
        {
			if ( slot == 1 )
			{
				JoinTeam(pPlayer, TEAM1_NAME);
				pPlayer->team = 1;
            }
            else if (slot == 2)
            {
                JoinTeam(pPlayer, TEAM2_NAME);
				pPlayer->team = 2;	
            }
            else if (slot == 3)
            {

				JoinTeam( pPlayer, TeamWithFewestPlayers( ));

            }
        } 

		return TRUE;
	}
    else if ( FStrEq( pcmd, "changeteam" ) )
    {
        //Make sure he's not dead
        if (!pPlayer->IsAlive()) return TRUE; 
		char szMenu [512];
        pPlayer->m_nMenu = CBasePlayer::Menu_ChangeTeam;
        strcpy (szMenu, "Select a team\n\n");
        strcat (szMenu, "1. Red Team\n");
        strcat (szMenu, "2. Blue Team\n\n");
        strcat (szMenu, "3. Autoteam");
        ShowMenu (pPlayer, 0x7, 0, 0, szMenu);


		return TRUE;
	}

	return FALSE;

	RecountTeams();
}

extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;


void CHalfLifeTeamplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 1 );  // game mode teamplay
	MESSAGE_END();
}

//=========================================================
// InitHUD
//=========================================================
void CHalfLifeTeamplay::InitHUD( CBasePlayer *pPlayer )
{
	CHalfLifeMultiplay::InitHUD( pPlayer );

	RecountTeams();

	char *mdls = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" );

	MESSAGE_BEGIN(MSG_ONE, gmsgTeamNames, NULL, pPlayer->edict());
			WRITE_BYTE(2); // Two teams. (You can have up to 4.)
			WRITE_STRING(TEAM1_NAME); // You need to pass the teams as literal strings
			WRITE_STRING(TEAM2_NAME);
			MESSAGE_END();
	// update this player with all the other players team info
	// loop through all active players and send their team info to the new client
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );
		if ( plr && IsValidTeam( plr->TeamID() ) )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgTeamInfo, NULL, pPlayer->edict() );
				WRITE_BYTE( plr->entindex() );
				WRITE_STRING( plr->TeamID() );
			MESSAGE_END();
		}
	}

    
        char szMenu [512];
        pPlayer->m_nMenu = CBasePlayer::Menu_ChangeTeam;
        strcpy (szMenu, "Select a team\n\n");
        strcat (szMenu, "1. Red Team\n");
        strcat (szMenu, "2. Blue Team\n\n");
        strcat (szMenu, "3. Autoteam");
        ShowMenu (pPlayer, 0x7, 0, 0, szMenu);
    

		
}


void CHalfLifeTeamplay::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
{
	int damageFlags = DMG_GENERIC;
	int clientIndex = pPlayer->entindex();

	if ( !bGib )
	{
		damageFlags |= DMG_NEVERGIB;
	}
	else
	{
		damageFlags |= DMG_ALWAYSGIB;
	}

	if ( bKill )
	{
		// kill the player,  remove a death,  and let them start on the new team
		m_DisableDeathMessages = TRUE;
		m_DisableDeathPenalty = TRUE;

		entvars_t *pevWorld = VARS( INDEXENT(0) );
		pPlayer->TakeDamage( pevWorld, pevWorld, 900, damageFlags );

		m_DisableDeathMessages = FALSE;
		m_DisableDeathPenalty = FALSE;
	}

	// copy out the team name from the model
	strncpy( pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH );


	//g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szTeamName );
    if ( !stricmp( pTeamName, TEAM1_NAME ) ) {
        g_engfuncs.pfnSetClientKeyValue( clientIndex, 
            g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
            "model", "obi03" );
        pPlayer->team = 1;
        strcpy( pPlayer->m_szTeamName, TEAM1_NAME );
    } else if ( !stricmp( pTeamName, TEAM2_NAME ) ) {
	    g_engfuncs.pfnSetClientKeyValue( clientIndex, 
            g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
            "model", "obi03" );
        pPlayer->team = 2;
        strcpy( pPlayer->m_szTeamName, TEAM2_NAME );
    } else {
        char buf[256];
        sprintf( buf, "Tell DataSpel you have a bogus team name [%s]\n", pTeamName );
        ClientPrint( pPlayer->pev, HUD_PRINTCONSOLE, buf );
        pPlayer->team = 0;
    }


	g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );

	// notify everyone's HUD of the team change
	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( clientIndex );
		WRITE_STRING( pPlayer->m_szTeamName );
	MESSAGE_END();
}


//=========================================================
// ClientUserInfoChanged
//=========================================================
void CHalfLifeTeamplay::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	// DME - This is the "dreaded FOV" fix for teamplay in the DMC code - TrenD

	pPlayer->m_iFOV = atoi( g_engfuncs.pfnInfoKeyValue( infobuffer, "cl_fov" ) );
	pPlayer->m_iAutoWepSwitch = atoi( g_engfuncs.pfnInfoKeyValue( infobuffer, "cl_autowepswitch" ) );//Just something extra
	
	
     //an observer can change a lot, but isn't visible so hasn't got to do much here
    if ( pPlayer->IsObserver())
        return; 

	// prevent skin/color/model changes
	char *mdls = g_engfuncs.pfnInfoKeyValue( infobuffer, "model" );

	if ( !stricmp( mdls, pPlayer->m_szTeamName ) )
		return;

	if ( defaultteam.value )
	{
		int clientIndex = pPlayer->entindex();

		if ( !stricmp( pPlayer->m_szTeamName, "Red" ) ) 
		{
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "obi03" );
		} 
		else 
		{
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "obi03" );
		}   

	} 

	RecountTeams();
}
// DME
extern int gmsgDeathMsg;

//=========================================================
// Deathnotice. 
//=========================================================
void CHalfLifeTeamplay::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
//
//Modified By Zapper
//Modded code to write teams kills into the server logs for HLStats and Logd
//

	// Work out what killed the player, and send a message to all clients about it
	CBaseEntity *Killer = CBaseEntity::Instance( pKiller );

	const char *killer_weapon_name = "world";		// by default, the player is killed by the world
	int killer_index = 0;
	
	// Hack to fix name change
	char *tau = "tau_cannon";
	char *gluon = "gluon gun";

	// QUAKECLASSIC
	// Might have overridden
	if (g_szDeathType)
	{
		killer_weapon_name = g_szDeathType;
	}
	else
	{
		if ( pKiller->flags & FL_CLIENT )
		{
			killer_index = ENTINDEX(ENT(pKiller));
			
			if ( pevInflictor )
			{
				if ( pevInflictor == pKiller )
				{
					// If the inflictor is the killer,  then it must be their current weapon doing the damage
					CBasePlayer *pPlayer = (CBasePlayer*)CBaseEntity::Instance( pKiller );
					
					switch( pPlayer->m_iQuakeWeapon )
					{
						case IT_AXE: killer_weapon_name = "weapon_axe"; break;
						case IT_SHOTGUN: killer_weapon_name = "weapon_shotgun"; break;
						case IT_SUPER_SHOTGUN: killer_weapon_name = "weapon_doubleshotgun"; break;
						case IT_NAILGUN: killer_weapon_name = "weapon_nailgun"; break;
						case IT_SUPER_NAILGUN: killer_weapon_name = "weapon_supernail"; break;
						case IT_GRENADE_LAUNCHER: killer_weapon_name = "weapon_grenadel"; break;
						case IT_ROCKET_LAUNCHER: killer_weapon_name = "weapon_rocketl"; break;
						case IT_LIGHTNING: killer_weapon_name = "weapon_lightning"; break;
							
						default:
							killer_weapon_name = "Empty";
					} 
				
				}
				else
				{
					killer_weapon_name = STRING( pevInflictor->classname );  // it's just that easy
				}
			}
		}
		else
		{
			killer_weapon_name = STRING( pevInflictor->classname );
		}

		// strip the monster_* or weapon_* from the inflictor's classname
		if ( strncmp( killer_weapon_name, "weapon_", 7 ) == 0 )
			killer_weapon_name += 7;
		else if ( strncmp( killer_weapon_name, "monster_", 8 ) == 0 )
			killer_weapon_name += 8;
		else if ( strncmp( killer_weapon_name, "func_", 5 ) == 0 )
			killer_weapon_name += 5;
	}


	// replace the code names with the 'real' names
	if ( !strcmp( killer_weapon_name, "egon" ) )
		killer_weapon_name = gluon;
	else if ( !strcmp( killer_weapon_name, "gauss" ) )
		killer_weapon_name = tau;


	if ( m_DisableDeathMessages )
		return;
	
	if ( pVictim && pKiller && pKiller->flags & FL_CLIENT )
	{
		CBasePlayer *pk = (CBasePlayer*) CBaseEntity::Instance( pKiller );

		if ( pk )
		{
			if ( (pk != pVictim) && (PlayerRelationship( pVictim, pk ) == GR_TEAMMATE) )
			{
				
				UTIL_LogPrintf( "\"%s<%i><%s><%s>\" killed \"%s<%i><%s><%s>\" with \"%s\"\n",  
				STRING( pKiller->netname ),
				GETPLAYERUSERID( ENT(pKiller) ),
				GETPLAYERAUTHID( ENT(pKiller) ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( ENT(pKiller) ), "model" ),
				STRING( pVictim->pev->netname ),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );
		
				MESSAGE_BEGIN( MSG_ALL, gmsgDeathMsg );
					WRITE_BYTE( ENTINDEX(ENT(pKiller)) );		// the killer
					WRITE_BYTE( ENTINDEX(pVictim->edict()) );	// the victim
					WRITE_STRING( "teammate" );		// flag this as a teammate kill
				MESSAGE_END();
				return;
			
			}
		}
	}

//
//End of Mod
//
	CHalfLifeMultiplay::DeathNotice( pVictim, pKiller, pevInflictor );
}

//=========================================================
//=========================================================
void CHalfLifeTeamplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	if ( !m_DisableDeathPenalty )
	{
		CHalfLifeMultiplay::PlayerKilled( pVictim, pKiller, pInflictor );
		RecountTeams();
	}
}


//=========================================================
// IsTeamplay
//=========================================================
BOOL CHalfLifeTeamplay::IsTeamplay( void )
{
	return TRUE;
}

BOOL CHalfLifeTeamplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	if ( pAttacker && PlayerRelationship( pPlayer, pAttacker ) == GR_TEAMMATE )
	{
		// my teammate hit me.
		if ( (CVAR_GET_FLOAT("mp_friendlyfire") == 0) && (pAttacker != pPlayer) )
		{
			// friendly fire is off, and this hit came from someone other than myself,  then don't get hurt
			return FALSE;
		}
	}

	return CHalfLifeMultiplay::FPlayerCanTakeDamage( pPlayer, pAttacker );
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() )
		return GR_NOTTEAMMATE;

	if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
	{
		return GR_TEAMMATE;
	}

	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeTeamplay::ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target )
{
	// always autoaim, unless target is a teammate
	CBaseEntity *pTgt = CBaseEntity::Instance( target );
	if ( pTgt && pTgt->IsPlayer() )
	{
		if ( PlayerRelationship( pPlayer, pTgt ) == GR_TEAMMATE )
			return FALSE; // don't autoaim at teammates
	}

	return CHalfLifeMultiplay::ShouldAutoAim( pPlayer, target );
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
		return -1;

	
	return 1;
}

//=========================================================
//=========================================================
const char *CHalfLifeTeamplay::GetTeamID( CBaseEntity *pEntity )
{
	if ( pEntity == NULL || pEntity->pev == NULL )
		return "";

	// return their team name
	return pEntity->TeamID();
}


int CHalfLifeTeamplay::GetTeamIndex( const char *pTeamName )
{
	if ( pTeamName && *pTeamName != 0 )
	{
		// try to find existing team
		for ( int tm = 0; tm < num_teams; tm++ )
		{
			if ( !stricmp( team_names[tm], pTeamName ) )
				return tm;
		}
	}
	
	return -1;	// No match
}


const char *CHalfLifeTeamplay::GetIndexedTeamName( int teamIndex )
{
	if ( teamIndex < 0 || teamIndex >= num_teams )
		return "";

	return team_names[ teamIndex ];
}


BOOL CHalfLifeTeamplay::IsValidTeam( const char *pTeamName ) 
{
	if ( !m_teamLimit )	// Any team is valid if the teamlist isn't set
		return TRUE;

	return ( GetTeamIndex( pTeamName ) != -1 ) ? TRUE : FALSE;
}

const char *CHalfLifeTeamplay::TeamWithFewestPlayers( void )
{
	int i;
	int minPlayers = MAX_TEAMS;
	int teamCount[ MAX_TEAMS ];
	char *pTeamName = NULL;

	//check if the hmctf teams exist. If a team doesn't exist it is the team with the fewest players
	if (GetTeamIndex(TEAM1_NAME) == -1)
		return TEAM1_NAME;
	else if (GetTeamIndex(TEAM2_NAME) == -1)
		return TEAM2_NAME;

	memset( teamCount, 0, MAX_TEAMS * sizeof(int) );
	
	// loop through all clients, count number of players on each team
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr )
		{
			int team = GetTeamIndex( plr->TeamID() );
			if ( team >= 0 )
				teamCount[team] ++;
		}
	}

	// Find team with least players
	for ( i = 0; i < num_teams; i++ )
	{
		if ( teamCount[i] < minPlayers )
		{
			minPlayers = teamCount[i];
			pTeamName = team_names[i];
		}
	}

	return pTeamName;
}


//=========================================================
//=========================================================
void CHalfLifeTeamplay::RecountTeams( void )
{
	char	*pName;
	char	teamlist[TEAMPLAY_TEAMLISTLENGTH];

	// loop through all teams, recounting everything
	num_teams = 0;

	// Copy all of the teams from the teamlist
	// make a copy because strtok is destructive
	strcpy( teamlist, m_szTeamList );
	pName = teamlist;
	pName = strtok( pName, ";" );
	while ( pName != NULL && *pName )
	{
		if ( GetTeamIndex( pName ) < 0 )
		{
			strcpy( team_names[num_teams], pName );
			num_teams++;
		}
		pName = strtok( NULL, ";" );
	}

	if ( num_teams < 2 )
	{
		num_teams = 0;
		m_teamLimit = FALSE;
	}

	// Sanity check
	memset( team_scores, 0, sizeof(team_scores) );

	// loop through all clients
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr )
		{
			const char *pTeamName = plr->TeamID();
			// try add to existing team
			int tm = GetTeamIndex( pTeamName );
			
			// need a reference variable to compare players to their flags
			if ( tm == 0 )
				plr->team = 1;// red team
			else if ( tm == 1)
				plr->team = 2;// blue team
			else
				plr->team = 0;

			/*if ( tm < 0 ) // no team match found
			{ 
				if ( !m_teamLimit )
				{
					// add to new team
					tm = num_teams;
					num_teams++;
					team_scores[tm] = 0;
					strncpy( team_names[tm], pTeamName, MAX_TEAMNAME_LENGTH );
				}
			}*/

			if ( tm >= 0 )
			{
				team_scores[tm] += plr->pev->frags;
			}
		}
	}
}
//=========================================================
// Menustuff
//========================================================= 

extern int gmsgShowMenu; // the message # for ShowMenu 

void CHalfLifeTeamplay::ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots, 
    int nDisplayTime, BOOL fNeedMore, char *pszText)
{
    MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pPlayer ->pev);
        WRITE_SHORT( bitsValidSlots);
        WRITE_CHAR( nDisplayTime );
        WRITE_BYTE( fNeedMore );
        WRITE_STRING (pszText);
    MESSAGE_END();
} 

