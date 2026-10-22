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
#include    "dme_utility.h"
#include    "dme_clanarena.h"
#include    "quake_gun.h"



//Added by Zapper
//
extern int g_teamplay;
//
//End of Mod
//
/*class CDMCGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool		ComparePlayerTeams(CBasePlayer *pPlayer1, CBasePlayer *pPlayer2)
	{
		return true;
	}
};
static CDMCGameMgrHelper g_GameMgrHelper;*/


static char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
static int team_scores[MAX_TEAMS];
static int num_teams = 0;
extern int gmsgTeamNames;
extern DLL_GLOBAL BOOL		g_fGameOver;
#define LMS_ROUND_DELAY 12
#define LMS_SPAWN_DELAY 4
CClanArena :: CClanArena()
{
//	m_VoiceGameMgr.Init(&g_GameMgrHelper, VGMode(HearPVS), gpGlobals->maxClients);
	m_Delay=1;
	m_DelayEnd=0;
	m_EndEnd=gpGlobals->time;
    gameended=1;

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
	
	m_iNextRoundTime = 0.0f;

	RecountTeams();
	
	m_hMessage.x            = 0.45f;
    m_hMessage.y            = 0.4f;
    m_hMessage.a1           = 0.0f;
    m_hMessage.a2           = 0.0f;

    m_hMessage.b1           = 255.0f;
    m_hMessage.g1           = 185.0f;
    m_hMessage.r1           = 120.0f;
	
	m_hMessage.b2           = 255.0f;
	m_hMessage.g2           = 255.0f;
    m_hMessage.r2           = 255.0f;
    
	m_hMessage.channel      = 4;
    m_hMessage.effect       = 2;
    m_hMessage.fxTime       = 0.5;			
    m_hMessage.fadeinTime   = 0.02;
    m_hMessage.fadeoutTime  = 0.5;    
    m_hMessage.holdTime     = 4.0;	


}

void CClanArena :: CheckLTS( )
{

	int m_iPlayersOnRed = 0;
	int m_iPlayersOnBlue = 0;
	int m_iDeadPlayersOnRed = 0;
	int m_iDeadPlayersOnBlue = 0;



	//This will go through all clients in the game
	for ( int i = 0; i <= gpGlobals->maxClients; i++ )
	{
		int team=-1;
		CBaseEntity *plr = UTIL_PlayerByIndex( i );
		CBasePlayer *pPlayer = (CBasePlayer *) plr;
		if ( plr )
		{//If the players team is red increase m_iPlayersOnRed
			CBasePlayer *pPlayer = (CBasePlayer *)plr;
			if (pPlayer)
			{
				if(!pPlayer->IsDormant())
				{
			if(pPlayer->pev->fuser2<gpGlobals->time)
			{
				pPlayer->StartObserver(pPlayer->pev->origin,pPlayer->pev->v_angle);
				pPlayer->pev->fuser2=gpGlobals->time + 20000;
				pPlayer->team=0;
			}
			
			if(!pPlayer->IsObserver())
			{
				if(FStrEq(pPlayer->m_szTeamName, team_names[0]))
					m_iPlayersOnRed++;
				else if(FStrEq(pPlayer->m_szTeamName, team_names[1]))
					m_iPlayersOnBlue++;
			}



	//		if(FStrEq(pPlayer->m_szTeamName, team_names[0])&&plr->IsAlive())
	//			m_iDeadPlayersOnRed++;

	//		else if(FStrEq(pPlayer->m_szTeamName, team_names[1])&&plr->IsAlive())
	//			m_iDeadPlayersOnBlue++;
		

		}
	}
	}
	}
		//Now we are out of the loop and now we will check wich team acctually wins this round.

	

		//Checking if Blue wins...
		if(m_iPlayersOnRed==0&&gameended!=1)
		{
			gameended=1;
			m_EndEnd=gpGlobals->time+5;
		}

		//Checking if red wins
		if(m_iPlayersOnBlue==0&&gameended!=1)
		{
			gameended=1;
			m_EndEnd=gpGlobals->time+5;
		}
		if(gameended==1 && m_EndEnd<gpGlobals->time)
		{
			StartDelay();
		}	
}

