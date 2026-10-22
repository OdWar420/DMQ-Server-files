
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
#include    "dme_arena.h"
#include	"hltv.h"
#include	"teamplay_gamerules.h"

extern int gmsgTeamInfo;
extern int gmsgScoreInfo;
extern int gmsgSpectator;
extern int gmsgServerName;
extern int gmsgGameMode;


extern int g_bPlayLongjumpSound; // define this in items.cpp, its explained later in this file (search for it below)


CGameModeDuel::CGameModeDuel()
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
	m_bDeux = false;
	m_bAllowRestrictedSpawning = false;	
	m_pWinner = NULL;
	m_pLoser = NULL;

	m_nBonus = DUEL_BONUS_NONE;

	

}

CGameModeDuel::~CGameModeDuel()
{
    ClearList();
}


void CGameModeDuel::UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_STRING( "DMQ2 : Mode Arena" );  
	MESSAGE_END();

	
}




void CGameModeDuel::PlayerThink( CBasePlayer *pPlayer )
{

	if(pPlayer->m_bUnSpawn && !pPlayer->m_isSpectator )
	{
		{
			// this is a new player.h int variable, all you have to do is add this in player.h bool m_bUnSpawn (it gets automaticly set to 0 (false) by default)
			pPlayer->m_bUnSpawn = false;
			pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles); // cant spawn homie
		}
	}

	CHalfLifeMultiplay::PlayerThink(pPlayer);
					
}

BOOL CGameModeDuel::FPlayerCanRespawn(CBasePlayer *pPlayer) 
{
	return TRUE;
}

void CGameModeDuel::ThinkLimit()
{
			
	CHalfLifeMultiplay::Think();
}

void CGameModeDuel::Think()
{
	if(m_bStartSequence)
	{
		if(m_dStartMessage.TimeUp())
		{
			m_dStartMessage.ClearTime();
			VersesMessage();

			CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));

			if(m_nNumber = 1 )
			{
			EMIT_SOUND( ENT(pPlayer->pev), CHAN_VOICE, "misc/321.wav", 1, ATTN_NORM );
			}
		}

		if(m_dStartRound.TimeUp())
		{
			m_dStartRound.AddTime(1);// add 1 second for the next count
				
			{
				m_nNumber++;

				if(m_nNumber > MAX_SPRITE_NUMBER)
				{
					CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
					STOP_SOUND( ENT(pPlayer->pev), CHAN_VOICE, "misc/321.wav" );
					RemoveAllEntites();
					ToggleSpawningRestriction(); // turn on selected spawning
					SpawnValidPlayers(); // spawn the players up for this round
 					ToggleSpawningRestriction();  // turn off selected spawning, no one can spawn now
					RespawnAllEntites();	
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
			EndRound();
		}

	}
	
							CBasePlayer* pPlayer2=NULL;
                        for(int i=0; i < m_lEntity.size(); i++) 
						{
							pPlayer2 = GetClassPtr((CBasePlayer *)&(m_lEntity[i]->v));
								if(gpGlobals->time-pPlayer2->pev->fuser2>0.5)
								{
 									pPlayer2->AddPoints(0,true);
									pPlayer2->pev->fuser2=gpGlobals->time;
								}
						}

	ThinkLimit();	
}

