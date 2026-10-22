/*

===== match_gamerules.cpp ========================================================

  Deathmatch Classic Match : TrenD*KilleR
  Functions governing the Match Modes. Rounds Code by 007 of HLE
  

*/


#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h" 
#include	"game.h"
#include	"items.h"
#include	"voice_gamemgr.h"
#include    "client.h"
#include    "dme_utility.h"
#include    "dme_match.h"
#include	"hltv.h"

extern int gmsgSayText;
extern int gmsgTeamInfo;
extern int gmsgScoreInfo;
extern int gmsgSpectator;
extern int gmsgServerName;
extern int gmsgGameMode;
extern bool g_bHaveMOTD;
extern cvar_t   startmatch;
extern int g_bPlayLongjumpSound; // define this in items.cpp, its explained later in this file (search for it below)
static int num_teams = 0;
static char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
static int team_scores[MAX_TEAMS];
CMatch::CMatch()
{
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
	m_bJustConnected = true;
	m_bInProgress = false;
	m_bStartSequence = false;
	m_nNumber = 0;
	fRoundTimerStart = 9999;
	fUserTimerStart = 9999;
	m_fExecCheckTime = 0.0f;
	m_fExecCheckTimeB = 0.0f;
	m_bDeux = false;
	m_bAllowRestrictedSpawning = false;	
	m_pWinner = NULL;
	m_pLoser = NULL;

	m_nBonus = DUEL_BONUS_NONE;
	memset( team_scores, 0, sizeof(team_scores) );
	m_IsReady = false;
	m_bStartTimer = false;

//	m_nVote = Vote_None;

	RecountTeams();


	
}

CMatch::~CMatch()
{
    ClearList();
}


void CMatch::UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 1 );  // game mode teamplay
	MESSAGE_END();

	
}

//=========================================================
// IsTeamplay
//=========================================================
BOOL CMatch::IsTeamplay( void )
{

	if((FStrEq(gamemode.string, "2on2")) || (FStrEq(gamemode.string, "3on3")) || (FStrEq(gamemode.string, "4on4")))
	{
		return TRUE;
	}
	else
		return FALSE;
}

void CMatch::PlayerThink( CBasePlayer *pPlayer )
{
	
	
		if(pPlayer->m_bUnSpawn && !pPlayer->m_isSpectator)
		{
			
			{			
				pPlayer->m_bUnSpawn = false;
				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles); 
			}
		}	
		

	
	CHalfLifeMultiplay::PlayerThink(pPlayer);
					
}



BOOL CMatch::FPlayerCanRespawn(CBasePlayer *pPlayer) 
{
	return TRUE;
}

void CMatch::ThinkLimit()
{
			
	CHalfLifeMultiplay::Think();
}