void CClanArena :: ClientDisconnected( edict_t *pClient )
{
	CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );
	pPlayer->StartObserver(pPlayer->pev->origin,pPlayer->pev->v_angle);
    pPlayer->team=0;
	pPlayer->MakeDormant();
	strncpy( pPlayer->m_szTeamName, "dummy team", TEAM_NAME_LENGTH );
	CHalfLifeMultiplay :: ClientDisconnected( pClient );
}

extern void respawn( entvars_t* pev, BOOL fCopyCorpse );

void CClanArena::NormalThink( void )
{
	CheckLTS( );
	
}

void CClanArena::DelayThink( void )
{
	//ALERT(at_console,"DelayThink. %f %f %f\n",m_DelayEnd-gpGlobals->time,m_DelayEnd,gpGlobals->time);
	if (gpGlobals->time>=m_DelayEnd)
	{
		//stop observer for every lms observer
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBasePlayer *pEnt = (CBasePlayer *)UTIL_PlayerByIndex( i );

				if ( pEnt && pEnt->IsPlayer() )
				{
					CBasePlayer *pPlayer = (CBasePlayer *)pEnt;

			        if (!( FStrEq( pPlayer->m_szTeamName, team_names[0] ) || FStrEq( pPlayer->m_szTeamName, team_names[1])))
				//	if ( pEnt->IsObserver()) 
					{
						//pEnt->m_iLFlags&=~LF_GAMERULE_OBSERVER;
						pEnt->pev->flags&=~FL_FROZEN;
						//pEnt->StopObserver();
						DisplaySelectTeamMenu( pEnt );	
						//JoinTeam( pEnt, TeamWithFewestPlayers( ));
						//pEnt->Spawn();
					}
				}
		}
		UTIL_ClientPrintAll( HUD_PRINTTALK, "The round has begun" );
		m_DelayEnd=gpGlobals->time+3;
		m_Delay=2;
		m_EndEnd=0;
		gameended=0;
	}
}

