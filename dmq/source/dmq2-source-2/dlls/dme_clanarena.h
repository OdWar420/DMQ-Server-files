
//
// teamplay_gamerules.h
//

#include "voice_gamemgr.h"

#define MAX_TEAMNAME_LENGTH	16
#define MAX_TEAMS			32

#define TEAMPLAY_TEAMLISTLENGTH		MAX_TEAMS*MAX_TEAMNAME_LENGTH
// Red team
#define TEAM1_NAME "Red"
#define TEAM1_COLOR "Red"
// blue team
#define TEAM2_NAME "Blue" 
#define TEAM2_COLOR "Blue"
//Chip


class CClanArena : public CHalfLifeMultiplay
{
public:
	CClanArena();

//	virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	virtual BOOL IsTeamplay( void );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual BOOL ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual const char *GetGameDescription( void ) { return "DMQ2 : Mode Clan Arena"; }  // this is the game name that gets seen in the server browser
	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	//virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Think ( void );
	virtual int GetTeamIndex( const char *pTeamName );
	virtual const char *GetIndexedTeamName( int teamIndex );
	virtual BOOL IsValidTeam( const char *pTeamName );
//	const char *SetDefaultPlayerTeam( CBasePlayer *pPlayer );
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
	virtual void ClientDisconnected(edict_t* pEntity);
	//DME
	virtual void JoinTeam( CBasePlayer *pPlayer, const char *pTeamName );
	//DME
	virtual void ReJoinTeam( CBasePlayer *pPlayer, const char *pTeamName );
	
	CVoiceGameMgr	m_VoiceGameMgr;
//	virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual int PlayersOnTeam( const char *szTeamName );
	virtual void PlayerThink( CBasePlayer *pPlayer );
	
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
	//virtual void Think( void );

private:
//	CBasePlayer* FindWinner( void );
	CBasePlayer* FindLivingPlayer( void );
	void NormalThink( void );
	void DelayThink( void );
	void StartDelay( void );

	int m_Delay;
	float m_DelayEnd;
	float m_EndEnd;
	int gameended;

	void RecountTeams( void );
	const char *TeamWithFewestPlayers( void );
    void ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots, 
        int nDisplayTime, BOOL fNeedMore, char *pszText);
	
	BOOL m_DisableDeathMessages;
	BOOL m_DisableDeathPenalty;
	BOOL m_teamLimit;				// This means the server set only some teams as valid
	char m_szTeamList[TEAMPLAY_TEAMLISTLENGTH];
	bool m_bPlayedQuadSound;
	void CheckLTS( );
	void ResetRound( );
	void StartRound();
	void RoundRestart( void );
	void PlayerSpawn (CBasePlayer*);
	bool ReadyToStart( void );
	float m_iNextRoundTime;
    int CountPlayers(void);
	float time_remaining;
	float last_time;
	CDelayTime m_dRoundTimer;

	hudtextparms_t m_hMessage;
protected:
	virtual void AwardPoints( CBasePlayer *pVictim, CBasePlayer *pKiller, entvars_t *pInflictor );


};