void CMatch::Think()
{
	int time_remaining = 0;
	

	if ( CVAR_GET_FLOAT( "warmup" ) == 1 ) 
	{
		SERVER_COMMAND("infinite_ammo 1\n");
		
	}

	if ( CVAR_GET_FLOAT( "startmatch" ) == 1 ) 
	{	
		SERVER_COMMAND("warmup 0\n");
		SERVER_COMMAND("infinite_ammo 0\n");
				
	}

	if ( CVAR_GET_FLOAT( "startmatch" ) == 0 ) 
	{	
		SERVER_COMMAND("warmup 1\n");		

	}
	
	
	if(FStrEq(gamemode.string, "1on1"))
	{
		if(MinimumClientsConnected())
		{
			if(!(m_bInProgress))
			{
				SERVER_COMMAND("startmatch 1\n");
					UpdateRound();
			}
		}
	}
	else if (FStrEq(gamemode.string, "dmm4"))
	{
		if(MinimumClientsConnected())
		{
			if(!(m_bInProgress))
			{
				SERVER_COMMAND("startmatch 1\n");
					UpdateRound();
				}
			}
		}
	else if (FStrEq(gamemode.string, "2on2"))
	{
		if(MinimumClientsConnected() && (PlayersOnTeam( TEAM_RED ) == 2) && PlayersOnTeam( TEAM_BLUE ) == 2)
		{
			if(!(m_bInProgress))
			{
				SERVER_COMMAND("startmatch 1\n");
				UpdateRound();
			}
			
		}
	}
	
	else if (FStrEq(gamemode.string, "3on3"))
	{
		if(MinimumClientsConnected() &&  (PlayersOnTeam( TEAM_RED ) == 3) && PlayersOnTeam( TEAM_BLUE ) == 3)
		{
			if(!(m_bInProgress))
			{
				SERVER_COMMAND("startmatch 1\n");
				UpdateRound();
			}
		}
	}
	else if (FStrEq(gamemode.string, "4on4"))
	{
		if(MinimumClientsConnected() &&  (PlayersOnTeam( TEAM_RED ) == 4) && PlayersOnTeam( TEAM_BLUE ) == 4)
		{
			if(!(m_bInProgress))
			{
				SERVER_COMMAND("startmatch 1\n");
				UpdateRound();
			}
		}
	}
	
	if(bNewMap)
	{
		bNewMap = 0;
	}
	if((fLastTime-10) > gpGlobals->time)
	{
		bNewMap = 1;
	}
	fLastTime = gpGlobals->time;

	// check for new map
	if(gpGlobals->time < m_nRoundTime)
	{
		bNewMap = 1;
		m_nRoundTime = gpGlobals->time;
	}
	if(gpGlobals->time < fUserTimerStart)
	{
		fUserTimerStart = gpGlobals->time;
	}

		float timer = roundtime.value*60;	
		float time = gpGlobals->time;
		//round timer write
		char buf[1024];	
	if(m_bStartTimer)
	{
		if ((int)fmod(((timer + m_nRoundTime) - time), 60) >= 10)  
		{
			sprintf(buf, "Time Left : %02d:%02d", (int)(((timer + m_nRoundTime) - time)/60), (int)fmod(((timer + m_nRoundTime) - time), 60));
		
		

			if (gpGlobals ->time > m_fExecCheckTime)
			{
			float fLength		= (float)strlen(buf);
			float fOffset		= fLength*=0.0035;
			hudtextparms_t      h_text;
			h_text.x            = 0.5-fOffset;
			h_text.y            = 0.9f;
			h_text.a1           = 0.0f;
			h_text.a2           = 0.0f;
			h_text.b1           = 30.0f;
			h_text.b2           = 255.0f;
			h_text.g1           = 85.0f;
			h_text.g2           = 255.0f;
			h_text.r1           = 255.0f;
			h_text.r2           = 255.0f;
			h_text.channel      = 3;
			h_text.effect       = 2;
			h_text.fxTime       = 0.5;
			h_text.fadeinTime   = 0.01;
			h_text.fadeoutTime  = 2;    
			h_text.holdTime     = 2;			
			
			UTIL_HudMessageAll(h_text, buf);
			

			// Update the check timer (every 30 seconds)
			m_fExecCheckTime = gpGlobals ->time + 30.0f;
			}
		}

		if (gpGlobals ->time > m_fExecCheckTimeB)
		{
			if ((int)fmod(((timer + m_nRoundTime) - time), 60) <= 10 && (int)(((timer + m_nRoundTime) - time)/60) == 0 )
			{
					sprintf(buf, "%02d:%02d", (int)(((timer + m_nRoundTime) - time)/60), (int)fmod(((timer + m_nRoundTime) - time), 60));
					float fLength		= (float)strlen(buf);
					float fOffset		= fLength*=0.0035;
					hudtextparms_t      h_text;
					h_text.x            = 0.5-fOffset;
					h_text.y            = 0.5f;
					h_text.a1           = 0.0f;
					h_text.a2           = 0.0f;
					h_text.b1           = 30.0f;
					h_text.b2           = 255.0f;
					h_text.g1           = 85.0f;
					h_text.g2           = 255.0f;
					h_text.r1           = 255.0f;
					h_text.r2           = 255.0f;
					h_text.channel      = 3;
					h_text.effect       = 2;
					h_text.fxTime       = 0.0;
					h_text.fadeinTime   = 0.01;
					h_text.fadeoutTime  = 0.01;    
					h_text.holdTime     = 1.0;			
			
					UTIL_HudMessageAll(h_text, buf);

					m_fExecCheckTimeB = gpGlobals ->time + 0.20f;
			}
			
		}

	
	}
		
	
	if(m_bStartSequence)
	{
		

	if(m_dStartMessage.TimeUp())
	{
			m_dStartMessage.ClearTime();
			

		for(int i=0; i < m_lEntity.size(); i++) 
		{
			CBasePlayer* pPlayer=NULL;
			pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[i]->v));
			
				
			if(m_nNumber = 1 )			{
				
				pPlayer->RemoveAllItems( TRUE );
				EMIT_SOUND( ENT(pPlayer->pev), CHAN_BODY, "misc/newstart.wav", 1, ATTN_NORM );
				
				

			}
		}
	}

		if(m_dStartRound.TimeUp())
		{
			m_dStartRound.AddTime(1);// add 1 second for the next count				
			
			{				
				m_nNumber++;
				
				if(m_nNumber > MAX_SPRITE_NUMBER)
				{
					
					RemoveAllEntites();
					ToggleSpawningRestriction(); // turn on selected spawning
					RespawnAllEntites();	
					SpawnValidPlayers(); // spawn the players up for this round
 					ToggleSpawningRestriction();  // turn off selected spawning, no one can spawn now
					m_bStartSequence = false;
					m_dStartRound.ClearTime();
					StartRoundTime();
					m_nNumber = 0;
					
				}
			}
		}
	}
	else 
	{
		if(m_dRoundTimer.TimeUp())			
		{
			TimeIsUp();
		}
		if(m_dEndMessage.TimeUp())
		{
 			AnounceOutcome();			
			m_dEndMessage.ClearTime();
			
			EndRound( );
		}

	}

	ThinkLimit();

//	CheckVote(); //MAC
	
}

void CMatch::TimeIsUp()
{
	if(FStrEq(gamemode.string, "1on1") || (FStrEq(gamemode.string, "dmm4")))
	{
		if(MinimumClientsConnected())
		{
		
		CBasePlayer* pPlayer0 = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
		CBasePlayer* pPlayer1 = GetClassPtr((CBasePlayer *)&(m_lEntity[1]->v));
		
		if(pPlayer0->pev->frags > pPlayer1->pev->frags)
		{
			m_nResult = RESULT_TIMEUP_D;
			m_pLoser = pPlayer1->edict();
			m_pWinner = pPlayer0->edict();

			m_dRoundTimer.ClearTime();
			m_dEndMessage.AddTime(END_DELAY);
			DropPlayer(pPlayer0->edict());
			DropPlayer(pPlayer1->edict());			
			
			
			EndRound();
			GoToIntermission();
				
		}
		else if(pPlayer0->pev->frags < pPlayer1->pev->frags)
		{
			m_nResult = RESULT_TIMEUP_D;
			m_pLoser = pPlayer0->edict();
			m_pWinner = pPlayer1->edict();
		
			m_dRoundTimer.ClearTime();
			m_dEndMessage.AddTime(END_DELAY);
			DropPlayer(pPlayer0->edict());
			DropPlayer(pPlayer1->edict());

			
			EndRound( );
			GoToIntermission();
			
			
		}
		else // same health same hev, no win!
		{
			// not really a winner, they are both losers, but we will use thsee pointers at our convience for later
			m_pLoser = pPlayer1->edict();
			m_pWinner = pPlayer0->edict();
			m_nResult = RESULT_DRAW_D;

			m_dRoundTimer.ClearTime();
			m_dEndMessage.AddTime(END_DELAY);
			DropPlayer(pPlayer0->edict());
			DropPlayer(pPlayer1->edict());
			
			
			EndRound( );
			GoToIntermission();
		}
		
		}
	}
	else if((FStrEq(gamemode.string, "2on2")) || (FStrEq(gamemode.string, "3on3")) || (FStrEq(gamemode.string, "4on4")))
	{
		
		EndRound( );
		GoToIntermission();
	}
	
}

