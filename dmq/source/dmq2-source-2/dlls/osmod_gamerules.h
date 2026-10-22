#ifndef OSMOD_GAMERULES_H
#define OSMOD_GAMERULES_H

//#include "item_generic.h"

#define LF_GAMERULE_OBSERVER (1<<15)

/*
=====================================================
Teamplay
=====================================================
*/


#define MAX_TEAMNAME_LENGTH	16
#define MAX_TEAMS			32

#define TEAMPLAY_TEAMLISTLENGTH		MAX_TEAMS*MAX_TEAMNAME_LENGTH

//#define TEAM1 "Red team"
//#define TEAM2 "Blue team"

void ShowVGUI( CBasePlayer *pPlayer, int iMenu );

#define MENU_DEFAULT				1
#define MENU_TEAM 					2
#define MENU_CLASS 					3
#define MENU_MAPBRIEFING			4
#define MENU_INTRO 					5
#define MENU_CLASSHELP				6
#define MENU_CLASSHELP2 			7
#define MENU_REPEATHELP 			8

extern char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
extern int team_scores[MAX_TEAMS];
extern int team_deaths[MAX_TEAMS];
extern int num_teams;

class COSModTeamplay : public CHalfLifeMultiplay
{
private:
	
	const char *TeamWithFewestPlayers( void );

	BOOL m_DisableDeathMessages;
	BOOL m_DisableDeathPenalty;
	BOOL m_teamLimit;				// This means the server set only some teams as valid
	char m_szTeamList[TEAMPLAY_TEAMLISTLENGTH];

	char *unused;
public:
	COSModTeamplay();

	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	virtual BOOL IsTeamplay( void );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual BOOL ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual const char *GetGameDescription( void ) { return "OSMod Team Deathmatch"; }  // this is the game name that gets seen in the server browser
	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Think ( void );
	virtual int GetTeamIndex( const char *pTeamName );
	virtual const char *GetIndexedTeamName( int teamIndex );
	virtual BOOL IsValidTeam( const char *pTeamName );
	const char *SetDefaultPlayerTeam( CBasePlayer *pPlayer );
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
	virtual void JoinTeam( CBasePlayer *pPlayer, const char *pTeamName );
	virtual void ResetRound( );

	virtual BOOL IsOSTeamplay( void ) // Used For team Spawn Points
	{ 
		return TRUE; 
	};

	void RecountTeams( bool bResendInfo = FALSE );
};

/*
=====================================================
Vampire
Killer gets victim's frags
=====================================================

class COSModVamp : public CHalfLifeMultiplay
{
public:
	virtual const char *GetGameDescription( void ) { return "OSMod Vampire"; }
protected:
	virtual void AwardPoints( CBasePlayer *pVictim, CBasePlayer *pKiller, entvars_t *pInflictor );
};
*/
/*
=====================================================
LMS: Last Man Standing
Last living player gets a point
=====================================================
*/
class COSModLMS : public CHalfLifeMultiplay
{
private:
	CBasePlayer* FindWinner( void );
	CBasePlayer* FindLivingPlayer( void );
	void NormalThink( void );
	void DelayThink( void );
	void StartDelay( CBasePlayer* pWinner );

	int m_Delay;
	float m_DelayEnd;
protected:
	virtual void AwardPoints( CBasePlayer *pVictim, CBasePlayer *pKiller, entvars_t *pInflictor );
public:
	COSModLMS();
	virtual const char *GetGameDescription( void ) { return "DMQ2 : Mode LMS"; }
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
	virtual void Think( void );
};

/*
=====================================================
Capture The "Thing"
=====================================================



// The THING! ( lol wtf )
// Well who would want to name such a thing 'thing'??

class CCTFThing : public CItemGeneric
{
public:
	void Spawn( void );
	void Touch( CBaseEntity *pOther );
	virtual void Event(item_generic_event_t event, CBaseEntity* pActivator=NULL);
};

// Capture Zone

class CCaptureZone : public CTriggerItem
{
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void Touch( CBaseEntity *pOther );
	int  CapturePoints;
};

class COSModCTF : public COSModTeamplay
{
public:
	COSModCTF();
	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	virtual const char *GetGameDescription( void ) { static char sz[256]; sprintf(sz,"OSMod Capture the %s",m_sThingName?STRING(m_sThingName):"flag"); return sz; }
	void ResetRound( BOOL fCaptured, const char* pszTeam=NULL );
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
	virtual void Think( void );
	virtual void HandleVars( gamemodevars_t* pgmVars );
	float m_flTimer;
	float m_flRoundTime; //in minutes!
	float m_flWaitTime;
	string_t m_sThingName; //WHO THOUGHT OF THAT 'THING' THIS IS RIDICULOUS
};

*/

#endif