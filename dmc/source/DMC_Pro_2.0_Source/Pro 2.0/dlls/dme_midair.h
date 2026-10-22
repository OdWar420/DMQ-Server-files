#ifndef _MODE_MIDAIR_H
#define _MODE_MIDAIR_H

#include    "dme_utility.h"
#include "effects.h"

#define DUEL_BONUS_NONE 0
#define DUEL_BONUS_OWNED 1
#define DUEL_BONUS_FLAWLESS 2
#define DUEL_BONUS_IMMORTAL 3
#define ROUND_DELAY 6
#define END_DELAY 2
#define TEAM3 "IN ROUND"


enum 
{
	RESULT_NONE_M=0,
	RESULT_TIE_M,
	RESULT_DRAW_M,
	RESULT_FORFEIT_M,
	RESULT_SUICIDE_M,
	RESULT_DEFEAT_M,
	RESULT_TIMEUP_M,
};

class CGameModeRoundMidair : public CHalfLifeMultiplay
{

public:

	// round logistics 
	virtual void StartRound()=0;
	virtual void StartRoundTime()=0;
	virtual void EndRound()=0;
	virtual void VersesMessage()=0;
	virtual void AnounceOutcome()=0;

	
	// spawn control
	virtual void SpawnValidPlayers()=0;
	virtual void ToggleSpawningRestriction()=0;

	// player control
	virtual int  MinimumClients()=0;
	virtual bool MinimumClientsConnected()=0;
	virtual bool IsInRound(edict_t* pEntity)=0;

	// round control
	virtual void UpdateRound()=0;
	virtual bool IsOver()=0;
	virtual void TimeIsUp()=0;
};

class CGameModeMidair : public CGameModeRoundMidair
{
public:


    CGameModeMidair();
	virtual ~CGameModeMidair();
	BOOL CanStartObserver(CBasePlayer *pPlayer);
	BOOL CanStopObserver(CBasePlayer *pPlayer);
	virtual void Think();	
	virtual void PlayerThink( CBasePlayer *pPlayer );
    virtual void ClientDisconnected(edict_t* pEntity);
    virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void InitHUD( CBasePlayer *pPlayer );
	virtual BOOL FPlayerCanRespawn( CBasePlayer *pPlayer );
	float FlPlayerSpawnTime( CBasePlayer *pPlayer );
    virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
    virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );   
	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	int DeadPlayerWeapons( CBasePlayer *pPlayer );
	int DeadPlayerAmmo( CBasePlayer *pPlayer );
	virtual const char *GetGameDescription( void ) { return "Pro : Mode Midair"; }  // this is the game name that gets seen in the server browser
	virtual void RemovePlayer(CBasePlayer* pPlayer);
	virtual void IdlePlayer(CBasePlayer* pPlayer);
	void ResumePlayer(CBasePlayer* pPlayer);
	void RecountTeams();	
	EHANDLE	m_hObserverTarget;
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	CVoiceGameMgr m_VoiceGameMgr;

protected:

	virtual void ThinkLimit();
	virtual void TimeIsUp();
	virtual void StartRoundTime();
	virtual void AnounceOutcome();
	virtual void SpawnValidPlayers();
	virtual void StartRound();
	virtual void EndRound();
	virtual void ToggleSpawningRestriction();
	virtual void VersesMessage();
	virtual int  MinimumClients();
	virtual bool MinimumClientsConnected();
	virtual bool IsInRound(edict_t* pEntity);
	virtual void UpdateRound();
	virtual bool IsOver();
	BOOL m_DisableDeathMessages;
	BOOL m_DisableDeathPenalty;
	BOOL m_teamLimit;
	int m_nResult;
	int m_nBonus;
	int m_nRoundTime;
	CEntityList m_lEntity;
	bool m_bDeux;
	edict_t* m_pWinner;
	edict_t* m_pLoser;
	
	bool m_bPlayedQuadSound;
    bool m_bInProgress;
	bool m_bJustConnected;
	bool m_bAllowRestrictedSpawning;
	bool m_bStartSequence;
	int  m_nNumber;

	CDelayTime m_dStartRound;
	CDelayTime m_dStartMessage;
	CDelayTime m_dEndMessage;
	CDelayTime m_dRoundTimer;
	

	hudtextparms_t m_hMessage;
	
	bool LookUpPlayer(edict_t* pEntity);
    void AddPlayer(edict_t* pEntity);
    void DropPlayer(edict_t* pEntity);
	void MovePlayer(edict_t* pEntity);
    void ClearList();
    void RefreshList();
};

#endif