void CMatch::StartRoundTime()
{
	m_nRoundTime = roundtime.value*60;
	
	if(m_nRoundTime >=1000)
		m_nRoundTime=999;
	else if(m_nRoundTime<=0)
		m_nRoundTime = 0;
	m_dRoundTimer.AddTime(m_nRoundTime);

	
	
}


// called from dtor
void CMatch::ClearList(void)
{
    m_lEntity.clear();
}

// add at bottom of list when a client connects
void CMatch::AddPlayer(edict_t* pEntity)
{
	m_lEntity.push_back(pEntity);
    UpdateRound();
}

bool CMatch::LookUpPlayer(edict_t* pEntity)
{
	for(int i = 0; i < m_lEntity.size(); i++)
	{
		if(pEntity == m_lEntity[i])
			return true;
	}
	return false;
}

bool CMatch::IsInRound(edict_t* pEntity)
{
	if(MinimumClientsConnected())
	{
		for(int i = 0; i < MinimumClients(); i++)
		{
			if(pEntity == m_lEntity[i])
				return true;
		}
	}
	return false;
}

void CMatch::MovePlayer(edict_t* pEntity)
{
	m_lEntity.remove(pEntity);
	m_lEntity.push_back(pEntity);
	UpdateRound();
	return;
}
// remove from list when client disconnects
void CMatch::DropPlayer(edict_t* pEntity)
{
	bool inRound = false;
	if(!LookUpPlayer(pEntity))
		return;
	if(IsInRound(pEntity))		
		inRound = true;	
	m_lEntity.remove(pEntity);
	if(inRound && !m_nResult)
		EndRound();
	return;
}

bool CMatch::MinimumClientsConnected()
{
	if(FStrEq(gamemode.string, "1on1") || (FStrEq(gamemode.string, "dmm4")))
	{
		return m_lEntity.size() >=2 ? true : false;
	}
	else

	return m_lEntity.size() >=(maxclients.value) ? true : false;
}

int CMatch::MinimumClients()
{
	if(FStrEq(gamemode.string, "1on1") || (FStrEq(gamemode.string, "dmm4")))
	{
		return 2;
	}
	else

	return (maxclients.value);
}

extern int gmsgDeathMsg;

//=========================================================
// Deathnotice. 
//=========================================================
void CMatch::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	if ( m_DisableDeathMessages )
		return;
	if((FStrEq(gamemode.string, "2on2")) || (FStrEq(gamemode.string, "3on3")) || (FStrEq(gamemode.string, "4on4")))
	{
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
	}

	CHalfLifeMultiplay::DeathNotice( pVictim, pKiller, pevInflictor );
}

int CMatch::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
		return -1;

	
	return 1;
}
void CMatch::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{

	if ( !m_DisableDeathPenalty )
	{
		CHalfLifeMultiplay::PlayerKilled( pVictim, pKiller, pInflictor );
		RecountTeams();
	}
		
}

BOOL CMatch::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	return CHalfLifeMultiplay::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
}



void CMatch :: ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
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

	if ( !stricmp( pTeamName, TEAM_RED ) ) {
        g_engfuncs.pfnSetClientKeyValue( clientIndex, 
            g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
            "model", "obi03" );
        //pPlayer->team = 1;
        strcpy( pPlayer->m_szTeamName, TEAM_RED );
    } else if ( !stricmp( pTeamName, TEAM_BLUE ) ) {
	    g_engfuncs.pfnSetClientKeyValue( clientIndex, 
            g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
            "model", "obi03" );
        //pPlayer->team = 2;
        strcpy( pPlayer->m_szTeamName, TEAM_BLUE );
    } else {
        char buf[256];
        sprintf( buf, "bogus team name [%s]\n", pTeamName );
        ClientPrint( pPlayer->pev, HUD_PRINTCONSOLE, buf );
        //pPlayer->ctf_team = 0;
    } 

	// notify everyone's HUD of the team change

	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( clientIndex );
		WRITE_STRING( pPlayer->m_szTeamName );
	MESSAGE_END();

	
}

int CMatch::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	if((FStrEq(gamemode.string, "2on2")) || (FStrEq(gamemode.string, "3on3")) || (FStrEq(gamemode.string, "4on4")))
	{
		// half life multiplay has a simple concept of Player Relationships.
		// you are either on another player's team, or you are not.
		if ( !pPlayer || !pTarget || !pTarget->IsPlayer() )
			return GR_NOTTEAMMATE;

		if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
		{
			return GR_TEAMMATE;
		}
	}

	return GR_NOTTEAMMATE;
	
}

const char *CMatch::GetTeamID( CBaseEntity *pEntity )
{
	
	if ( pEntity == NULL || pEntity->pev == NULL )
		return "";
	
	// return their team name
	return pEntity->TeamID();
	
}

extern int gmsgTeamNames;

void CMatch::InitHUD( CBasePlayer *pPlayer )
{
	char buf[128];
	*buf=NULL;
	pPlayer->m_iNextTeam = 0;		// unknown next team
	RecountTeams();
	// populate the TFC team VGUI

	if((FStrEq(gamemode.string, "2on2")) || (FStrEq(gamemode.string, "3on3")) || (FStrEq(gamemode.string, "4on4")))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgTeamNames, NULL, pPlayer->edict());
		WRITE_BYTE(2); // Two teams. (You can have up to 4.)
		WRITE_STRING(TEAM_RED); // You need to pass the teams as literal strings
		WRITE_LONG(0x00ff0000);
		WRITE_STRING(TEAM_BLUE);
		WRITE_LONG(0x000000ff);
	MESSAGE_END();

	}
	else
		MESSAGE_BEGIN( MSG_ONE, gmsgTeamNames, NULL, pPlayer->edict() );
		WRITE_BYTE( 2 );
		WRITE_STRING( TEAM2 );
		MESSAGE_END();

	
	
	// run CHalfLifeMultiplay's InitHUD

	CHalfLifeMultiplay::InitHUD( pPlayer );

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

	
}	