void CGameModeDuel::TimeIsUp()
{
	if(MinimumClientsConnected())
	{

		CBasePlayer* pPlayer0 = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
		CBasePlayer* pPlayer1 = GetClassPtr((CBasePlayer *)&(m_lEntity[1]->v));

		if(pPlayer0->pev->health > pPlayer1->pev->health)
		{
			m_nResult = RESULT_TIMEUP;
			m_pLoser = pPlayer1->edict();
			m_pWinner = pPlayer0->edict();

			m_dRoundTimer.ClearTime();
			m_dEndMessage.AddTime(END_DELAY);
			MovePlayer(m_pLoser);
		}
		else if(pPlayer0->pev->health < pPlayer1->pev->health)
		{
			m_nResult = RESULT_TIMEUP;
			m_pLoser = pPlayer0->edict();
			m_pWinner = pPlayer1->edict();
			m_dRoundTimer.ClearTime();
			m_dEndMessage.AddTime(END_DELAY);
			MovePlayer(m_pLoser);
		
		}
		else // same health
		{
			if(pPlayer0->pev->armorvalue > pPlayer1->pev->armorvalue)
			{
				m_nResult = RESULT_TIMEUP;
				m_pLoser = pPlayer1->edict();
				m_pWinner = pPlayer0->edict();
				m_dRoundTimer.ClearTime();
				m_dEndMessage.AddTime(END_DELAY);
				MovePlayer(m_pLoser);
			}
			else if(pPlayer0->pev->armorvalue < pPlayer1->pev->armorvalue)
			{
				m_nResult = RESULT_TIMEUP;
				m_pLoser = pPlayer0->edict();
				m_pWinner = pPlayer1->edict();

				m_dRoundTimer.ClearTime();
				m_dEndMessage.AddTime(END_DELAY);
				MovePlayer(m_pLoser);
				//pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
			
			}
			else // same health same hev, no win!
			{
				// not really a winner, they are both losers, but we will use thsee pointers at our convience for later
				m_pLoser = pPlayer1->edict();
				m_pWinner = pPlayer0->edict();
				m_nResult = RESULT_DRAW;

				//CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
				//pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
				edict_t* pTemp = m_lEntity[0];
				m_lEntity.remove(m_lEntity[0]);
				m_lEntity.push_back(pTemp);
				pTemp = m_lEntity[0];
				//pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
				//pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
				m_lEntity.remove(m_lEntity[0]);
				m_lEntity.push_back(pTemp);


				m_dRoundTimer.ClearTime();
				AnounceOutcome();
				m_dEndMessage.ClearTime();
				EndRound();
			}
		}
	}
	
}

void CGameModeDuel::StartRoundTime()
{
	m_nRoundTime = roundtime.value*60;
	
	if(m_nRoundTime >=1000)
		m_nRoundTime=999;
	else if(m_nRoundTime<=0)
		m_nRoundTime = 0;
	m_dRoundTimer.AddTime(m_nRoundTime);

	
}


// called from dtor
void CGameModeDuel::ClearList(void)
{
    m_lEntity.clear();
}

// add at bottom of list when a client connects
void CGameModeDuel::AddPlayer(edict_t* pEntity)
{
	m_lEntity.push_back(pEntity);
    UpdateRound();
}

bool CGameModeDuel::LookUpPlayer(edict_t* pEntity)
{
	for(int i = 0; i < m_lEntity.size(); i++)
	{
		if(pEntity == m_lEntity[i])
			return true;
	}
	return false;
}

bool CGameModeDuel::IsInRound(edict_t* pEntity)
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


void CGameModeDuel::MovePlayer(edict_t* pEntity)
{
	m_lEntity.remove(pEntity);
	m_lEntity.push_back(pEntity);
	UpdateRound();
	return;
}
// remove from list when client disconnects
void CGameModeDuel::DropPlayer(edict_t* pEntity)
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

bool CGameModeDuel::MinimumClientsConnected()
{
	return m_lEntity.size() >=2 ? true : false;
}

int CGameModeDuel::MinimumClients()
{
	return 2;
}


