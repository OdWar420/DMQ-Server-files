//
// ctf_gamerules.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"dme_ctf.h"
#include	"game.h"

static char team_names[MAX_CTF_TEAMS][MAX_CTF_TEAMNAME_LENGTH];
static int team_scores[MAX_CTF_TEAMS];
static int num_teams = 0;

extern DLL_GLOBAL BOOL		g_fGameOver;

CGameModeCTF :: CGameModeCTF()
{
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;

	memset( team_names, 0, sizeof(team_names) );
	memset( team_scores, 0, sizeof(team_scores) );
	num_teams = 0;

	// Copy over the team from the server config
	m_szTeamList[0] = 0;

	// Cache this because the team code doesn't want to deal with changing this in the middle of a game
	strncpy( m_szTeamList, ctfteamlist.string, CTF_TEAMLISTLENGTH );

	edict_t *pWorld = INDEXENT(0);
	if ( pWorld && pWorld->v.team )
	{
		if ( teamoverride.value )
		{
			const char *pTeamList = STRING(pWorld->v.team);
			if ( pTeamList && strlen(pTeamList) )
			{
				strncpy( m_szTeamList, pTeamList, CTF_TEAMLISTLENGTH );
			}
		}
	}
	// Has the server set teams
	m_teamLimit = TRUE;

	RecountTeams();
}

void CGameModeCTF :: Think ( void )
{
	///// Check game rules /////

	if ( g_fGameOver )   // someone else quit the game already
	{
		CHalfLifeMultiplay::Think();
		return;
	}

	float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;

	if ( flTimeLimit != 0 && gpGlobals->time >= flTimeLimit )
	{
		GoToIntermission();
		return;
	}

	float flFragLimit = fraglimit.value;
	if ( flFragLimit )
	{
		// check if any team is over the frag limit
		for ( int i = 0; i < num_teams; i++ )
		{
			if ( team_scores[i] >= flFragLimit )
			{
				GoToIntermission();
				return;
			}
		}
	}
}
void CGameModeCTF::JoinTeam(CBasePlayer *pPlayer, const char *pTeamName)
{
    char text [1024];

    //If already on this team, do nothing
    if (FStrEq(pPlayer->m_szTeamName, pTeamName))
    {
        return;
    }
    //Join the team
    if ( !stricmp( CTF_TEAM1_NAME, pTeamName ) )
	{
        sprintf( text, "%s joined the %s team\n", STRING(pPlayer->pev->netname), "Red" );
        UTIL_SayTextAll( text, pPlayer );
		pPlayer->ctf_team = 1;
		//Zapper - Altered to comply with Half-Life log format standard
        //UTIL_LogPrintf( "\"%s<%i>\" joined team \"%s\"\n",
			//STRING( pPlayer->pev->netname ), GETPLAYERUSERID( pPlayer->edict() ), "RED" );

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
		pPlayer->ctf_team = 2;
//Zapper - Altered to comply with Half-Life log format standard
        //UTIL_LogPrintf( "\"%s<%i>\" joined team \"%s\"\n",
			//STRING( pPlayer->pev->netname ), GETPLAYERUSERID( pPlayer->edict() ), "Blue" );

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
BOOL CGameModeCTF :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if ( FStrEq( pcmd, "menuselect" ) )
	{
		if ( CMD_ARGC() < 2 )
			return TRUE;

		int slot = atoi( CMD_ARGV(1) );

		// select the item from the current menu
        switch(pPlayer->m_nMenu)
        case CBasePlayer::Menu_ChangeTeam:
        {
            if (slot == 1)
            {
				JoinTeam(pPlayer, CTF_TEAM1_NAME);
				pPlayer->ctf_team = 1;
            }
            else if (slot == 2)
            {
                JoinTeam(pPlayer, CTF_TEAM2_NAME);
				pPlayer->ctf_team = 2;
            }
            else if (slot == 3)
            {
                JoinTeam(pPlayer, TeamWithFewestPlayers());
            }
        }

        return TRUE;
    }
    else if ( FStrEq( pcmd, "changeteam" ) )
    {
        //Make sure he's not dead
        if (!pPlayer->IsAlive()) return TRUE;
		DisplaySelectTeamMenu( pPlayer );
		/*char szMenu [512];
		pPlayer ->m_nMenu = CBasePlayer::Menu_ChangeTeam;
        //pPlayer->m_nMenu = Menu_ChangeTeam;
        strcpy (szMenu, "Select a team\n\n");
        strcat (szMenu, "1. Red Team\n");
        strcat (szMenu, "2. Blue Team\n\n");
        strcat (szMenu, "3. Autoteam");
        ShowMenu (pPlayer, 0x7, 0, 0, szMenu);*/


		return TRUE;
	}

	else if ( FStrEq( pcmd, "jointeam" ) )
    {
		int iszTeam = ALLOC_STRING( CMD_ARGV(1) );
		
		JoinTeam(pPlayer, ( STRING(iszTeam)));
        


		return TRUE;
	}

	return FALSE;

RecountTeams();
}

extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;


void CGameModeCTF :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 1 );  // game mode ctf
	MESSAGE_END();
}