void CMatch::ClientDisconnected(edict_t* pEntity)
{
	
		if(IsInRound(pEntity))
		{
			DropPlayer(pEntity);	
			EndRound();
			GoToIntermission();
		}
		else
		{
			DropPlayer(pEntity);
	}
	
	if(MinimumClientsConnected()) 
		m_bJustConnected = true; 
	CHalfLifeMultiplay::ClientDisconnected(pEntity);
	//DME
			// Tell all clients this player isn't a spectator anymore
			MESSAGE_BEGIN( MSG_ALL, gmsgSpectator );  
				WRITE_BYTE( ENTINDEX(pEntity) );
				WRITE_BYTE( 0 );
			MESSAGE_END();
}

void CMatch::StartRound()
{
	m_dStartMessage.AddTime(ROUND_DELAY_M);
	m_dStartRound.AddTime(ROUND_DELAY_M);
	m_bAllowRestrictedSpawning	= false;
	m_bStartSequence			= true;
	m_bInProgress				= true;
	m_nNumber					= 0;
	
	m_pWinner					= NULL;
	m_pLoser					= NULL;
	m_nResult					= RESULT_NONE_D;
}





void CMatch::EndRound()
{
	m_bInProgress		= false;
	m_nNumber			= 0;
	m_bStartTimer		= false;
	m_bStartSequence	= false;
	m_nRoundTime		= 0;
	
	RemoveAllEntites();	
	
}



void CMatch::UpdateRound()
{
	if(m_bInProgress)
	{
		if(IsOver())		
			
			SERVER_COMMAND("startmatch 0\n");
			EndRound();			
			
		
		
	}
	else if(MinimumClientsConnected())
	{
		if(m_dEndMessage.GetTimeLeft()!=0)
			return;
			

			if ( CVAR_GET_FLOAT( "startmatch" ) == 1 ) 
			{
					
					StartRound();
			}
				

		
		if(m_bJustConnected)
		{	
			// this is not used, ytou can add more time between rounds ir there are 2 ppl in the server and one i still connecting, i never had this problem yet tho
			m_bJustConnected = false;
			//m_dStartMessage.AddTime(10);
			//m_dStartRound.AddTime(10);
		}
	}
}


float CMatch :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;//now!
}




void CMatch::PlayerSpawn( CBasePlayer *pPlayer )
{
	BOOL		addDefault;
	CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<IT_AXE);

	
	if ( CVAR_GET_FLOAT( "startmatch" ) == 1 )
	{	
	
	    
		///////////////////////
		///START SPAWN CHECK///
		///////////////////////
	
			bool UnSpawn = true;

			if(MinimumClientsConnected())
			{				
				if(IsInRound(pPlayer->edict()))
					UnSpawn = false;
			}

			if(UnSpawn)
			{
				pPlayer->m_bUnSpawn = true;
				return;
			}

			if ((!FStrEq(pPlayer->m_szTeamName, TEAM_WARMUP)) && (!FStrEq(pPlayer->m_szTeamName, TEAM2)) && (!FStrEq(pPlayer->m_szTeamName, TEAM_RED)) && (!FStrEq(pPlayer->m_szTeamName, TEAM_BLUE))) 
			{
				
				RemovePlayer(pPlayer);
				addDefault = FALSE;
			}
			else 
			{        
					
				addDefault = TRUE;
			}
			

			while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
			{
				pWeaponEntity->Touch( pPlayer );
				addDefault = FALSE;
			}
			
			if(addDefault)
			{
					
					pPlayer->StopObserver();
					if(FStrEq(gamemode.string, "1on1") || (FStrEq(gamemode.string, "dmm4")))
					{
						ChangePlayerTeam( pPlayer, TEAM2, FALSE, FALSE );
					}
					
					pPlayer->m_iQuakeItems &= ~IT_QUAD;
					pPlayer->pev->renderfx = kRenderFxNone;
					pPlayer->m_flSuperDamageFinished = 0.0;
					m_bPlayedQuadSound = FALSE;
					pPlayer->RemoveAllItems( TRUE );
					ArmPlayerDefault(pPlayer);
					LoadPlayerDefault(pPlayer);
					FillPlayerDefault(pPlayer);
					m_bStartTimer = true;
			}
				CHalfLifeMultiplay::PlayerSpawn( pPlayer );
				
	}

	else if ( CVAR_GET_FLOAT( "startmatch" ) == 0 )
	{
			addDefault = TRUE;

			while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
			{
				pWeaponEntity->Touch( pPlayer );
				addDefault = FALSE;
			}

				if(addDefault)
				{	
					
					//ResumePlayer(pPlayer);
					ArmPlayer(pPlayer);
					LoadPlayer(pPlayer);
					FillPlayer(pPlayer);
					pPlayer->pev->flags |= FL_GODMODE;
				}
	}
			
			//CHalfLifeMultiplay::PlayerSpawn( pPlayer );	

	
}
		
	



void CMatch::VersesMessage()
{
	m_lEntity.clean();
	char buf[128];
	*buf=NULL;
	CBasePlayer *pPlayer;
	entvars_t *pev = &m_lEntity[0]->v;
	pPlayer = GetClassPtr((CBasePlayer *)pev);
	pev = &m_lEntity[0]->v;
	const char* defender = STRING(pev->netname);
	pev = &m_lEntity[1]->v;
	pPlayer = GetClassPtr((CBasePlayer *)pev);
	pev = &m_lEntity[1]->v;
	const char* challanger = STRING(pev->netname);
	strcpy(buf, defender);
	strcat(buf," vs. ");
	strcat(buf, challanger);
	float fLength = (float)strlen(buf);
	float fOffset = fLength*=0.0035;
	m_hMessage.x = 0.5-fOffset;
	UTIL_HudMessageAll(m_hMessage, buf);
}