void CGameModeDuel::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{

	DeathNotice( pVictim, pKiller, pInflictor );

	

	pVictim->m_iDeaths += 1;

	CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
		if ( pPlayer->IsObserver() && m_hObserverTarget == pVictim )
			pPlayer->Observer_FindNextPlayer(!!atoi( CMD_ARGV(1) ));
	
	

	FireTargets( "game_playerdie", pVictim, pVictim, USE_TOGGLE, 0 );
	CBasePlayer *peKiller = NULL;
	CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );
	if ( ktmp && (ktmp->Classify() == CLASS_PLAYER) )
		peKiller = (CBasePlayer*)ktmp;

	if ( pVictim->pev == pKiller )  
	{  
		if(pVictim->edict() == m_pWinner) // killed self after he won before the round ended
		{
			m_pLoser = NULL;
			m_pWinner = NULL;
			m_nResult = RESULT_TIE;
		}
		else // killed self
		{
			m_pWinner = NULL;
			m_pLoser = pVictim->edict();
			m_nResult = RESULT_SUICIDE;
		}
		pKiller->frags -= 1;
	}
	else if ( ktmp && ktmp->IsPlayer() )
	{

		if(FStrEq(STRING(pVictim->pev->netname), ""))// deals with ghosts
		{
			return;
		}
	

		if(m_pWinner) // killed eachother
		{
			m_pLoser = NULL;
			m_pWinner = NULL;
			m_nResult = RESULT_TIE;
		}
		else
		{
			m_nResult = RESULT_DEFEAT;
			m_pLoser = pVictim->edict();
			m_pWinner = peKiller->edict();
		}
		pKiller->frags += IPointsForKill( peKiller, pVictim );
		FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
	}
	else
	{   //killed by the world
		if(pVictim->edict() == m_pWinner)
		{
			m_pLoser = pVictim->edict();;
			m_pWinner = NULL;
			m_nResult = RESULT_TIE;
		}
		else
		{
			m_pLoser = pVictim->edict();
			m_pWinner = NULL;
			m_nResult = RESULT_SUICIDE;
		}	
		pKiller->frags -= 1;
	}
	if(m_pWinner)
	{
		
		entvars_t *pevWinner = &m_pWinner->v;
		
		if((pevWinner->health >= 250) && (pevWinner->armorvalue >= 200))
			m_nBonus = DUEL_BONUS_FLAWLESS;

		else if(pevWinner->health > 150) // no lost of hev (only fall damage occured)
			m_nBonus = DUEL_BONUS_OWNED;

		
		
	}

	// update the scores
	// killed scores
	MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( ENTINDEX(pVictim->edict()) );
		WRITE_SHORT( pVictim->pev->frags );
		WRITE_SHORT( pVictim->m_iDeaths );
		WRITE_SHORT( 0 );
		//WRITE_SHORT( pVictim->pev->team );
		WRITE_SHORT( GetTeamIndex( pVictim->m_szTeamName ) + 1 );
		WRITE_SHORT( pVictim->m_iWins );
		WRITE_SHORT( 0 );
		WRITE_BYTE( 0);
	MESSAGE_END();

	// killers score, if it's a player
	CBaseEntity *ep = CBaseEntity::Instance( pKiller );
	if ( ep && ep->Classify() == CLASS_PLAYER )
	{
		CBasePlayer *PK = (CBasePlayer*)ep;

		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(PK->edict()) );
			WRITE_SHORT( PK->pev->frags );
			WRITE_SHORT( PK->m_iDeaths );
			WRITE_SHORT( 0 );
			//WRITE_SHORT( PK->pev->team );
			WRITE_SHORT( GetTeamIndex( PK->m_szTeamName) + 1 );
		MESSAGE_END();

		// let the killer paint another decal as soon as he'd like.
		PK->m_flNextDecalTime = gpGlobals->time;
	}
/*	
#ifndef HLDEMO_BUILD
	if ( pVictim->HasNamedPlayerItem("weapon_satchel") )
	{
		DeactivateSatchels( pVictim );
	}
#endif
*/
	m_dRoundTimer.ClearTime();
	

	//MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
	//WRITE_SHORT (-1); // -1 freezes the current timer 
	//MESSAGE_END();

	
	m_dEndMessage.AddTime(END_DELAY);
	MovePlayer(pVictim->edict());
	// wait 2 seconds incase the winner dies too, otherwise we anouce the winner
}


BOOL CGameModeDuel::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	return CHalfLifeMultiplay::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
}

void CGameModeDuel :: ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
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

	// notify everyone's HUD of the team change

	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( clientIndex );
		WRITE_STRING( pPlayer->m_szTeamName );
	MESSAGE_END();

	/*MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( clientIndex );
		WRITE_SHORT( pPlayer->pev->frags );
		WRITE_SHORT( pPlayer->m_iDeaths );
		WRITE_SHORT( 0 );
		WRITE_SHORT( g_pGameRules->GetTeamIndex( pPlayer->m_szTeamName ) + 1 );
	MESSAGE_END();*/
}


extern int gmsgTeamNames;