void CClanArena::StartDelay(void)
{
	char buf[128];
	*buf=NULL;
	m_Delay=1;
	m_DelayEnd=gpGlobals->time+LMS_ROUND_DELAY;
	int winningteam=-1;
	int health=0;
	int armor=0;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
			CBasePlayer *pEnt = (CBasePlayer *)UTIL_PlayerByIndex( i );
		

        	// notify everyone's HUD of the team change


		if(pEnt)
		{
			if ( !pEnt->IsObserver() )
			{
				//add points
				if(CountPlayers()>1) pEnt->AddPoints(1,FALSE);
				if (FStrEq( pEnt->m_szTeamName, team_names[1]))
				{
					winningteam=1;
				}
				if (FStrEq( pEnt->m_szTeamName, team_names[0]))
				{
					winningteam=2;
				}
				health+=pEnt->pev->health;
				armor+=pEnt->pev->armorvalue;
				CBasePlayerItem *m_pActiveItem = pEnt->m_pActiveItem;
				((CQuakeGun*)m_pActiveItem)->DestroyEffect();
				//sprintf( buf, "%s WINS THE ROUND WITH %i HEALTH AND %i ARMOR TO SPARE", STRING( pEnt->pev->netname ), (signed long)pEnt->pev->health, (signed long)pEnt->pev->armorvalue ); 
				//JoinTeam( pEnt, TeamWithFewestPlayers( ));	
				//pEnt->pev->flags|=FL_FROZEN;
			}
			pEnt->StartObserver(pEnt->pev->origin,pEnt->pev->v_angle);
			int team=-1;
			if (FStrEq( pEnt->m_szTeamName, team_names[1]))
			{
				team=1;
			}
			if ( FStrEq( pEnt->m_szTeamName, team_names[0] ))
			{
				team=0;
			}
			strncpy( pEnt->m_szTeamName, "dummy team", TEAM_NAME_LENGTH );
			if ( team==0)
			{
				ReJoinTeam(pEnt,TEAM1_NAME);
				pEnt->team=2;
			}
			if ( team==1)
			{
				ReJoinTeam(pEnt,TEAM2_NAME);
				pEnt->team=1;
			}
			if(team==-1)
			{
				DisplaySelectTeamMenu( pEnt );
			}
		}
	}

	if(winningteam==2)
	{
		sprintf( buf, "RED WINS THE ROUND WITH %i HEALTH AND %i ARMOR TO SPARE", health,armor); 
		float fLength = (float)strlen(buf);
		float fOffset = fLength*=0.0035;
		m_hMessage.x = 0.5-fOffset;
		m_hMessage.fadeoutTime=3;
		UTIL_HudMessageAll(m_hMessage, buf);
	}
	else
	{
		if(winningteam==1)
		{
			sprintf( buf, "BLUE WINS THE ROUND WITH %i HEALTH AND %i ARMOR TO SPARE", health,armor); 
			float fLength = (float)strlen(buf);
			float fOffset = fLength*=0.0035;
			m_hMessage.x = 0.5-fOffset;
			m_hMessage.fadeoutTime=3;
		UTIL_HudMessageAll(m_hMessage, buf);
		}
		else
		{	
			sprintf( buf, "DRAW", health,armor); 
			float fLength = (float)strlen(buf);
			float fOffset = fLength*=0.0035;
			m_hMessage.x = 0.5-fOffset;
			m_hMessage.fadeoutTime=3;
			UTIL_HudMessageAll(m_hMessage, buf);

		}
	}
	


		//show msg
		//char msg[255];
		//sprintf(msg,"%s won the round",STRING(pWinner->pev->netname));
		//UTIL_ClientPrintAll( HUD_PRINTTALK, msg );

		//focus on winning player
		/* DOESNT WORK
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBasePlayer *pEnt = (CBasePlayer *)UTIL_PlayerByIndex( i );

			if ( pEnt && pEnt->IsPlayer() && pEnt!=pWinner && pEnt->m_iLFlags&LF_GAMERULE_OBSERVER )
			{
				Vector vecWinner=pWinner->pev->origin+pWinner->pev->view_ofs;
				pEnt->pev->origin=vecWinner+pWinner->pev->v_angle.Normalize()*128+Vector(0,0,32);
				pEnt->pev->v_angle=vecWinner-pWinner->pev->origin;
			}
		}*/
	
	
}

/*CBasePlayer* CClanArena::FindWinner( void )
{
	int lnum=0,pnum=0;
	CBasePlayer *pLastEnt;
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt )
		{
			++pnum;
			if ( pEnt->IsAlive() && pEnt->IsPlayer() )
			{
				++lnum;
				pLastEnt=(CBasePlayer *)pEnt;
			}
		}
	}
//#define ONE_PLAYER_BUG
#ifndef ONE_PLAYER_BUG
	if (pnum>1)
	{
#endif
		if (lnum==1)
		{
			StartDelay(pLastEnt);
			return pLastEnt;
		}
		else if (lnum==0)
		{
			StartDelay(NULL);
			return (CBasePlayer *)CBaseEntity::Instance(INDEXENT(0));
		}
#ifndef ONE_PLAYER_BUG
	}
#endif
	return NULL;
}*/

CBasePlayer* CClanArena::FindLivingPlayer( void )
{
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsAlive() && pEnt->IsPlayer() )
			return (CBasePlayer*)pEnt;
	}
	return NULL;
}

void CClanArena::AwardPoints( CBasePlayer *pVictim, CBasePlayer *pKiller, entvars_t *pInflictor )
{
	return;
}

int CClanArena :: PlayersOnTeam( const char *szTeamName )
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
void CClanArena::PlayerThink( CBasePlayer *pPlayer )
{


	//pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);

}

