#ifndef HLE_UTILITY_H
#define HLE_UTILITY_H


#define MATCHSTART_NONE 0
#define MATCHSTART_EMPTY 1
#define MATCHSTART_FULL 2

#define MAX_SPRITE_NUMBER 6
#define COUNTER_TIME 1


class CEntityNode
{

public:

	~CEntityNode();
	CEntityNode();

	edict_t* m_pEntity;
	CEntityNode* m_pNext;
};


class CEntityList 
{

private:

	CEntityNode *m_pHead;
	int	m_nSize;

public:
	CEntityList();
	~CEntityList();
	void push_back(edict_t* pNewEntity);
	int lookup(edict_t* pEntity);
	int size() { return m_nSize; }
	void clear();
	void clean();
	void remove(int nIndex);
	void remove(edict_t* pEntity);
	edict_t* operator [] ( int nIndex );
};



class CDelayTime
{
private:
	float m_fTime;
public:
	CDelayTime(){m_fTime = 0;}
	inline void AddTime(float fTime){m_fTime = gpGlobals->time + fTime;}
	inline void SubtractTime(float fTime) { m_fTime = gpGlobals->time - fTime;}
	inline BOOL TimeUp()
	{
		if(m_fTime==0) return FALSE;
		if(gpGlobals->time > m_fTime ) return TRUE;
		else return FALSE;
	}
	inline BOOL TimeUp(float fExtendedDelay)
	{
		if(m_fTime==0) return FALSE;
		if(gpGlobals->time > m_fTime+fExtendedDelay ) return TRUE;
		else return FALSE;
	}
	inline float GetTimeLeft()
	{ 
		if(m_fTime - gpGlobals->time  <=0) return 0;
		return m_fTime - gpGlobals->time;
	}
	inline void ClearTime() { m_fTime = 0; }
};

class CMatchStart /*: public CBaseEntity*/
{
	int m_nMatchStart;
	
	hudtextparms_t m_hudtMessage;
public:
	
	//void EXPORT ThinkSequence();
	int m_bInProgess;
	CMatchStart();
	void setMatch(int nMatch);
	int getMatch();
	int hasStarted();
	void hasStarted(bool bStarted);
	bool isMatch();
	CDelayTime m_dDelay;
};


void FillPlayerDefault(CBasePlayer *pPlayer);
void LoadPlayerDefault(CBasePlayer *pPlayer);
void ArmPlayerDefault(CBasePlayer *pPlayer);
void FillPlayer(CBasePlayer *pPlayer);
void LoadPlayer(CBasePlayer *pPlayer);
void ArmPlayer(CBasePlayer *pPlayer);
void FillPlayerMidair(CBasePlayer *pPlayer);
void LoadPlayerMidair(CBasePlayer *pPlayer);
void ArmPlayerMidair(CBasePlayer *pPlayer);
void ArmPlayerInstagib(CBasePlayer *pPlayer);
void LoadPlayerUncell(CBasePlayer *pPlayer);
void FillPlayerDMM(CBasePlayer *pPlayer);	
void LoadPlayerEX(CBasePlayer *pPlayer);	

void RemoveAllCorpses();
void RemoveAllEntites();
void RemoveAllPowerUps();
void ShowGameInfo(CBasePlayer* pPlayer);
void ShowGameHelp(CBasePlayer* pPlayer);
void ShowPlayerInfo(CBasePlayer* pPlayer);
void RespawnAllEntites();
void ResetCtfFlags();
void RemoveFlag(CBasePlayer* pPlayer);
void ShowGameVote(CBasePlayer* pPlayer);

#endif