void CGameModeDuel::InitHUD( CBasePlayer *pPlayer )
{
	
	pPlayer->m_iNextTeam = 0;		// unknown next team

	// populate the TFC team VGUI

	MESSAGE_BEGIN( MSG_ONE, gmsgTeamNames, NULL, pPlayer->edict() );
			WRITE_BYTE( 2 );
		WRITE_STRING( TEAM1 );
		MESSAGE_END();
	
	// start them off on a random team

	

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


void CGameModeDuel::ClientDisconnected(edict_t* pEntity)
{
    
	DropPlayer(pEntity);


	if(MinimumClientsConnected()) // 1 guy left
		m_bJustConnected = true; // next guy gets and extra 10 seconds to connect b4 round starts

	
	
	
	CHalfLifeMultiplay::ClientDisconnected(pEntity);
}

void CGameModeDuel::StartRound()
{
	m_dStartMessage.AddTime(ROUND_DELAY);
	m_dStartRound.AddTime(ROUND_DELAY);
	m_bAllowRestrictedSpawning	= false;
	m_bStartSequence			= true;
	m_bInProgress				= true;
	m_nNumber					= 0;
	m_pWinner					= NULL;
	m_pLoser					= NULL;
	m_nResult					= RESULT_NONE;
}

void CGameModeDuel::EndRound()
{
	m_bInProgress		= false;
	m_nNumber			= 0;
	m_bStartSequence	= false;
	m_nRoundTime		= 0;
	UpdateRound();
}
void CGameModeDuel::UpdateRound()
{
	if(m_bInProgress)
	{
		if(IsOver())
			EndRound();
	}
	else if(MinimumClientsConnected())
	{
		if(m_dEndMessage.GetTimeLeft()!=0)
			return;
		StartRound();

		if(m_bJustConnected)
		{	
			// this is not used, ytou can add more time between rounds ir there are 2 ppl in the server and one i still connecting, i never had this problem yet tho
			m_bJustConnected = false;
			//m_dStartMessage.AddTime(10);
			//m_dStartRound.AddTime(10);
		}
	}
}


float CGameModeDuel :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;//now!
}




void CGameModeDuel::PlayerSpawn( CBasePlayer *pPlayer )
{
	BOOL		addDefault;
	CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<IT_AXE);
	

	///////////////////////
	///NEW PLAYER CHECK ///
	///////////////////////

	if(!LookUpPlayer(pPlayer->edict()))
	{
		AddPlayer(pPlayer->edict());
		pPlayer->m_bUnSpawn = true;
		return;
	}

	

	///////////////////////
	///START SPAWN CHECK///
	///////////////////////
	
	bool UnSpawn = true;

	if(m_bAllowRestrictedSpawning)
	{	
		if(MinimumClientsConnected())
		{				
			if(IsInRound(pPlayer->edict()))
				UnSpawn = false;
		}
	}
	if(UnSpawn)
	{
		pPlayer->m_bUnSpawn = true;
		return;
	}

	addDefault = TRUE;
	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}

	if(addDefault)
	{	
		
		pPlayer->StopObserver();
		ChangePlayerTeam( pPlayer, TEAM1, FALSE, FALSE );
		
	}

	// run CHalfLifeMultiplay's PlayerSpawn

	CHalfLifeMultiplay::PlayerSpawn( pPlayer );
}


void CGameModeDuel::VersesMessage()
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