void CMatch::AnounceOutcome()
{
	char buf[128];
	*buf=NULL;
	
	CBasePlayer* pWinner;
	CBasePlayer* pLoser;
	

	

	switch(m_nResult)
	{

	case RESULT_NONE_D:

		break;

	case RESULT_DRAW_D:

		if(MinimumClientsConnected())
		{
			m_pWinner->v.flags |= FL_GODMODE;
			m_pLoser->v.flags |= FL_GODMODE;
			

			pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
			pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));

			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pWinner->edict()) );
			WRITE_SHORT( pWinner->pev->frags );
			WRITE_SHORT( pWinner->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( pWinner->pev->team );
			MESSAGE_END();


			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pWinner->edict()) );
			WRITE_SHORT( pLoser->pev->frags );
			WRITE_SHORT( pLoser->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( pLoser->pev->team );
			MESSAGE_END();
		}
		strcpy(buf, "Draw");
		break;

	case RESULT_TIE_D:
		
		strcpy(buf, "Tie");
		break;
	case RESULT_TIMEUP_D:
	
		if(MinimumClientsConnected())
		{
			CBasePlayer* pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
			CBasePlayer* pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
			pWinner->m_iWins+=1;
			strncpy(buf, "Time up\n", 128);
			strncat(buf, STRING(m_pWinner->v.netname), 128);
			strncat(buf," HAS DEFEATED ", 128);
			strncat(buf, STRING(m_pLoser->v.netname),128 );
			
			

			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pWinner->edict()) );
			WRITE_SHORT( pWinner->pev->frags );
			WRITE_SHORT( pWinner->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( pWinner->pev->team );
			MESSAGE_END();

			

			DropPlayer(pWinner->edict());
			DropPlayer(pLoser->edict());

		}
		break;
		

	case RESULT_SUICIDE_D://///////////////////

		if(MinimumClientsConnected())
		{
			m_lEntity[0]->v.flags |= FL_GODMODE;
			// give the winner a win
			pWinner = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
			pWinner->m_iWins+=1;

			strncpy(buf, STRING(m_pLoser->v.netname), 128);
			strncat(buf," has killed himself", 128);

			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pWinner->edict()) );
			WRITE_SHORT( pWinner->pev->frags );
			WRITE_SHORT( pWinner->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( pWinner->pev->team );
			MESSAGE_END();

		}
		break;

	case RESULT_FORFEIT_D://///////////////////

		pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));	
		pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
		pLoser->m_iWins-=1;

		strncpy(buf, STRING(m_pLoser->v.netname), 128);
		strncat(buf," has forfeited", 128);

		DropPlayer(pWinner->edict());
		DropPlayer(pLoser->edict());	

		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( ENTINDEX(pLoser->edict()) );
		WRITE_SHORT( pLoser->pev->frags );
		WRITE_SHORT( pLoser->m_iDeaths );
		WRITE_SHORT( 0 );
		WRITE_SHORT( pLoser->pev->team );
		MESSAGE_END();

		break;

	case RESULT_DEFEAT_D://////////////////////
		{

		

		if(MinimumClientsConnected())
		{

			
			
			strncpy(buf, STRING(m_pWinner->v.netname), 128);
			strncat(buf," HAS DEFEATED ", 128);
			strncat(buf, STRING(m_pLoser->v.netname),128 );
			
			
		
		
			// add a win to this player
			pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
			pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
			pWinner->pev->flags |= FL_GODMODE;
			pLoser->pev->flags |= FL_GODMODE;			
			
			
			if(m_nBonus == DUEL_BONUS_FLAWLESS)
			{
				CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
				if(pPlayer->m_iDeaths == 0)
				{
					EMIT_SOUND( ENT(pWinner->pev), CHAN_VOICE, "misc/flawless.wav", 1, ATTN_NORM );
					
				}
			}
			else if(m_nBonus == DUEL_BONUS_OWNED)
			{
				CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
				if(pPlayer->m_iDeaths <= 5 )
				{
					EMIT_SOUND( ENT(pWinner->pev), CHAN_VOICE, "misc/excelent.wav", 1, ATTN_NORM );
					
				}
			}
						
						

			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pWinner->edict()) );
			WRITE_SHORT( pWinner->pev->frags );
			WRITE_SHORT( pWinner->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( pWinner->pev->team );
			MESSAGE_END();


			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pLoser->edict()) );
			WRITE_SHORT( pLoser->pev->frags );
			WRITE_SHORT( pLoser->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( pLoser->pev->team );
			MESSAGE_END();

		}
		break;
		}
	default:
		break;
	}

	float fLength = (float)strlen(buf);
	float fOffset = fLength*=0.0035;
	m_hMessage.x = 0.5-fOffset;
	UTIL_HudMessageAll(m_hMessage, buf);
	
}

void CMatch::ToggleSpawningRestriction()
{

	m_bAllowRestrictedSpawning =! m_bAllowRestrictedSpawning;
}

void CMatch::SpawnValidPlayers()
{
	CBasePlayer* pPlayer=NULL;
	if(!MinimumClientsConnected())
	{
		UpdateRound();
		return;
	}
	
	for(int i=0; i < m_lEntity.size(); i++) 
	{
		pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[i]->v));
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE( ENTINDEX(pPlayer->edict()) );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
		WRITE_SHORT( GetTeamIndex( pPlayer->m_szTeamName ) + 1 );
		MESSAGE_END();

		if(i < MinimumClients())
		{
			pPlayer->RemoveAllItems( TRUE );
			pPlayer->pev->takedamage = DAMAGE_YES;
			pPlayer->pev->frags = 0;
			pPlayer->m_iDeaths = 0;
			 if(pPlayer->m_isSpectator)
				pPlayer->StopObserver();
			 else
				pPlayer->Spawn();
		 }
		else
			{
				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
			}	
		}
		
}



