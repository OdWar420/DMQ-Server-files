#ifndef _MODE_ARENA_H
#define _MODE_ARENA_H

#include "dme_utility.h"
#include "effects.h"

#define DUEL_BONUS_NONE 0
#define DUEL_BONUS_OWNED 1
#define DUEL_BONUS_FLAWLESS 2
#define DUEL_BONUS_IMMORTAL 3
#define ROUND_DELAY 6
#define END_DELAY 2
#define TEAM1 "IN ARENA"

enum 
{
	RESULT_NONE2=0,
	RESULT_TIE2,
	RESULT_DRAW2,
	RESULT_FORFEIT2,
	RESULT_SUICIDE2,
	RESULT_DEFEAT2,
	RESULT_TIMEUP2,
};

class CRumble : public CHalfLifeMultiplay
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

class CGameModeRumble : public CRumble
{
public:


    CGameModeRumble();
	virtual ~CGameModeRumble();
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
	virtual const char *GetGameDescription( void ) { return "DMQ2 : Mode Royal Rumble"; }  // this is the game name that gets seen in the server browser
	virtual void RemovePlayer(CBasePlayer* pPlayer);
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
	 
    bool m_bInProgress;
	bool m_bJustConnected;
	bool m_bAllowRestrictedSpawning;
	bool m_bStartSequence;
	int  m_nNumber;
	int aliveplayers;

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