void CGameModeDuel::AnounceOutcome()
{
	char buf[128];
	*buf=NULL;
	CBasePlayer* pWinner;
	CBasePlayer* pLoser;
	//if(!MinimumClientsConnected())
	//{
	//	UpdateRound();
	//	return;
	//}

	

	switch(m_nResult)
	{

	case RESULT_NONE:

		break;

	case RESULT_DRAW:

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
			//WRITE_SHORT( pWinner->pev->team );
			WRITE_SHORT( GetTeamIndex( pWinner->m_szTeamName) + 1 );
			MESSAGE_END();


			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pWinner->edict()) );
			WRITE_SHORT( pLoser->pev->frags );
			WRITE_SHORT( pLoser->m_iDeaths );
			WRITE_SHORT( 0 );
			//WRITE_SHORT( pLoser->pev->team );
			WRITE_SHORT( GetTeamIndex( pLoser->m_szTeamName) + 1 );
			MESSAGE_END();
		}
		strcpy(buf, "Draw");
		break;

	case RESULT_TIE:
		
		strcpy(buf, "Tie");
		break;
	case RESULT_TIMEUP:
	
		if(MinimumClientsConnected())
		{
			
			strncpy(buf, "Time up\n", 128);
			strncat(buf, STRING(m_pWinner->v.netname), 128);
			strncat(buf," has forced the win over ", 128);
			strncat(buf, STRING(m_pLoser->v.netname),128 );;
			m_pWinner->v.flags |= FL_GODMODE;
			m_pLoser->v.flags |= FL_GODMODE;
			
			CBasePlayer* pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
			pWinner->m_iWins+=1;

			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pWinner->edict()) );
			WRITE_SHORT( pWinner->pev->frags );
			WRITE_SHORT( pWinner->m_iDeaths );
			WRITE_SHORT( 0 );
			//WRITE_SHORT( pWinner->pev->team );
			WRITE_SHORT( GetTeamIndex( pWinner->m_szTeamName) + 1 );
			MESSAGE_END();

		}
		break;
		

	case RESULT_SUICIDE://///////////////////

		if(MinimumClientsConnected())
		{
			m_lEntity[0]->v.takedamage = DAMAGE_NO;
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
			//WRITE_SHORT( pWinner->pev->team );
			WRITE_SHORT( GetTeamIndex( pWinner->m_szTeamName) + 1 );
			MESSAGE_END();

		}
		break;

	case RESULT_FORFEIT://///////////////////

		pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
		pLoser->m_iWins-=1;

		strncpy(buf, STRING(m_pLoser->v.netname), 128);
		strncat(buf," has forfeited", 128);

		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( ENTINDEX(pLoser->edict()) );
		WRITE_SHORT( pLoser->pev->frags );
		WRITE_SHORT( pLoser->m_iDeaths );
		WRITE_SHORT( 0 );
		//WRITE_SHORT( pLoser->pev->team );
		WRITE_SHORT( GetTeamIndex( pLoser->m_szTeamName) + 1 );
		MESSAGE_END();

		break;

	case RESULT_DEFEAT://////////////////////
		{

		

		if(MinimumClientsConnected())
		{

			RemoveAllEntites();
			sprintf( buf, "%s WINS THE ROUND WITH %i HEALTH AND %i ARMOR TO SPARE", STRING( m_pWinner->v.netname ), (signed long)m_pWinner->v.health, (signed long)m_pWinner->v.armorvalue ); 
				
			//strncpy(buf, STRING(m_pWinner->v.netname), 128);
			//strncat(buf," WINS THE ROUND\n ", 128);
		//	strncat(buf, STRING(m_pLoser->v.netname),128 );
		
			
		
		
			// add a win to this player
			pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
			pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
			//pWinner->pev->takedamage = DAMAGE_NO;
			//m_pWinner->v.takedamage = DAMAGE_NO;

			

			
			
			
			
			if(m_nBonus == DUEL_BONUS_FLAWLESS)
			{
				if((m_pWinner->v.health >= 250) && (m_pWinner->v.armorvalue >= 200))
				{

					EMIT_SOUND( ENT(pWinner->pev), CHAN_VOICE, "misc/flawless.wav", 1, ATTN_NORM );
					
				}
			}
			else if(m_nBonus == DUEL_BONUS_OWNED)
			{
				if(m_pWinner->v.health > 150)
				{
					EMIT_SOUND( ENT(pWinner->pev), CHAN_VOICE, "misc/excelent.wav", 1, ATTN_NORM );
					
				}
			}
			else if(m_nBonus == DUEL_BONUS_IMMORTAL)
			{
				CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
				if(pPlayer->m_iDeaths == 0)
				{
					// This will draw a sprite saying "immortal" THIS IS FOR TOURENY MODE ONLY
					//MESSAGE_BEGIN( MSG_ALL, gmsgBonus, NULL);
					//WRITE_BYTE(2);
					//MESSAGE_END();
				}
			}

			

			
			

			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pWinner->edict()) );
			WRITE_SHORT( pWinner->pev->frags );
			WRITE_SHORT( pWinner->m_iDeaths );
			WRITE_SHORT( 0 );
			//WRITE_SHORT( pWinner->pev->team );
			WRITE_SHORT( GetTeamIndex( pWinner->m_szTeamName) + 1 );
			MESSAGE_END();


			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pLoser->edict()) );
			WRITE_SHORT( pLoser->pev->frags );
			WRITE_SHORT( pLoser->m_iDeaths );
			WRITE_SHORT( 0 );
			//WRITE_SHORT( pLoser->pev->team );
			WRITE_SHORT( GetTeamIndex( pLoser->m_szTeamName) + 1 );
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