BOOL CMatch::CanStartObserver(CBasePlayer *pPlayer)
{
	if(!pPlayer->m_isSpectator)
	{
		if(IsInRound(pPlayer->edict()))
		{
			if(!m_bStartSequence)
			{					
				if(FStrEq(gamemode.string, "1on1") || (FStrEq(gamemode.string, "dmm4")))
				{
					//CBasePlayer* pPlayer0 = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
					//CBasePlayer* pPlayer1 = GetClassPtr((CBasePlayer *)&(m_lEntity[1]->v));
					m_nResult = RESULT_FORFEIT_D;
					//m_pLoser = pPlayer0->edict();
					//m_pWinner = pPlayer1->edict();
					m_dEndMessage.AddTime(END_DELAY);
					//DropPlayer(m_pLoser);
					//DropPlayer(m_pWinner);
					//pPlayer0->StartObserver(pPlayer0->pev->origin, pPlayer0->pev->angles);
					//pPlayer1->StartObserver(pPlayer1->pev->origin, pPlayer1->pev->angles);	
					//m_lEntity[0]->v.takedamage = DAMAGE_NO; // immediatly stop damage
					EndRound( );
					GoToIntermission();
				
				}
							
			}
		}
		return TRUE;
	}
	return FALSE;
}

// prevents the can from droping on a deal player
BOOL CMatch::CanStopObserver(CBasePlayer *pPlayer)
{
	return FALSE;
}

// prevents the can from droping on a deal player
int CMatch::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

// prevents the can from droping on a deal player
int CMatch::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

// checks to see if the round is ended
bool CMatch::IsOver()
{
	if(MinimumClientsConnected())
	{
		CBasePlayer* pPlayer = NULL;
		int nAlive=0;
		for(int i = 0; i < MinimumClients(); i++)
		{
			pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[i]->v));
			if(pPlayer->IsAlive())
				nAlive++;
		}
		if(nAlive < MinimumClients())
		{
			if(!m_bStartSequence)
				return true;
		}
	}
	return false;
}

void CMatch :: ResetRound( )
{
	// reset players

	CBaseEntity *pEnt = NULL;
	CBasePlayer *pPlayer = NULL;

	for( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pEnt = UTIL_PlayerByIndex( i );
		pPlayer = (CBasePlayer *)pEnt;

		if( pPlayer && pPlayer->IsPlayer() )
		{
			RemovePlayer(pPlayer);
			EndRound();
		}
	}

	
}


// this deals with adding and removing the player from the active queue, if a client types "break" 
// he will no longer spawn or play in the round
void CMatch::RemovePlayer(CBasePlayer* pPlayer)
{

	pPlayer->m_bIsPlaying = false;

	if(CanStartObserver(pPlayer))
	{
		pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
	}
	pPlayer->m_bIsPlaying = false;
	DropPlayer(pPlayer->edict());
	
}

void CMatch::IdlePlayer(CBasePlayer* pPlayer)
{

	pPlayer->m_bIsPlaying = false;
	DropPlayer(pPlayer->edict());
}

// same as above, self explanitory
void CMatch::ResumePlayer(CBasePlayer* pPlayer)
{
	
	pPlayer->m_bIsPlaying = true;
	if(!LookUpPlayer(pPlayer->edict()))
		AddPlayer(pPlayer->edict());
	
	
}




extern CVoiceGameMgr m_VoiceGameMgr;

