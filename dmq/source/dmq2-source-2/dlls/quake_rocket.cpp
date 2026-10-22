//=========== (C) Copyright 1996-2002, Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Quake rocket entity
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"

#define GRENADE_TRAIL 1
#define ROCKET_TRAIL 2

extern unsigned short g_sTrail;
extern unsigned short g_sExplosion;
extern cvar_t gamemode;
extern cvar_t rltrail;
extern cvar_t rlflare;
LINK_ENTITY_TO_CLASS( quake_rocket, CQuakeRocket );

//=========================================================
CQuakeRocket *CQuakeRocket::CreateRocket( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner )
{
	CQuakeRocket *pRocket = GetClassPtr( (CQuakeRocket *)NULL );
	
	UTIL_SetOrigin( pRocket->pev, vecOrigin );
	
	pRocket->Spawn();


	if(FStrEq(gamemode.string, "midair"))
	{
		// Velocity
		if ( ((CBasePlayer*)pOwner)->m_flSuperDamageFinished > gpGlobals->time )
		{
			pRocket->pev->velocity = vecAngles * 2000;
			pRocket->pev->angles = UTIL_VecToAngles( vecAngles );
		}
		else
		pRocket->pev->velocity = vecAngles * 1000;
		pRocket->pev->angles = UTIL_VecToAngles( vecAngles );
	}
	else

	pRocket->pev->velocity = vecAngles * 1000;
	pRocket->pev->angles = UTIL_VecToAngles( vecAngles );

	pRocket->SetTouch( CQuakeRocket::RocketTouch );

	pRocket->pev->owner = pOwner->edict();

	pRocket->pev->angles = UTIL_VecToAngles( vecAngles );
	
	

	// Effects
	
	//mfrx 28FEB06
	if (CVAR_GET_FLOAT("rlflare") == 1)
	{
		pRocket->pev->effects |= EF_LIGHT;
	}
	//mfrx

	// Safety Remove
	//

//	DME 
	if ( CVAR_GET_FLOAT( "rltrail" ) == 0 )
	{
		PLAYBACK_EVENT_FULL (FEV_GLOBAL, pRocket->edict(), g_sTrail, 0.0, 
		(float *)&pRocket->pev->origin, (float *)&pRocket->pev->angles, 0.7, 0.0, pRocket->entindex(), ROCKET_TRAIL, 0, 0);
	}


	return pRocket;
} 


//=========================================================
void CQuakeRocket::Spawn( void )
{
	Precache();

	// Setup
	pev->movetype = MOVETYPE_FLYMISSILE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/rocket.mdl");
	
	UTIL_SetSize(pev, Vector( 0, 0, 0), Vector(0, 0, 0));
	UTIL_SetOrigin( pev, pev->origin );	
	
	pev->classname = MAKE_STRING("missile");

	SetThink( RocketThink );
	// Touch
	SetTouch( ExplodeTouch );

	if ( CVAR_GET_FLOAT( "rltrail" ) == 1 )
	{

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMFOLLOW );
			WRITE_SHORT(entindex());	// entity
			WRITE_SHORT(m_iTrail );	// model
			WRITE_BYTE( 10 ); // life
			WRITE_BYTE( 4 );  // width
			WRITE_BYTE( 224 );   // r, g, b
			WRITE_BYTE( 224 );   // r, g, b
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 250 );	// brightness

		MESSAGE_END();
	}
	pev->nextthink = gpGlobals->time + 0.2;

}