extern cvar_t timeleft, fragsleft;
extern int CountPlayers( void );
void CClanArena :: Think ( void )
{
//	m_VoiceGameMgr.Update(gpGlobals->frametime);

	if (m_DelayEnd==0)
		m_DelayEnd=gpGlobals->time+LMS_SPAWN_DELAY;
	if (m_Delay==1 && CountPlayers()>1)
	{
		DelayThink();
	}
	else if (m_Delay==0)
		NormalThink();
	else if (m_Delay==2&&gpGlobals->time>=m_DelayEnd)
	{
			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

				if ( pEnt && pEnt->IsPlayer( ) )
				{
					CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
		
					if ( FStrEq( pPlayer->m_szTeamName, team_names[0] ) || FStrEq( pPlayer->m_szTeamName, team_names[1]))
					{
						pPlayer->RemoveAllItems( TRUE );
						pPlayer->pev->takedamage = DAMAGE_YES;
						if(pPlayer->m_isSpectator)
							pPlayer->StopObserver(); 
						else
						{
							pPlayer->pev->fuser2=gpGlobals->time + 20000;
							pPlayer->Spawn();							
						}
					}
				}
			}
		m_Delay=0;


	}

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

/*	float flFragLimit = fraglimit.value;
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
	//	frags_remaining = bestfrags;
	}*/

/*	// Updates when frags change
	if ( frags_remaining != last_frags )
	{
		g_engfuncs.pfnCvar_DirectSet( &fragsleft, UTIL_VarArgs( "%i", frags_remaining ) );
	}*/

	// Updates once per second
	if ( timeleft.value != last_time )
	{
		g_engfuncs.pfnCvar_DirectSet( &timeleft, UTIL_VarArgs( "%i", time_remaining ) );
	}

//	last_frags = frags_remaining;
	last_time  = time_remaining;
}

// DME
BOOL CClanArena::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	BOOL ret=CHalfLifeMultiplay::ClientConnected(pEntity,pszName,pszAddress,szRejectReason);
	if (ret)
	{
		CBasePlayer* pPlayer=FindLivingPlayer();
		CBasePlayer* pClient=(CBasePlayer*)CBaseEntity::Instance(pEntity);
		if (pClient)
		{
			if (pPlayer)
			{	
				pClient->pev->frags=0;
				pClient->m_iDeaths=0;
				pClient->StartObserver(pPlayer->pev->origin,pPlayer->pev->v_angle);
			}
			    
			else
				pClient->StartObserver(pClient->pev->origin,pClient->pev->v_angle);
			//pClient->m_iLFlags|=LF_GAMERULE_OBSERVER;
		}

		VARS(pEntity)->flags|=FL_FROZEN;
	}
	return ret;

	
}


void CClanArena::PlayerSpawn( CBasePlayer *pPlayer )
{
	BOOL		addDefault;
	CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<IT_AXE);


	
	addDefault = TRUE;
		while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
			{
				pWeaponEntity->Touch( pPlayer );
				addDefault = FALSE;
			}		
	
			if(addDefault)
			{
					
					pPlayer->StopObserver();
					//pPlayer->pev->flags |= ~FL_GODMODE;
					pPlayer->m_iQuakeItems &= ~IT_QUAD;
					pPlayer->pev->renderfx = kRenderFxNone;
					pPlayer->m_flSuperDamageFinished = 0.0;
					m_bPlayedQuadSound = FALSE;
					pPlayer->RemoveAllItems( TRUE );
					ArmPlayer(pPlayer);
					LoadPlayer(pPlayer);
					FillPlayer(pPlayer);
				
			}
				CHalfLifeMultiplay::PlayerSpawn( pPlayer );	
	
	
	
	
}

void CClanArena::JoinTeam(CBasePlayer *pPlayer, const char *pTeamName)
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
	//	pPlayer->Spawn();
	}
	//Else just switch teams
	else
	{
		ChangePlayerTeam( pPlayer, pTeamName, TRUE, TRUE );
		// recound stuff
		RecountTeams();
	}
}


