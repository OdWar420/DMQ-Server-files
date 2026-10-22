#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "client.h"
#include "decals.h"
#include "effects.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"

class CPlayerCorpse : public CBaseMonster
{
public:
	void Spawn( void );
	void EXPORT FadeThink( void );
	void InitCorpse( entvars_t *evars );
	int Classify ( void ) { return CLASS_PLAYER; }
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
};

void CPlayerCorpse::InitCorpse( entvars_t *evars )
{
	SET_MODEL(ENT(pev), STRING(evars->model));
	UTIL_SetOrigin(pev, evars->origin);
	vec3_t mins, maxs;
	mins = evars->mins;
	maxs = evars->maxs;
	maxs.z = mins.z + 10;
	UTIL_SetSize(pev, mins, maxs);
	pev->angles   = evars->angles;
	pev->modelindex  = (0 + evars->modelindex);
	pev->frame   = evars->frame;
	pev->colormap  = (0 + evars->colormap);
	pev->velocity  = evars->velocity;
	pev->sequence  = evars->sequence;
	pev->animtime  = evars->animtime;
}

void CPlayerCorpse::Spawn( void )
{
	pev->classname     = MAKE_STRING("player_corpse");
	pev->rendermode     = kRenderNormal;
	pev->renderamt      = 255;
	pev->rendercolor.x  = 0;
	pev->rendercolor.y  = 0;
	pev->rendercolor.z  = 0;
	pev->renderfx       = kRenderFxNone;
	pev->solid          = SOLID_BBOX;
	pev->movetype       = MOVETYPE_TOSS;
	pev->deadflag       = DEAD_DEAD;
	pev->nextthink      = gpGlobals->time + 25.0;
	pev->takedamage     = 1.0;
	pev->health         = 25;
	m_bloodColor  = BLOOD_COLOR_RED;
	SetThink(FadeThink);
	InitBoneControllers();
}

void CPlayerCorpse :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if ( pev->takedamage )
	{
	 SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
	 TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
	 AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
	}
}

void CPlayerCorpse::FadeThink( void )
{
	pev->rendermode = kRenderTransAlpha;
	pev->renderamt -= 5;
	pev->nextthink = gpGlobals->time + 0.1;
	if( pev->renderamt <= 0 )
	{
	 SetThink(SUB_Remove);
	}
}

void CopyToBodyQue(entvars_t *pev)
{
	if (pev->effects & EF_NODRAW)
	 return;
	CPlayerCorpse *pGib = GetClassPtr( (CPlayerCorpse *)NULL );
	if( !pGib )
	{
	 ALERT( at_console, "Failed to spawn player corpse." );
	 return;
	}
	pGib->InitCorpse( pev );
	pGib->Spawn();
}