BOOL CMatch :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	
	char text[1024];
	CBasePlayer::_Team nTeam;

	if ( FStrEq( pcmd, "menuselect" ) )
	{
		if ( CMD_ARGC() < 2 )
			return TRUE;

		int slot = atoi( CMD_ARGV(1) );

	/*	// select the item from the current menu
        switch(pPlayer->m_nMenu)
		case CBasePlayer::Menu_VoteCast:
			if (slot == 1)
			{
				pPlayer ->m_nVote = CBasePlayer::Vote_Yes;
				
			}
			else if (slot == 2)
			{
				pPlayer ->m_nVote = CBasePlayer::Vote_No;
				
			}
			//break;*/
			
	
		switch(pPlayer->m_nMenu)	
        case CBasePlayer::Menu_ChangeTeam:
        {
			
			if ( slot == 1 )
			{
				nTeam = (CBasePlayer::_Team) (CBasePlayer::Team_Team0 );
				if((FStrEq(gamemode.string, "2on2")))
				{

					if(PlayersOnTeam( TEAM_RED ) <= 1)
					{	
						ResumePlayer(pPlayer);
						JoinTeam(pPlayer, TEAM_RED);
						sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
						UTIL_SayTextAll( text, pPlayer );
						return TRUE;
					}
					else
						sprintf( text, "* Only 2 Players Allowed Per Team" );
						UTIL_SayText( text, pPlayer );
						return FALSE;
				}
				else if((FStrEq(gamemode.string, "3on3")))
				{
					if(PlayersOnTeam( TEAM_RED ) <= 2)
					{	
						ResumePlayer(pPlayer);	
						JoinTeam(pPlayer, TEAM_RED);
						sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
						UTIL_SayTextAll( text, pPlayer );
						return TRUE;
					}
					else
						sprintf( text, "* Only 3 Players Allowed Per Team" );
						UTIL_SayText( text, pPlayer );
						return FALSE;
				}
				else if((FStrEq(gamemode.string, "4on4")))
				{
					if(PlayersOnTeam( TEAM_RED ) <= 3)
					{	
						ResumePlayer(pPlayer);
						JoinTeam(pPlayer, TEAM_RED);
						sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
						UTIL_SayTextAll( text, pPlayer );
						return TRUE;
					}
					else
						sprintf( text, "* Only 4 Players Allowed Per Team" );
						UTIL_SayText( text, pPlayer );
						return FALSE;
						
				}
			}
            else if (slot == 2)
            {
				nTeam = (CBasePlayer::_Team) (CBasePlayer::Team_Team1 );
				if((FStrEq(gamemode.string, "2on2")))
				{

					if(PlayersOnTeam( TEAM_BLUE ) <= 1)
					{	
						ResumePlayer(pPlayer);
						JoinTeam(pPlayer, TEAM_BLUE);
						sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
						UTIL_SayTextAll( text, pPlayer );
						return TRUE;
					}
					else
						sprintf( text, "* Only 2 Players Allowed Per Team" );
						UTIL_SayText( text, pPlayer );
						return FALSE;
				}
				else if((FStrEq(gamemode.string, "3on3")))
				{
					if(PlayersOnTeam( TEAM_BLUE ) <= 2)
					{	
						ResumePlayer(pPlayer);
						JoinTeam(pPlayer, TEAM_BLUE);
						sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
						UTIL_SayTextAll( text, pPlayer );
						return TRUE;
					}
					else
						sprintf( text, "* Only 3 Players Allowed Per Team" );
						UTIL_SayText( text, pPlayer );
						return FALSE;
				}
				else if((FStrEq(gamemode.string, "4on4")))
				{
					if(PlayersOnTeam( TEAM_BLUE ) <= 3)
					{	
						ResumePlayer(pPlayer);				
						JoinTeam(pPlayer, TEAM_BLUE);
						sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
						UTIL_SayTextAll( text, pPlayer );
						return TRUE;
					}
					else
						sprintf( text, "* Only 4 Players Allowed Per Team" );
						UTIL_SayText( text, pPlayer );
						return FALSE;
						
				}
            }
            else if (slot == 3)
            {

				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
				sprintf(text, "* %s Wants To Spectate\n", STRING( pPlayer->pev->netname ));
				UTIL_SayTextAll( text, pPlayer );
				return TRUE;
            }
        } 

	return TRUE;
	}
    else if ( FStrEq( pcmd, "changeteam" ) )
    {
        //Make sure he's not dead
        //if (!pPlayer->IsAlive()) return TRUE; 
		char szMenu [512];
        pPlayer->m_nMenu = CBasePlayer::Menu_ChangeTeam;
        strcpy (szMenu, "Select a team\n\n");
        strcat (szMenu, "1. Red Team\n");
        strcat (szMenu, "2. Blue Team\n\n");
        strcat (szMenu, "3. Spectate");
        ShowMenu (pPlayer, 0x7, 0, 0, szMenu);


		return TRUE;
	}

	
	
	if ( FStrEq( pcmd, "break" ) )
	{	
		RemovePlayer(pPlayer);
		return TRUE;	
			
	}			
	
	

	
	if ( FStrEq( pcmd, "ready" ) )
	{	
		if ( CVAR_GET_FLOAT( "startmatch" ) == 0 ) 
		{			
			if(FStrEq(gamemode.string, "1on1")  && PlayersOnTeam( TEAM_WARMUP ) <=1)
			{
				//JoinTeam(pPlayer, TEAM_WARMUP);
				ResumePlayer(pPlayer);
				ChangePlayerTeam( pPlayer, TEAM_WARMUP, FALSE, FALSE );
				pPlayer->StopObserver();			 
				pPlayer->Spawn();
				sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
				UTIL_SayTextAll( text, pPlayer );
				return TRUE;
			}
			else if (FStrEq(gamemode.string, "dmm4") && PlayersOnTeam( TEAM_WARMUP ) <=1)
			{
				//JoinTeam(pPlayer, TEAM_WARMUP);
				ResumePlayer(pPlayer);
				ChangePlayerTeam( pPlayer, TEAM_WARMUP, FALSE, FALSE );
				pPlayer->StopObserver();			 
				pPlayer->Spawn();
				sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
				UTIL_SayTextAll( text, pPlayer );
				return TRUE;
			}

			else if((FStrEq(gamemode.string, "2on2")) && PlayersOnTeam( TEAM_WARMUP ) <=3)
			{
				if(pPlayer->m_bHadFirstSpawn && g_bHaveMOTD)
				{
					char szMenu [512];
					pPlayer->m_nMenu = CBasePlayer::Menu_ChangeTeam;
					strcpy (szMenu, "Select a team\n\n");
					strcat (szMenu, "1. Red Team\n");
					strcat (szMenu, "2. Blue Team\n\n");
					strcat (szMenu, "3. Spectate");
					ShowMenu (pPlayer, 0x7, 0, 0, szMenu);
					return TRUE;
				}
			}
			else if((FStrEq(gamemode.string, "3on3") && PlayersOnTeam( TEAM_WARMUP ) <=5))
			{
				if(pPlayer->m_bHadFirstSpawn && g_bHaveMOTD)
				{
					char szMenu [512];
					pPlayer->m_nMenu = CBasePlayer::Menu_ChangeTeam;
					strcpy (szMenu, "Select a team\n\n");
					strcat (szMenu, "1. Red Team\n");
					strcat (szMenu, "2. Blue Team\n\n");
					strcat (szMenu, "3. Spectate");
					ShowMenu (pPlayer, 0x7, 0, 0, szMenu);
					return TRUE;
				}
			}
			else if((FStrEq(gamemode.string, "4on4") && PlayersOnTeam( TEAM_WARMUP ) <=7))
			{
				if(pPlayer->m_bHadFirstSpawn && g_bHaveMOTD)
				{
					char szMenu [512];
					pPlayer->m_nMenu = CBasePlayer::Menu_ChangeTeam;
					strcpy (szMenu, "Select a team\n\n");
					strcat (szMenu, "1. Red Team\n");
					strcat (szMenu, "2. Blue Team\n\n");
					strcat (szMenu, "3. Spectate");
					ShowMenu (pPlayer, 0x7, 0, 0, szMenu);
					return TRUE;

				}
			}
			
			
		}
		if(FStrEq(gamemode.string, "1on1") && PlayersOnTeam( TEAM_WARMUP ) == 2)
		{
			sprintf( text, "* Only 2 Players Allowed in 1on1" );
			UTIL_SayText( text, pPlayer );
			return FALSE;
		}
		else if (FStrEq(gamemode.string, "dmm4"))
		{
			sprintf( text, "* Only 2 Players Allowed in dmm4" );
			UTIL_SayText( text, pPlayer );
			return FALSE;
		}
		else if((FStrEq(gamemode.string, "2on2") && PlayersOnTeam( TEAM_RED ) == 2 && PlayersOnTeam( TEAM_BLUE ) == 2))
		{
			sprintf( text, "* Teams are full" );
			UTIL_SayText( text, pPlayer );
			return FALSE;
		}
		else if((FStrEq(gamemode.string, "3on3") && PlayersOnTeam( TEAM_RED ) == 3 && PlayersOnTeam( TEAM_BLUE ) == 3))
		{	
			sprintf( text, "* Teams are full" );
			UTIL_SayText( text, pPlayer );
			return FALSE;
		}
		else if((FStrEq(gamemode.string, "4on4") && PlayersOnTeam( TEAM_RED ) == 4 && PlayersOnTeam( TEAM_BLUE ) == 4))
		{	
			sprintf( text, "* Teams are full" );
			UTIL_SayText( text, pPlayer );
			return FALSE;
		}

	}

	
	if ( m_VoiceGameMgr.ClientCommand( pPlayer, pcmd ) )
	{
		return TRUE;
	}
	
	return FALSE;
}