void CClanArena::ReJoinTeam(CBasePlayer *pPlayer, const char *pTeamName)
{
//char text [1024]; 
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
        //sprintf( text, "%s joined the %s team\n", STRING(pPlayer->pev->netname), "Red" );
    //    UTIL_SayTextAll( text, pPlayer );
        //Zapper - Altered to comply with Half-Life log format standard
		//UTIL_LogPrintf( "\"%s<%i>\" joined the %s team\n", 
        //    STRING( pPlayer->pev->netname ), GETPLAYERUSERID( pPlayer->edict() ), "Red" );
	//	UTIL_LogPrintf( "\"%s<%i><%s><%s>\" joined team \"%s\"\n",
	//	STRING(pPlayer->pev->netname),
	//	GETPLAYERUSERID( pPlayer->edict() ),
	//	GETPLAYERAUTHID( pPlayer->edict() ),
	//	g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" ), "obi03" );
    } 
	else 
	{
        //sprintf( text, "%s joined the %s team\n", STRING(pPlayer->pev->netname), "Blue" );
    //    UTIL_SayTextAll( text, pPlayer );
        //Zapper - Altered to comply with Half-Life log format standard
		//UTIL_LogPrintf( "\"%s<%i>\" joined the %s team\n", 
        //    STRING( pPlayer->pev->netname ), GETPLAYERUSERID( pPlayer->edict() ), "Blue" );
	//	UTIL_LogPrintf( "\"%s<%i><%s><%s>\" joined team \"%s\"\n",
	//	STRING(pPlayer->pev->netname),
	//	GETPLAYERUSERID( pPlayer->edict() ),
	//	GETPLAYERAUTHID( pPlayer->edict() ),
	//	g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" ), "obi03" );
    }

//When observer, make it respawnable, let it join a team and then respawn
	if ( pPlayer->m_afPhysicsFlags & PFLAG_OBSERVER)
	{
		pPlayer->pev->deadflag = DEAD_RESPAWNABLE;
		ChangePlayerTeam( pPlayer, pTeamName, FALSE, FALSE );
		// recound stuff
		RecountTeams();
	//	pPlayer->Spawn();
	}
	//Else just switch teams
	else
	{
		ChangePlayerTeam( pPlayer, pTeamName, TRUE, TRUE );
		// recound stuff
		RecountTeams();
	}
}
extern bool g_bHaveMOTD;
//=========================================================
// ClientCommand
// the user has typed a command which is unrecognized by everything else;
// this check to see if the gamerules knows anything about the command
//=========================================================
BOOL CClanArena :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{ 

int damageFlags = DMG_GENERIC;
int clientIndex = pPlayer->entindex();
//char text[1024];

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
				pPlayer->team = 2;
				pPlayer->m_SpawnTeam = NULL;
				pPlayer->m_SpawnTeam = 1;
				return TRUE;

            }
            else if (slot == 2)
            {
                JoinTeam(pPlayer, TEAM2_NAME);
				pPlayer->team = 1;
				pPlayer->m_SpawnTeam = NULL;
				pPlayer->m_SpawnTeam = 2;
				return TRUE;

            }
            else if (slot == 3)
            {
				if( FStrEq( TeamWithFewestPlayers( ),TEAM2_NAME ))
					pPlayer->team=1;
				else
					pPlayer->team=2;
				JoinTeam( pPlayer, TeamWithFewestPlayers( ));
				return TRUE;

            }
        } 


}
    else if ( FStrEq( pcmd, "changeteam" ) )
    {
        //Make sure he's not dead
        //if (!pPlayer->IsAlive()) return TRUE;
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

	if ( FStrEq( pcmd, "break" ) )
	{	
		pPlayer->StartObserver(pPlayer->pev->origin,pPlayer->pev->v_angle);
		strncpy( pPlayer->m_szTeamName, "               ", TEAM_NAME_LENGTH );
		pPlayer->team=-1;
		char buf[256];
	    sprintf( buf, "%s IS NOT READY", STRING( pPlayer->pev->netname ));			
		float fLength = (float)strlen(buf);
		float fOffset = fLength*=0.0035;
		m_hMessage.x = 0.5-fOffset;
		UTIL_HudMessageAll(m_hMessage, buf);
		return TRUE;
	}

   return	CHalfLifeMultiplay::ClientCommand( pPlayer, pcmd );

	RecountTeams();
}

extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;


void CClanArena :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 1 );  // game mode teamplay
	MESSAGE_END();
}

//=========================================================
// InitHUD
//=========================================================
void CClanArena::InitHUD( CBasePlayer *pPlayer )
{
	//SetDefaultPlayerTeam( pPlayer );
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
        //strcat (szMenu, "3. Autoteam");
        ShowMenu (pPlayer, 0x7, 0, 0, szMenu);
    

		
}


void CClanArena::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
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
		m_DisableDeathMessages = TRUE;
		m_DisableDeathPenalty = TRUE;

		entvars_t *pevWorld = VARS( INDEXENT(0) );
		pPlayer->TakeDamage( pevWorld, pevWorld, 900, damageFlags );

		m_DisableDeathMessages = FALSE;
		m_DisableDeathPenalty = FALSE;
	}

	strncpy( pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH );
	g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );

	if ( !stricmp( pTeamName, TEAM1_NAME ) ) {
        g_engfuncs.pfnSetClientKeyValue( clientIndex, 
            g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
            "model", "obi03" );
        pPlayer->m_SpawnTeam = NULL;
		pPlayer->m_SpawnTeam = 1;
        strcpy( pPlayer->m_szTeamName, TEAM1_NAME );
    } else if ( !stricmp( pTeamName, TEAM2_NAME ) ) {
	    g_engfuncs.pfnSetClientKeyValue( clientIndex, 
            g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
            "model", "obi03" );
        pPlayer->m_SpawnTeam = NULL;
		pPlayer->m_SpawnTeam = 2;
        strcpy( pPlayer->m_szTeamName, TEAM2_NAME );
    } else {
        char buf[256];
        sprintf( buf, "bogus team name [%s]\n", pTeamName );
        ClientPrint( pPlayer->pev, HUD_PRINTCONSOLE, buf );
        //pPlayer->ctf_team = 0;
    } 

	SetPlayerColor (pPlayer, -1);

	// notify everyone's HUD of the team change

	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( clientIndex );
		WRITE_STRING( pPlayer->m_szTeamName );
	MESSAGE_END();

}


//=========================================================
// ClientUserInfoChanged
//=========================================================
void CClanArena::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
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
	
	char *psz;	

	psz = g_engfuncs.pfnInfoKeyValue( infobuffer, "topcolor" );
	if (atol (psz) != pPlayer ->m_nColor)
	{
		SetPlayerColor (pPlayer, -1);

	}

	
	psz = g_engfuncs.pfnInfoKeyValue( infobuffer, "bottomcolor" );
	if (atol (psz) != pPlayer ->m_nColor)
	{
		SetPlayerColor (pPlayer, -1);

	}

	 RecountTeams();
}
// DME
extern int gmsgDeathMsg;

//=========================================================
// Deathnotice. 
//=========================================================
void CClanArena::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
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

extern int CountPlayers( void );

void CClanArena::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	int frags=pVictim->pev->frags;
	CHalfLifeMultiplay::PlayerKilled(pVictim,pKiller,pInflictor);
	pVictim->pev->frags=frags;
	pVictim->m_iAmmoCells = 0;
	pVictim->m_iQuakeWeapon = (IT_AXE);
    
//	DeathNotice( pVictim, pKiller, pInflictor );	

