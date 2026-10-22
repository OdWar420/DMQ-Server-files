
//
// ctf_gamerules.h
//

#define MAX_CTF_TEAMNAME_LENGTH	16
#define MAX_CTF_TEAMS			2
// red team
#define CTF_TEAM1_NAME "Red"
#define CTF_TEAM1_COLOR "Red"
// blue team
#define CTF_TEAM2_NAME "Blue" 
#define CTF_TEAM2_COLOR "Blue"
#define CTF_TEAMLISTLENGTH		MAX_CTF_TEAMS*MAX_CTF_TEAMNAME_LENGTH

class CGameModeCTF : public CHalfLifeMultiplay
{
public:
	CGameModeCTF();

	
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	virtual BOOL IsCTF( void );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual BOOL ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual const char *GetGameDescription( void ) { return "DMQ2 : Mode CTF"; }  // this is the game name that gets seen in the server browser
	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Think ( void );
	virtual int GetTeamIndex( const char *pTeamName );
	virtual const char *GetIndexedTeamName( int teamIndex );
	virtual BOOL IsValidTeam( const char *pTeamName );
	const char *SetDefaultPlayerTeam( CBasePlayer *pPlayer );
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
    virtual void JoinTeam(CBasePlayer *pPlayer, const char *pTeamName);
	virtual int PlayersOnTeam( const char *szTeamName );
private:
	void RecountTeams( void );
	const char *TeamWithFewestPlayers( void );
    void ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots, 
        int nDisplayTime, BOOL fNeedMore, char *pszText);

	BOOL m_DisableDeathMessages;
	BOOL m_DisableDeathPenalty;
	BOOL m_teamLimit;				// This means the server set only some teams as valid
	char m_szTeamList[CTF_TEAMLISTLENGTH];
};