const char *CGameModeCTF::SetDefaultPlayerTeam( CBasePlayer *pPlayer )
{
	// copy out the team name from the model
	char *mdls = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" );
	strncpy( pPlayer->m_szTeamName, mdls, TEAM_NAME_LENGTH );

	RecountTeams();

	// update the current player of the team he is joining
	if ( pPlayer->m_szTeamName[0] == '\0' || !IsValidTeam( pPlayer->m_szTeamName ) || defaultteam.value )
	{
		const char *pTeamName = NULL;

		if ( defaultteam.value )
		{
			pTeamName = team_names[0];
		}
		else
		{
			pTeamName = TeamWithFewestPlayers();
		}
		strncpy( pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH );
	}

	return pPlayer->m_szTeamName;
}




//=========================================================
// InitHUD
//=========================================================
void CGameModeCTF::InitHUD( CBasePlayer *pPlayer )
{
	//SetDefaultPlayerTeam( pPlayer );
	CHalfLifeMultiplay::InitHUD( pPlayer );

	RecountTeams();

	char *mdls = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" );

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
		

        /*char szMenu [512];
        pPlayer->m_nMenu = Menu_ChangeTeam;
        strcpy (szMenu, "Select a team\n\n");
        strcat (szMenu, "1. Red Team\n");
        strcat (szMenu, "2. Blue Team\n\n");
        strcat (szMenu, "3. Autoteam");
        ShowMenu (pPlayer, 0x7, 0, 0, szMenu);*/
        //ShowMenu (pPlayer, 0x7, 0, 0, "#Team_Menu_Join"); //Changed for the stupid bot - trend - hmm turns out its not needed , lol
		DisplaySelectTeamMenu( pPlayer );
		
		



}


void CGameModeCTF::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
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


	// g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szTeamName );
    if ( !stricmp( pTeamName, CTF_TEAM1_NAME ) ) {
        g_engfuncs.pfnSetClientKeyValue( clientIndex,
            g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ),
            "model", "fire" );
        pPlayer->ctf_team = 1;
        strcpy( pPlayer->m_szTeamName, CTF_TEAM1_NAME );
    } else if ( !stricmp( pTeamName, CTF_TEAM2_NAME ) ) {
	    g_engfuncs.pfnSetClientKeyValue( clientIndex,
            g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ),
            "model", "ice" );
        pPlayer->ctf_team = 2;
        strcpy( pPlayer->m_szTeamName, CTF_TEAM2_NAME );
    } else {
        char buf[256];
        sprintf( buf, "Tell DataSpel you have a bogus team name [%s]\n", pTeamName );
        ClientPrint( pPlayer->pev, HUD_PRINTCONSOLE, buf );
        pPlayer->ctf_team = 0;
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
void CGameModeCTF::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{

	pPlayer->m_iFOV = atoi( g_engfuncs.pfnInfoKeyValue( infobuffer, "cl_fov" ) );
	pPlayer->m_iAutoWepSwitch = atoi( g_engfuncs.pfnInfoKeyValue( infobuffer, "cl_autowepswitch" ) );


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


extern int gmsgDeathMsg;

//=========================================================
// Deathnotice.
//=========================================================
void CGameModeCTF::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	if ( m_DisableDeathMessages )
		return;

	if ( pVictim && pKiller && pKiller->flags & FL_CLIENT )
	{
		CBasePlayer *pk = (CBasePlayer*) CBaseEntity::Instance( pKiller );

		if ( pk )
		{
			if ( (pk != pVictim) && (PlayerRelationship( pVictim, pk ) == GR_TEAMMATE) )
			{
				MESSAGE_BEGIN( MSG_ALL, gmsgDeathMsg );
					WRITE_BYTE( ENTINDEX(ENT(pKiller)) );		// the killer
					WRITE_BYTE( ENTINDEX(pVictim->edict()) );	// the victim
					WRITE_STRING( "teammate" );		// flag this as a teammate kill
				MESSAGE_END();
				return;
			}
		}
	}

	CHalfLifeMultiplay::DeathNotice( pVictim, pKiller, pevInflictor );
}

//=========================================================
//=========================================================
void CGameModeCTF :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	if ( !m_DisableDeathPenalty )
	{
		CHalfLifeMultiplay::PlayerKilled( pVictim, pKiller, pInflictor );
		RecountTeams();
	}
}