//=========================================================
// SetPlayerColor
//=========================================================

void CMatch :: SetPlayerColor( CBasePlayer *pPlayer, int nColor )
{
	//
	// If the color has changed
	//

	if (pPlayer ->m_nColor != nColor || nColor == -1)
	{
		//
		// Save the new color
		//

		if (nColor != -1) pPlayer ->m_nColor = nColor;

		//
		// Generate the color string
		//

		char szText [32];
		sprintf (szText, "%d", pPlayer ->m_nColor);

		//
		// Set the colors
		//

		int clientIndex = pPlayer->entindex ();
		g_engfuncs.pfnSetClientKeyValue( clientIndex, 
			g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
			"topcolor", szText );
		g_engfuncs.pfnSetClientKeyValue( clientIndex, 
			g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
			"bottomcolor", szText );
	}
}








//=========================================================
// ClientUserInfoChanged
//=========================================================
void CMatch::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
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

	//
	// Prevent skin/color/model changes
	//

	/*psz = g_engfuncs.pfnInfoKeyValue( infobuffer, "model" );
	if (stricmp ( psz, pPlayer ->m_szModelName)) 
	{
		g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), 
			g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), 
			"model", pPlayer ->m_szModelName );
//		ClientPrint ( pPlayer->pev, HUD_PRINTCONSOLE, 
//			UTIL_VarArgs("* Not allowed to change models in this game!\n"));
		return;
	}*/

	//
	// Prevent topcolor changes
	//

	psz = g_engfuncs.pfnInfoKeyValue( infobuffer, "topcolor" );
	if (atol (psz) != pPlayer ->m_nColor)
	{
		SetPlayerColor (pPlayer, -1);
//		ClientPrint ( pPlayer->pev, HUD_PRINTCONSOLE, 
//			UTIL_VarArgs("* Not allowed to change top or bottom color in this game!\n"));
	}

	//
	// Prevent bottomcolor changes
	//

	psz = g_engfuncs.pfnInfoKeyValue( infobuffer, "bottomcolor" );
	if (atol (psz) != pPlayer ->m_nColor)
	{
		SetPlayerColor (pPlayer, -1);
//		ClientPrint ( pPlayer->pev, HUD_PRINTCONSOLE, 
//			UTIL_VarArgs("* Not allowed to change top or bottom color in this game!\n"));
	}












	 RecountTeams();
}

void CMatch::JoinTeam(CBasePlayer *pPlayer, const char *pTeamName)
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
    if ( !stricmp( TEAM_RED, pTeamName ) )
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
//		ResumePlayer(pPlayer);
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
/*int CountPlayers( void )
{
	int	num = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt )
		{
			num = num + 1;
		}
	}

	return num;
}
*/

BOOL CMatch::TeamCount()
{
	int i;
	int minPlayers = maxclients.value;
	int teamCount[ MAX_TEAMS ];
	char *pTeamName = NULL;

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
/*	for ( i = 0; i < num_teams; i++ )
	{
		if ( teamCount[i] < minPlayers )
		{
			minPlayers = teamCount[i];
			pTeamName = team_names[i];
		}
	}*/

	//if((FStrEq(gamemode.string, "2on2")) || (FStrEq(gamemode.string, "3on3")) || (FStrEq(gamemode.string, "4on4")))
	//{
		for ( i = 0; i < num_teams; i++ )
		{
			if ( teamCount[i] = minPlayers )
			{
				return TRUE;
			}
		}
	//}
		

	return FALSE;
}

int CMatch :: PlayersOnTeam( const char *szTeamName )
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

//=========================================================
//=========================================================
void CMatch::RecountTeams( void )
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
				team_scores[tm] += plr->pev->frags;
			}
		}
	}
}

//=========================================================
// Menustuff
//========================================================= 

extern int gmsgShowMenu; // the message # for ShowMenu 

void CMatch::ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots, 
    int nDisplayTime, BOOL fNeedMore, char *pszText)
{
    MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pPlayer ->pev);
        WRITE_SHORT( bitsValidSlots);
        WRITE_CHAR( nDisplayTime );
        WRITE_BYTE( fNeedMore );
        WRITE_STRING (pszText);
    MESSAGE_END();
} 


	