#include "extdll.h" 
#include "util.h" 
#include "cbase.h" 
#include "weapons.h" 
#include "gamerules.h" 
#include "player.h" 
#include "hook.h"

extern cvar_t	grappledragspeed; 
// This will enable precaching of the hook 
LINK_ENTITY_TO_CLASS( grapple_hook, CHook );

//This static function is called when the player creates a hook entity.  By the time it returns, the hook is fully initialized and ready to fly. 
CHook *CHook::CreateHook( void ) 
{ 
    // Perform entity initialization 
    CHook *pHook = GetClassPtr( (CHook *)NULL ); 
    pHook->pev->classname = MAKE_STRING("hook");  // unique identifier 
    pHook->Spawn();       // perform instance initialization 
    return pHook; 
}
//The function Spawn establishes the look, feel, and basic properties of the hook.  Normally the engine calls Spawn() for each entity it creates on a map.  But the player creates the hook, so it will be called from our code. 
void CHook::Spawn() 
{ 
    Precache( ); // load resources used by the hook 
    m_hookRed = 255; 
    m_hookGreen = 255; 
    m_hookBlue = 255; 
    m_hookBright = 255; 
   // Set the basic properties.  The hook will fly, but will eventually be dragged down to the ground 
   pev->movetype = MOVETYPE_FLY; 
   pev->solid = SOLID_BBOX; 
   pev->gravity = 0.5; 

    // Hook has the look and sound of a crossbow bolt. This is one of the best matches between 
    // hook behavior and available entities I have seen.. 
    SET_MODEL(ENT(pev), "models/crossbow_bolt.mdl");

    // Link the hook into the world 
    UTIL_SetOrigin( pev, pev->origin ); 
    // Hooks are point-objects.  They do not interact with entities unless they contact the origin. 
    UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0)); 

    // This function will be called when any entity touches the hook origin. 
    SetTouch( CHook::Touch ); 

    // The hook will update its state every 1/10 second. 
    SetThink( CHook::Think ); 
    pev->nextthink = gpGlobals->time + 0.1; 

    // Becomes true when the hook hits the target entity 
    m_fHookInWall = FALSE; 

    // Set when the hook hits something 
    m_targetEntity = 0; 

}

//This function marshalls the models, sounds, and sprites used by the hook 
void CHook::Precache( ) 
{ 
	PRECACHE_SOUND("weapons/xbow_fire1.wav");
    PRECACHE_SOUND("weapons/xbow_hitbod1.wav"); 
    PRECACHE_SOUND("weapons/xbow_hitbod2.wav"); 
    PRECACHE_SOUND("weapons/xbow_fly1.wav"); 
    PRECACHE_SOUND("weapons/xbow_hit1.wav"); 
    PRECACHE_SOUND("fvox/beep.wav"); 
    PRECACHE_MODEL ("models/crossbow_bolt.mdl"); 
    m_bCableSprite = PRECACHE_MODEL("sprites/rope.spr"); 
}
//This function is called by the engine to determine the hook's place in relation to other entities.  Since our hook is not directly managed by the engine, no special class information is returned. 
int CHook :: Classify ( void ) 
{ 
    return  CLASS_NONE; 
}
//This function is called when the hook is to be destroyed.  Either the player or the hook itself can call this function at any time.  The hook will be removed and the player internal hook state will be reset. 
void CHook::KillHook( void ) { 
    pevOwner->pev->movetype = MOVETYPE_WALK; 
    pevOwner->pev->gravity = 1;
//	pevOwner->pev->friction = 1;
    pevOwner->pev->iuser4 = 0;  // tell physics to allow moving. 
    pevOwner->pHook = 0; 
    SUB_Remove(); 
}
//This function is called when any entity touches the hook.  The hook will transition from the flying state to the pulling state here. pOther is the entity that has touched the hook.  If the target can be hurt, the hook will damage it. 
void CHook::Touch( CBaseEntity *pOther ) 
{ 
    // This hook only has one chance to embed itself and start pulling.  If something goes wrong, 
    // the hook will be destroyed. 
    SetTouch( NULL ); 
    // If the hook contacts a dead entity, do not try to interact with it, in case the engine removes it.. 
    if ( pOther->pev->deadflag == DEAD_DEAD ) 
    { 
        KillHook(); 
        return; 
    } 

    // If the entity that touched the hook can take damage, then damage it. 
    if ( pOther->pev->takedamage ) 
    { 
        // Initialize a trace object from the current global state 
        TraceResult tr = UTIL_GetGlobalTrace( ); 
        entvars_t   *Owner; 
        Owner = VARS( pev->owner ); 

        // The hook will now damage the touching entity.  Any time the TraceAttack() and 
        // MultiDamage() functions are called, care must be taken to maintain the global variable 
        // gMultiDamage which is used inside these functions. 

        // establish an attacking posture with regards to the target 
        pOther->TraceAttack(Owner, 5, pev->velocity.Normalize(), &tr, DMG_NEVERGIB ); 

        // apply a unit of damage 
        ApplyMultiDamage( pev, Owner ); 

        // reset the global damage entity, in case it dies. 
        gMultiDamage.pEntity = NULL; 

        // release the hook if the damaged entity has died. 
        if ( pOther->pev->deadflag == DEAD_DEAD )
		{ 
            KillHook(); 
            return; 
        } 

		if ( pOther->pev->health <= 0 )
		{ 
            KillHook(); 
            return; 
        } 
        // Make a noise for hitting a hurtable target. 
        switch( RANDOM_LONG(0,1) ) 
        { 
            case 0: 
               EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/xbow_hitbod1.wav", 
               1, ATTN_NORM); 
                break; 
            case 1: 
                EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/xbow_hitbod2.wav", 
                1, ATTN_NORM); 
                break; 
        } 
    } 
    else 
    { 
        // Entity was not hurtable, so just make a thwok noise 
        EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/xbow_hit1.wav", 
        RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 98 + RANDOM_LONG(0,7)); 
    } 

    // Store a reference to the target in case it moves or goes away 
    m_targetEntity = pOther; 

    // Embed the hook in the target 
    Vector vecDir = pev->velocity.Normalize( ); 
    UTIL_SetOrigin( pev, pev->origin - vecDir * 12 ); 
    pev->angles = UTIL_VecToAngles( vecDir ); 

    // Halt the forward motion of the hook.  From this point on, it will follow the target around, 
    // if the target is in motion.. 
    pev->velocity = Vector( 0, 0, 0 ); 
    pev->avelocity.z = 0; 
    pev->angles.z = RANDOM_LONG(0,360); 

    // Each time think() is called, it must reset the nextthink timer in order to think again. 
    pev->nextthink = gpGlobals->time + 0.1; 

    // Keep track of stuff so there is only one 
    // active hook per player. 
    m_fHookInWall = TRUE; 

    // Now the player must allow herself to be pulled by the hook 
    pevOwner->pev->movetype = MOVETYPE_FLY; 

    // Visual effect 
    UTIL_Sparks( pev->origin ); 
}

