/*
=====================================================
'small' gametypes go here. like LMS and Vampire
=====================================================
*/
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"gamerules.h"
#include	"osmod_gamerules.h"

/*
=====================================================
Vampire
=====================================================

void COSModVamp::AwardPoints( CBasePlayer *pVictim, CBasePlayer *pKiller, entvars_t *pInflictor )
{
	pKiller->pev->frags += IPointsForKill( pKiller, pVictim ) + ((pVictim->pev->frags > 0)? pVictim->pev->frags : 0);
	pVictim->pev->frags = 0;
}
*/
/*
=====================================================
Last Man Standing
=====================================================
hmm, this got bigger then i thought... may place this in a separate file..
*/
#define LMS_ROUND_DELAY 5
#define LMS_SPAWN_DELAY 1
COSModLMS::COSModLMS()
{
	m_Delay=2;
	m_DelayEnd=0;
}

CBasePlayer* COSModLMS::FindWinner( void )
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
}

CBasePlayer* COSModLMS::FindLivingPlayer( void )
{
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsAlive() && pEnt->IsPlayer() )
			return (CBasePlayer*)pEnt;
	}
	return NULL;
}

void COSModLMS::AwardPoints( CBasePlayer *pVictim, CBasePlayer *pKiller, entvars_t *pInflictor )
{
	return;
}

extern int CountPlayers( void );

void COSModLMS::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	int frags=pVictim->pev->frags;
	CHalfLifeMultiplay::PlayerKilled(pVictim,pKiller,pInflictor);
	pVictim->pev->frags=frags;

	if (CountPlayers()>1)
	{
		CBasePlayer* pPlayer=FindLivingPlayer();
		if (pPlayer)
			pVictim->StartObserver(pPlayer->pev->origin,pPlayer->pev->v_angle);
		else
			pVictim->StartObserver(pVictim->pev->origin,pVictim->pev->v_angle);
		//pVictim->m_iLFlags|=LF_GAMERULE_OBSERVER;
		pVictim->pev->flags|=FL_FROZEN;
	}
}

BOOL COSModLMS::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	BOOL ret=CHalfLifeMultiplay::ClientConnected(pEntity,pszName,pszAddress,szRejectReason);
	if (ret)
	{
		CBasePlayer* pPlayer=FindLivingPlayer();
		CBasePlayer* pClient=(CBasePlayer*)CBaseEntity::Instance(pEntity);
		if (pClient)
		{
			if (pPlayer)
				pClient->StartObserver(pPlayer->pev->origin,pPlayer->pev->v_angle);
			else
				pClient->StartObserver(pClient->pev->origin,pClient->pev->v_angle);
			//pClient->m_iLFlags|=LF_GAMERULE_OBSERVER;
		}
		VARS(pEntity)->flags|=FL_FROZEN;
	}
	return ret;
}

void COSModLMS::StartDelay( CBasePlayer* pWinner )
{
	m_Delay=1;
	m_DelayEnd=gpGlobals->time+LMS_ROUND_DELAY;

	if (pWinner)
	{
		//add points
		pWinner->AddPoints(1,FALSE);
		pWinner->StartObserver(pWinner->pev->origin,pWinner->pev->v_angle);
//		pWinner->m_iLFlags|=LF_GAMERULE_OBSERVER;
		pWinner->pev->flags|=FL_FROZEN;

		//show msg
		char msg[255];
		sprintf(msg,"%s won the round",STRING(pWinner->pev->netname));
		UTIL_ClientPrintAll( HUD_PRINTTALK, msg );

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
	else
		UTIL_ClientPrintAll( HUD_PRINTTALK, "The round was drawn" );
		

}

void COSModLMS::Think( void )
{
	if (m_DelayEnd==0)
		m_DelayEnd=gpGlobals->time+LMS_SPAWN_DELAY;
	if (m_Delay==1)
		DelayThink();
	else if (m_Delay==0)
		NormalThink();
	else if (m_Delay==2&&gpGlobals->time>=m_DelayEnd)
		m_Delay=0;
}

void COSModLMS::NormalThink( void )
{
	FindWinner();
}

void COSModLMS::DelayThink( void )
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
				if ( pEnt->IsObserver()) 
				{
					//pEnt->m_iLFlags&=~LF_GAMERULE_OBSERVER;
					pEnt->pev->flags&=~FL_FROZEN;
					pEnt->StopObserver();
					pEnt->Spawn();
				}
			}
		}
		UTIL_ClientPrintAll( HUD_PRINTTALK, "The round has begun" );
		m_DelayEnd=gpGlobals->time+LMS_SPAWN_DELAY;
		m_Delay=2;
	}
}