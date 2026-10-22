#ifndef _MODE_MATCH_H
#define _MODE_MATCH_H

#include "dme_utility.h"
#include "effects.h"

#define DUEL_BONUS_NONE 0
#define DUEL_BONUS_OWNED 1
#define DUEL_BONUS_FLAWLESS 2
#define DUEL_BONUS_IMMORTAL 3
#define ROUND_DELAY_M 10
#define END_DELAY 2
#define TEAM2 "MATCH"
#define TEAM_WARMUP "PREWAR"
#define TEAM_RED "Red"
#define TEAM_BLUE "Blue"
#define TEAM_SPECTATE "SPECTATE"
#define MAX_TEAMNAME_LENGTH	16
#define MAX_TEAMS			32


#define TEAMPLAY_TEAMLISTLENGTH		MAX_TEAMS*MAX_TEAMNAME_LENGTH
enum 
{
	RESULT_NONE_D=0,
	RESULT_TIE_D,
	RESULT_DRAW_D,
	RESULT_FORFEIT_D,
	RESULT_SUICIDE_D,
	RESULT_DEFEAT_D,
	RESULT_TIMEUP_D,
};

class CRoundsRules : public CHalfLifeMultiplay
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

class CMatch : public CRoundsRules
{
public:


    CMatch();
	virtual ~CMatch();
	BOOL CanStartObserver(CBasePlayer *pPlayer);
	BOOL CanStopObserver(CBasePlayer *pPlayer);
	virtual void Think();	
	virtual void PlayerThink( CBasePlayer *pPlayer );
    virtual void ClientDisconnected(edict_t* pEntity);
    virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void InitHUD( CBasePlayer *pPlayer );
	virtual BOOL FPlayerCanRespawn( CBasePlayer *pPlayer );
	float FlPlayerSpawnTime( CBasePlayer *pPlayer );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
    virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
    virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );   
	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	int DeadPlayerWeapons( CBasePlayer *pPlayer );
	int DeadPlayerAmmo( CBasePlayer *pPlayer );
	virtual const char *GetGameDescription( void ) { return "Pro Match"; }  // this is the game name that gets seen in the server browser
	virtual void RemovePlayer(CBasePlayer* pPlayer);
	virtual void IdlePlayer(CBasePlayer* pPlayer);
	void ResumePlayer(CBasePlayer* pPlayer);
	virtual void JoinTeam( CBasePlayer *pPlayer, const char *pTeamName );
	virtual BOOL IsTeamplay( void );
	void RecountTeams();	
	EHANDLE	m_hObserverTarget;
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	CVoiceGameMgr m_VoiceGameMgr;
//	void BeginVote (CBasePlayer *pPlayer, _Vote nVote, const char *pszVoteText);
//	void CheckVote (void);
//	void MenuCastVote( CBasePlayer *pPlayer );
	void ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots, 
        int nDisplayTime, BOOL fNeedMore, char *pszText);
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual int PlayersOnTeam( const char *szTeamName );
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );

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
	virtual void ResetRound();
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
	bool m_bStartTimer;
	bool m_bJustConnected;
	bool m_bAllowRestrictedSpawning;
	bool m_bStartSequence;
	int  m_nNumber;
	float fRoundTimerStart;
	bool bNewMap;
	float fLastTime;
	float fUserTimerStart;
	float m_fExecCheckTime;
	float m_fExecCheckTimeB;
	bool m_IsReady;
	int  m_iNextRoundTime; 
	int m_afButtonPressed;

	CDelayTime m_dStartRound;
	CDelayTime m_dStartMessage;
	CDelayTime m_dEndMessage;
	CDelayTime m_dRoundTimer;
	

	hudtextparms_t m_hMessage;
	
	bool LookUpPlayer(edict_t* pEntity);
    void AddPlayer(edict_t* pEntity);
    void DropPlayer(edict_t* pEntity);
	char m_szTeamList[TEAMPLAY_TEAMLISTLENGTH];
	void MovePlayer(edict_t* pEntity);
    void ClearList();
    void RefreshList();
	
	virtual BOOL TeamCount();
	void SetPlayerColor (CBasePlayer *pPlayer, int nColor);

	
//	void BeginVote (CBasePlayer *pPlayer, _Vote nVote, const char *pszVoteText);
//	void CheckVote (void);

};

#endif