//=========================================================
// IsCTF
//=========================================================
BOOL CGameModeCTF::IsCTF( void )
{
	return TRUE;
}

BOOL CGameModeCTF::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
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
int CGameModeCTF::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
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
BOOL CGameModeCTF::ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target )
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
int CGameModeCTF::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
		return -1;

	// Oz runes - frags
	if (PlayerRelationship( pAttacker, pKilled ) != GR_TEAMMATE)
		return 1;

	return 1;
}

//=========================================================
//=========================================================
const char *CGameModeCTF::GetTeamID( CBaseEntity *pEntity )
{
	if ( pEntity == NULL || pEntity->pev == NULL )
		return "";

	// return their team name
	return pEntity->TeamID();
}


int CGameModeCTF::GetTeamIndex( const char *pTeamName )
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

const char *CGameModeCTF::GetIndexedTeamName( int teamIndex )
{
	if ( teamIndex < 0 || teamIndex >= num_teams )
		return "";

	return team_names[ teamIndex ];
}


BOOL CGameModeCTF::IsValidTeam( const char *pTeamName )
{
	if ( !m_teamLimit )	// Any team is valid if the teamlist isn't set
		return TRUE;

	return ( GetTeamIndex( pTeamName ) != -1 ) ? TRUE : FALSE;
}

int CGameModeCTF :: PlayersOnTeam( const char *szTeamName )
{
	// returns # of players on a certain team

	int i;
	int iPlayers = 0;

	// loop through all players and increment for that team

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;

			if ( FStrEq( pPlayer->m_szTeamName, szTeamName ) )
				iPlayers++;

		}
	}

	return iPlayers;
}

const char *CGameModeCTF::TeamWithFewestPlayers( void )
{
	// returns team with fewest players

	if( PlayersOnTeam( CTF_TEAM1_NAME ) < PlayersOnTeam( CTF_TEAM2_NAME ) )
		return CTF_TEAM1_NAME;
	else if( PlayersOnTeam( CTF_TEAM1_NAME ) > PlayersOnTeam( CTF_TEAM2_NAME ) )
		return CTF_TEAM2_NAME;
	else	// returns random team if equal
	{
		if( RANDOM_LONG( 1, 2 ) == 1 )
			return CTF_TEAM1_NAME;
		else
			return CTF_TEAM2_NAME;
	}
}


//=========================================================
//=========================================================
void CGameModeCTF::RecountTeams( void )
{
	char	*pName;
	char	teamlist[CTF_TEAMLISTLENGTH];

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
				plr->ctf_team = 1;// red team
			else if ( tm == 1)
				plr->ctf_team = 2;// blue team
			else
				plr->ctf_team = 0;
			// Oz error check
			//ClientPrint( plr->pev, HUD_PRINTCENTER, UTIL_VarArgs ("-%i-\n", INT( plr->ctf_team ) ) );

			if ( tm >= 0 )
			{
				team_scores[tm] += plr->pev->frags;
			}
		}
	}
}