//=========================================================
void CQuakeRocket :: RocketThink( void  )
{


	/*DME - Custom Rockettrail = GOOD*/
	// rocket trail

	//if (CVAR_GET_FLOAT("rlflare") == 1)
	//{
	//	//TE_ELIGHT?
	//	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	//	WRITE_BYTE(TE_DLIGHT);
	//	WRITE_COORD(pev->origin.x);
	//	WRITE_COORD(pev->origin.y);
	//	WRITE_COORD(pev->origin.y);
	//	WRITE_BYTE(100);	//radius
	//	WRITE_BYTE(200);	//r
	//	WRITE_BYTE(100);	//g
	//	WRITE_BYTE(100);	//b
	//	WRITE_BYTE(10);	//life
	//	WRITE_BYTE(10);	//decay
	//	MESSAGE_END();
	//	//MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	//	//WRITE_BYTE(TE_GLOWSPRITE);	// coord, coord, coord (pos) short (model index) byte (scale / 10)
	//	//WRITE_COORD(pev->origin.x);
	//	//WRITE_COORD(pev->origin.y);
	//	//WRITE_COORD(pev->origin.y);
	//	//WRITE_SHORT(m_iFlare);
	//	//WRITE_BYTE(10);	//life * 10
	//	//WRITE_BYTE(10);	//model
	//	//WRITE_BYTE(200);	//brightness
	//	//MESSAGE_END();
	//	//WRITE_BYTE(TE_SPRITETRAIL);
	//	// coord, coord, coord (start) 
	//	// coord, coord, coord (end) 
	//	// short (sprite index)
	//	// byte (count)
	//	// byte (life in 0.1's) 
	//	// byte (scale in 0.1's) 
	//	// byte (velocity along vector in 10's)
	//	// byte (randomness of velocity in 10's)
	//}



float rheight;	
#define MAX_HEIGHT 8000 

	
		TraceResult tr;
		UTIL_TraceLine( pev->origin, (pev->origin + Vector(0,0,-MAX_HEIGHT)), ignore_monsters, pev->pContainingEntity, &tr);
		rheight = (pev->origin.z - tr.vecEndPos.z);

	if(FStrEq(gamemode.string, "midair"))
	{
		CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
		CBaseEntity *pOther = NULL;

		if((pOther = UTIL_FindEntityInSphere( pOther, pev->origin , 50)) != NULL)
		{
			if ( pOther != pOwner )
			{
				if ( pOther->pev->takedamage != DAMAGE_NO )
				{
					if (!(pOther->pev->flags & FL_ONGROUND))
					{
						
						ExplodeTouch(pOther);

					}
				
				}
			}
		}
	}
	/*if(FStrEq(gamemode.string, "midair"))
	{
		CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
		
		if ( pOwner->pev->flags & FL_ONGROUND)
		{

			if (rheight >= 101 && rheight <= 300)
			{
				UTIL_SetSize(pev, Vector( -6, -6, -6), Vector(6, 6, 6));
			}
			else if (rheight >= 301)
			{
				UTIL_SetSize(pev, Vector( -8, -8, -8), Vector(8, 8, 8));
			}
			else 
				UTIL_SetSize(pev, Vector( 0, 0, 0), Vector(0, 0, 0));
		}
		else
			UTIL_SetSize(pev, Vector( 0, 0, 0), Vector(0, 0, 0));
			
	

	}
	
	else
	
		UTIL_SetSize(pev, Vector( 0, 0, 0), Vector(0, 0, 0));*/
}

//=========================================================
void CQuakeRocket::Precache( void )
{
	m_iTrail = PRECACHE_MODEL("sprites/smoke.spr");
	//mfrx 01MAR05
	//custom rocket flare
	//m_iFlare = PRECACHE_MODEL("sprites/rocketflare.spr");
	//mfrx
	//m_iTrail = PRECACHE_MODEL("sprites/animglow01.spr");
}

//=========================================================
void CQuakeRocket::RocketTouch ( CBaseEntity *pOther )
{
	// Remove if we've hit skybrush
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	ExplodeTouch( pOther );
	
	/*
	// Do touch damage
	float flDmg = RANDOM_FLOAT( 100, 120 );
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if (pOther->pev->health)
	pOther->TakeDamage( pev, pOwner->pev, flDmg, DMG_BULLET );

	// Don't do radius damage to the other, because all the damage was done in the impact
	Q_RadiusDamage(this, pOwner, 120, pOther);

	// Finish and remove
	Explode();*/
}

void CQuakeRocket::ExplodeTouch( CBaseEntity *pOther )
{
	TraceResult tr;
	Vector		vecSpot;// trace starts here!

	pev->enemy = pOther->edict();
	pev->team = team;

	vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );
	float flDmg = RANDOM_FLOAT( 100, 120 );
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if (pOther->pev->health)
	pOwner->team=team;
	pOwner->pev->team=team;
	pOther->TakeDamage( pev, pOwner->pev, flDmg, DMG_BULLET );

	// Don't do radius damage to the other, because all the damage was done in the impact
	Q_RadiusDamage(this, pOwner, 120, pOther);
	Explode();
}

//=========================================================
CQuakeRocket *CQuakeRocket::CreateGrenade( Vector vecOrigin, Vector vecVelocity, CBaseEntity *pOwner )
{
	CQuakeRocket *pRocket = GetClassPtr( (CQuakeRocket *)NULL );

	UTIL_SetOrigin( pRocket->pev, vecOrigin );
	pRocket->Spawn();
	SET_MODEL(ENT(pRocket->pev), "models/grenade.mdl");
	SET_MODEL(ENT(pRocket->pev), "models/dmqgrenade.mdl");
	pRocket->pev->classname = MAKE_STRING("grenade");
	pRocket->pev->owner = pOwner->edict();

	// Setup
	pRocket->pev->movetype = MOVETYPE_BOUNCE;
	pRocket->pev->solid = SOLID_BBOX;

	pRocket->pev->avelocity = Vector(300,300,300);
	
	// Velocity
	pRocket->pev->velocity = vecVelocity;
	pRocket->pev->angles = UTIL_VecToAngles(vecVelocity);
	pRocket->pev->friction = 0;
	pRocket->pev->maxs[0]*=1.1;
	pRocket->pev->maxs[1]*=1.1;
	pRocket->pev->maxs[2]*=1.5;
	pRocket->pev->mins[0]*=1.1;
	pRocket->pev->mins[1]*=1.1;
	pRocket->pev->mins[2]*=1.5;

	pRocket->pev->absmax[0]*=1.1;
	pRocket->pev->absmax[1]*=1.1;
	pRocket->pev->absmax[2]*=1.5;
	pRocket->pev->absmin[0]*=1.1;
	pRocket->pev->absmin[1]*=1.1;
	pRocket->pev->absmin[2]*=1.5;

	// Touch
	pRocket->SetTouch( CQuakeRocket::GrenadeTouch );

	// set newmis duration
/*	if ( gpGlobals->deathmatch == 4 )
	{
		pRocket->m_flAttackFinished = gpGlobals->time + 1.1;	// What's this used for?
		if (pOwner)
			pOwner->TakeDamage( pOwner->pev, pOwner->pev, 10, DMG_GENERIC );
	}*/

	pRocket->pev->nextthink = gpGlobals->time + 2.5;
	pRocket->SetThink( CQuakeRocket::GrenadeExplode );
	pRocket->pev->iuser4=0;
	pRocket->pev->vuser1 = Vector(999,999,999);
	pRocket->pev->fuser2=gpGlobals->time-10;
	pRocket->pev->fuser3=-1000000;

//	PLAYBACK_EVENT_FULL (FEV_GLOBAL, pRocket->edict(), g_sTrail, 0.0, 
//	(float *)&g_vecZero, (float *)&g_vecZero, 0.7, 0.0, pRocket->entindex(), GRENADE_TRAIL, 0, 0);
//	PLAYBACK_EVENT_FULL (FEV_GLOBAL, pRocket->edict(), g_sTrail, 0.0, 
//	(float *)&g_vecZero, (float *)&g_vecZero, 0.7, 0.0, pRocket->entindex(), ROCKET_TRAIL, 0, 0);



	return pRocket;
}