//	pVictim->m_iDeaths += 1;

	

	if (CountPlayers()>1)
	{
		CBasePlayer* pPlayer=FindLivingPlayer();
		pVictim->pev->fuser2=gpGlobals->time+0.5;
//		pVictim->m_pActiveItem->DestroyEffect();

		CBasePlayerItem *m_pActiveItem = pVictim->m_pActiveItem;
		((CQuakeGun*)m_pActiveItem)->DestroyEffect();
		
	//	pVictim->StartObserver(pVictim->pev->origin,pVictim->pev->v_angle);        
	//	if (pPlayer)
		//	pVictim->StartObserver(pPlayer->pev->origin,pPlayer->pev->v_angle);
	//	else
	//		pVictim->StartObserver(pVictim->pev->origin,pVictim->pev->v_angle);
		//pVictim->m_iLFlags|=LF_GAMERULE_OBSERVER;
	//	pVictim->pev->flags|=FL_FROZEN;
	}
}


//=========================================================
// IsTeamplay
//=========================================================
BOOL CClanArena::IsTeamplay( void )
{
	return TRUE;
}

BOOL CClanArena::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
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
int CClanArena::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() )
		return GR_NOTTEAMMATE;

	//if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
	if ( !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
	
	{
		return GR_TEAMMATE;
	}

	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
BOOL CClanArena::ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target )
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
int CClanArena::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 0;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
		return -1;
	
	return 0;
}

//=========================================================
//=========================================================
const char *CClanArena::GetTeamID( CBaseEntity *pEntity )
{
	if ( pEntity == NULL || pEntity->pev == NULL )
		return "";

	// return their team name
	return pEntity->TeamID();
}


int CClanArena::GetTeamIndex( const char *pTeamName )
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


const char *CClanArena::GetIndexedTeamName( int teamIndex )
{
	if ( teamIndex < 0 || teamIndex >= num_teams )
		return "";

	return team_names[ teamIndex ];
}


BOOL CClanArena::IsValidTeam( const char *pTeamName ) 
{
	if ( !m_teamLimit )	// Any team is valid if the teamlist isn't set
		return TRUE;

	return ( GetTeamIndex( pTeamName ) != -1 ) ? TRUE : FALSE;
}

const char *CClanArena::TeamWithFewestPlayers( void )
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
void CClanArena::RecountTeams( void )
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
		/*	if ( tm == 0 )
				plr->team = 1;// red team
			else if ( tm == 1)
				plr->team = 2;// blue team
			else
				plr->team = 0;*/

			if ( tm < 0 ) // no team match found
			{ 
				if ( !m_teamLimit )
				{
					// add to new team
					tm = num_teams;
					num_teams++;
					team_scores[tm] = 0;
					strncpy( team_names[tm], pTeamName, MAX_TEAMNAME_LENGTH );
				}
			}

			if ( tm >= 0 )
			{
			//	team_scores[tm] += plr->pev->frags;
			}
		}
	}
}
//=========================================================
// Menustuff
//========================================================= 

extern int gmsgShowMenu; // the message # for ShowMenu 

void CClanArena::ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots, 
    int nDisplayTime, BOOL fNeedMore, char *pszText)
{
    MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pPlayer ->pev);
        WRITE_SHORT( bitsValidSlots);
        WRITE_CHAR( nDisplayTime );
        WRITE_BYTE( fNeedMore );
        WRITE_STRING (pszText);
    MESSAGE_END();
} 

int CClanArena::CountPlayers( void )
{
	int	num = 0;
	int team1=0;
	int team2=0;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		CBasePlayer *pPlr = (CBasePlayer *)UTIL_PlayerByIndex( i );

		if ( pPlr )
		{
			if(!pPlr->IsDormant())
			{
				if ( FStrEq( pPlr->m_szTeamName, team_names[0] ))
				{
					team1+=1;
				}
				if ( FStrEq( pPlr->m_szTeamName, team_names[1] ))
				{
					team2+=1;
				}

			}
		}
	}
	if(team1!=0&&team2!=0) return team1+team2;
	else return 0;

}