class CRedSpawn : public CPointEntity
{
	void Spawn( void )
	{
		pev->classname = MAKE_STRING( "ctf_redspawn" );
/*		int iNumSpawnSpots = rgTeamList[0].iNumSpawnSpots;
		if ( iNumSpawnSpots < MAX_SPAWNSPOTS )
		{
			rgTeamList[0].pSpawnSpots[iNumSpawnSpots] = ENT( pev );
			rgTeamList[0].iNumSpawnSpots++;
		}
*/		CPointEntity::Spawn();
	}
	//jammer ctf
	public:
	void JamRedSpawn(int,int,int,int,BOOL);

};

LINK_ENTITY_TO_CLASS(ctf_redspawn, CRedSpawn);

class CBlueSpawn : public CPointEntity
{
	void Spawn( void )
	{
		pev->classname = MAKE_STRING( "ctf_bluespawn" );
/*		int iNumSpawnSpots = rgTeamList[1].iNumSpawnSpots;
		if ( iNumSpawnSpots < MAX_SPAWNSPOTS )
		{
			rgTeamList[1].pSpawnSpots[iNumSpawnSpots] = ENT( pev );
			rgTeamList[1].iNumSpawnSpots++;
		}
*/		CPointEntity::Spawn();
	}
//jammer ctf
public:
	void JamBlueSpawn(int,int,int,int,BOOL);
};

LINK_ENTITY_TO_CLASS(ctf_bluespawn, CBlueSpawn);



//jammer ctf
void CBlueSpawn::JamBlueSpawn(int x, int y, int z, int a, BOOL random)
{
  Vector pos(x,y,z);
  Vector angle(0,a,0);
  CBaseEntity *pBlueSpawn = CBaseEntity::Create("ctf_bluespawn",pos,angle,ENT(pev));
}
void CRedSpawn::JamRedSpawn(int x, int y, int z, int a, BOOL random)
{
  Vector pos(x,y,z);
  Vector angle(0,a,0);
  CBaseEntity *pRedSpawn = CBaseEntity::Create("ctf_redspawn",pos,angle,ENT(pev));

}

void RedSpawnPoint(int x, int y, int z, int a, BOOL random)
{
  CRedSpawn *pRedSpawn = GetClassPtr((CRedSpawn *)NULL);
  pRedSpawn->JamRedSpawn(x,y,z,a,random);

}

void BlueSpawnPoint(int x, int y, int z, int a, BOOL random)
{
  CBlueSpawn *pBlueSpawn = GetClassPtr((CBlueSpawn *)NULL);
  pBlueSpawn->JamBlueSpawn(x,y,z,a,random);

}

//=========================================================
// Menustuff
//=========================================================

extern int gmsgShowMenu; // the message # for ShowMenu

void CGameModeCTF::ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots,
    int nDisplayTime, BOOL fNeedMore, char *pszText)
{
    MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pPlayer ->pev);
        WRITE_SHORT( bitsValidSlots);
        WRITE_CHAR( nDisplayTime );
        WRITE_BYTE( fNeedMore );
		//WRITE_STRING ("#Team_Menu_Join");
        WRITE_STRING (pszText);
    MESSAGE_END();
}

//WRITE_STRING (fNoCancel ? "#Team_Menu_Join" : "#Team_Menu_Join");

// SCTF
/*
Not implemented at this time
As you probably know, Half-Life searches each map for a certain entity
(set in the liblist.gam file) to see if it's a valid map for the current
modification of the game. For HMCTF I created the "info_hmctfdetect"
entity by adding this code to the end of "hmctf_gamerules.cpp":
It just has to be spawned and placed in the world. It isn't solid and
isn't visible. That's all for this time!
*/
//=========================================================
// Map detection
//=========================================================

class CInfoHMCTFDetect : public CBaseEntity
{
    void Spawn( void )
    {
        UTIL_SetOrigin( pev, pev->origin );
        pev->solid = SOLID_NOT;
        pev->effects = EF_NODRAW;
    }
};

LINK_ENTITY_TO_CLASS( info_hmctfdetect, CInfoHMCTFDetect );