//=========================================================
void CQuakeRocket::GrenadeTouch( CBaseEntity *pOther )
{
	if (pOther->pev->takedamage == DAMAGE_AIM)
	{
		GrenadeExplode();
		return;
	}

	
   	vec3_t vel;
	bool comeoffwall=false;

	TraceResult trace;

	int removex, removey;
	
	UTIL_TraceLine( pev->origin - Vector(4,4,0), pev->origin + Vector(4,4,0),  ignore_monsters, ENT(pev), &trace );

	vel = pev->velocity;

	if(pev->velocity[0]>0)
	{
		pev->oldorigin[0]-=1;
		pev->origin[0]-=1;
		removex=4;
	}
	else
	{
		pev->oldorigin[0]+=1;
		pev->origin[0]+=1;
		removex=-4;

	}
	if(pev->velocity[1]>0)
	{
		pev->oldorigin[1]-=1;
		pev->origin[1]-=1;
		removey=4;
	}
		else
	{
		pev->oldorigin[1]+=1;
		pev->origin[1]+=1;
		removey=-4;

	}
	UTIL_TraceLine( pev->origin - Vector(removex,removey,0), pev->origin + Vector(removex,removey,0),  ignore_monsters, ENT(pev), &trace );




	if(!(trace.vecPlaneNormal[2] <0.7 && trace.flFraction<1))
	{





if(pev->velocity[2]<0)
{

		if(gpGlobals->time - pev->fuser2 >0.4)//&&(pev->angles[0]!=pev->vuser1[0]||pev->angles[1]!=pev->vuser1[1]))
		{

		 		pev->velocity[2]*=0.5;					
				pev->vuser1[0]=pev->angles[0];
				pev->vuser1[1]=pev->angles[1];
				pev->vuser1[2]=pev->angles[2];
				pev->fuser2=gpGlobals->time;

	//	pev->velocity[1]*=0.9;
	//	pev->velocity[0]*=0.9;
		}
		else
		{
			
			pev->velocity[2]*=0;
			pev->velocity[1]*=0;
			pev->velocity[0]*=0;

		}
}
else
{
		if(pev->velocity[2]>pev->fuser3*2)
		{
		if(gpGlobals->time - pev->fuser2 <0.05)
		{
			pev->velocity[2]*=0;
			pev->velocity[1]*=0;
			pev->velocity[0]*=0;
		}
		else
		{
			pev->vuser1=Vector(999,999,999);
			pev->fuser2=gpGlobals->time-10;
		}
		}
}
pev->fuser3=pev->velocity[2];
	}
	else
	{

		pev->velocity[2]*=0.8;
		pev->velocity[1]*=0.7;
		pev->velocity[0]*=0.7;
	}




	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/bounce.wav", 1, ATTN_NORM);

	if (pev->velocity == g_vecZero)
		pev->avelocity = g_vecZero;
}

void CQuakeRocket::GrenadeExplode()
{
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	Q_RadiusDamage(this, pOwner, 120, NULL);

	// Finish and remove
	Explode();
}
//mfrx 14FEB06
void CQuakeRocket::GrenadeExplode(float fDmg)
{
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	Q_RadiusDamage(this, pOwner, fDmg, NULL);

	// Finish and remove
	Explode();
}
//mfrx
//=========================================================
void CQuakeRocket::Explode()
{
	//We use the angles field to send the rocket velocity.
	PLAYBACK_EVENT_FULL( FEV_GLOBAL, edict(), g_sExplosion, 0.0, (float *)&pev->origin, (float *)&pev->velocity, 0.0, 0.0, 0, 0, 0, 0 );

	UTIL_Remove( this );
}