void CGameModeDuel::ToggleSpawningRestriction()
{
// This is used to turn off the longjump sound, check out items.cpp and add an if stamtent right before the server plays the sound, (pretty simple hack)
//	g_bPlayLongjumpSound =! g_bPlayLongjumpSound;
	m_bAllowRestrictedSpawning =! m_bAllowRestrictedSpawning;
}

void CGameModeDuel::SpawnValidPlayers()
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

// this is a new function you will have to add to GameRules
// BOOL CGameModeDuel::CanStartObserver(CBasePlayer *pPlayer)
BOOL CGameModeDuel::CanStartObserver(CBasePlayer *pPlayer)
{
	if(!pPlayer->m_isSpectator)
	{
		if(IsInRound(pPlayer->edict()))
		{
			if(!m_bStartSequence)
			{
				// stops the timer is an active player spectates, he automaticly foreits
				//MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
				//WRITE_SHORT (-1); 
				//MESSAGE_END();

				m_nResult = RESULT_FORFEIT;
				m_pWinner = NULL;
				m_pLoser = pPlayer->edict();
				m_dEndMessage.AddTime(END_DELAY);
				MovePlayer(pPlayer->edict());
				m_lEntity[0]->v.takedamage = DAMAGE_NO; // immediatly stop damage
			}
		}
		return TRUE;
	}
	return FALSE;
}

// prevents the can from droping on a deal player
BOOL CGameModeDuel::CanStopObserver(CBasePlayer *pPlayer)
{
	return FALSE;
}

// prevents the can from droping on a deal player
int CGameModeDuel::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

// prevents the can from droping on a deal player
int CGameModeDuel::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

// checks to see if the round is ended
bool CGameModeDuel::IsOver()
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


// this deals with adding and removing the player from the active queue, if a client types "break" 
// he will no longer spawn or play in the round
void CGameModeDuel::RemovePlayer(CBasePlayer* pPlayer)
{

	pPlayer->m_bIsPlaying = false;

	if(CanStartObserver(pPlayer))
	{
		pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
	}

	DropPlayer(pPlayer->edict());

	// This is a sprite showing if a player is connected or removed from a match, kinda like in ag if you type "remove" or some shit
	//MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
	//WRITE_BYTE( 1 );
	//MESSAGE_END();
}



// same as above, self explanitory
void CGameModeDuel::ResumePlayer(CBasePlayer* pPlayer)
{
	pPlayer->m_bIsPlaying = true;
	if(!LookUpPlayer(pPlayer->edict()))
		AddPlayer(pPlayer->edict());

	//MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
	//WRITE_BYTE( 3 );
	//MESSAGE_END();
}
extern CVoiceGameMgr m_VoiceGameMgr;

BOOL CGameModeDuel :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	char text[1024];
	
	if ( FStrEq( pcmd, "break" ) )
	{	
		
		RemovePlayer(pPlayer);
		sprintf(text, "* %s IS NOT READY\n", STRING( pPlayer->pev->netname ));
		UTIL_SayTextAll( text, pPlayer );
		return TRUE;			
	}
	
	if ( FStrEq( pcmd, "ready" ) )
	{	

//		pPlayer->m_IsReady = true;
	    ResumePlayer(pPlayer);
		sprintf(text, "* %s IS READY\n", STRING( pPlayer->pev->netname ));
		UTIL_SayTextAll( text, pPlayer );
		return TRUE;
	}

	
   return	CHalfLifeMultiplay::ClientCommand( pPlayer, pcmd );
}

	