//The hook will think every 1/10 second.  The vector of motion will be reset as needed, and the velocity reset.  If anything happens to the target, the hook will be destroyed. Thinking begins when the hook is launched, continues when the hook is set and the player pulled up, and still continues after the player reaches the hook.  This function must be able to detect all three of these states and handle the player and hook motion appropriately.. 
void CHook::Think ( void ) 
{ 
    //Check to make use the player is not dead 
    if(!pevOwner->IsAlive( ) ) 
    { 
        KillHook(); 
        return; 
    } 

    // Check to make sure the target has not died. 
    if ( m_targetEntity && m_targetEntity->pev->deadflag == DEAD_DEAD ) 
    { 
        KillHook(); 
        return; 
    } 

    // Always update the sprite visual for the client 
    MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY ); //Draw the rope 
    WRITE_BYTE( TE_BEAMENTS ); 
    WRITE_SHORT( pevOwner->entindex() ); 
    WRITE_SHORT( this->entindex() ); 
    WRITE_SHORT( m_bCableSprite ); 
    WRITE_BYTE( 1 ); 
    WRITE_BYTE( 0 ); 
    WRITE_BYTE( 1 ); 
    WRITE_BYTE( 10 ); 
    WRITE_BYTE( 0 ); 
    // r+g=yellow 
    // g+b=teal 
    // r+b=violet 
    WRITE_BYTE( m_hookRed ); //Red 
    WRITE_BYTE( m_hookGreen ); //Green 
    WRITE_BYTE( m_hookBlue ); //blue 
    WRITE_BYTE( m_hookBright ); //bright 
    WRITE_BYTE( 10); 
    MESSAGE_END( ); 

    // The player is pulled only if the hook has reached the wall but has not yet reached the hook 
    if( m_fHookInWall ){ 
        // Adjust the hook position if necessary 
        // handle nonliving but moving entities (slidewalks, doors, etc) 
        if (m_targetEntity->pev->solid == SOLID_SLIDEBOX) 
        { 
            // follow the motion of the object 
            pev->velocity = Vector( 0, 0, 0 ); 
            UTIL_SetOrigin( pev, m_targetEntity->pev->origin + 
                  m_targetEntity->pev->mins + (m_targetEntity->pev->size * 0.5)); 
        } 
        else 
        { 
            // follow the motion of the player or monster 
            pev->velocity = m_targetEntity->pev->velocity; 
        } 

        if(!m_fPlayerAtEnd) { 
            // player is in motion 
            if(( pev->origin - pevOwner->pev->origin ).Length() >= 50.0) { 

                // Calculate a normal (unit direction) vector from player to target. 
                Vector v = pev->origin - pevOwner->pev->origin; 
                v = v.Normalize(); 

                // Convert normal vector into a velocity and apply it 
                float m_playerHookSpeed = grappledragspeed.value; 
                v = v * m_playerHookSpeed; 
                pevOwner->pev->velocity = v; 
                pevOwner->pev->speed = m_playerHookSpeed;
				pevOwner->pev->gravity = 0.5;
            } else { 
                // Player is no longer in motion once they get reasonably close to the hook. 
                m_fPlayerAtEnd = TRUE; 
            } 

        } 
        if(m_fPlayerAtEnd){ 
            // A halted player will have no motion, although they can turn and tilt. 
            pevOwner->pev->velocity = Vector(0, 0, 0); 
            pevOwner->pev->gravity = 0;
		//	pevOwner->pev->friction = 0;//trend
            pevOwner->pev->speed = 0; 

            // This is a special code we will use to inform the physics that the player must not move at all. 
            // There are a limited number of these general purpose variables, so use them only at need. 
            // The physics only has the player entvars (the pev variable), so we must use an entvars variable 
            // to control it. 
            pevOwner->pev->iuser4 = 2;  // tell physics to stop moving. 
        } 
    } 

    // Reset the think timer. 
    pev->nextthink = gpGlobals->time + 0.10; //set delay untill next think 

}
