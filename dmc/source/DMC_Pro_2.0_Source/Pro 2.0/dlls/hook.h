// SCTF 
class CHook : public CBaseEntity 
{ 
    void Spawn( void ); 
    int  Classify ( void ); 
    void EXPORT Touch( CBaseEntity *pOther ); 
    void Think ( void ); 
    BOOL m_fPlayerAtEnd; 
    short m_bCableSprite; 
    BOOL m_fHookInWall; 
    int m_hookRed; 
    int m_hookGreen; 
    int m_hookBlue; 
    int m_hookBright; 
    CBaseEntity *m_targetEntity;     // entity that the hook hits 
public: 
   static CHook *CreateHook();    // factory function 
    void KillHook( void ); 
    void Precache( void ); 
    CBasePlayer *pevOwner; // player who created the hook 
}; 

// How fast the hook will shoot out 
//#define HOOK_THROW_SPEED grapple_airspeed.value 
// How fast the hook will pull the player 
//#define HOOK_PULL_SPEED grapple_dragspeed.value 

// SCTF
